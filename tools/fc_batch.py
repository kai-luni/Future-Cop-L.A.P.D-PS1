#!/usr/bin/env python3
"""Mechanical, byte-safe renaming of retail symbols in a matching decomp unit set.

Subcommands
-----------
plan   <units.txt>                     : per-unit facts for naming decisions (JSON)
bind   <units.txt> <mapping.json>      : rename functions/globals, keep retail labels
verify <units.txt>                     : compile the set and require exact bytes

mapping.json format::

    {"functions": {"func_8001072C": "gGteProjectVertex"},
     "globals":   {"D_8009F998": "gQueueWriteIndex"}}

Rules encoded here (see the repo's symbol-renaming conventions):
* identifiers are rewritten in code and comments only, never inside string
  literals, so ``__asm__("func_XXXXXXXX")`` survives untouched;
* every top-level ``extern`` declaration of a renamed name gets its own
  ``__asm__("<retail>")`` binding, multi-declarator statements are split;
* a renamed definition gets ``RET name(PARAMS) __asm__("func_XXXXXXXX");``
  directly above its documentation block, not at the top of the file, because a
  unit's own struct/typedef block lives there.
"""
import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TOOLS_DIR = Path(__file__).resolve().parent
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))
import unit_paths  # noqa: E402
MANIFEST = ROOT / "config" / "us" / "matching_units.json"

IDENT = re.compile(r"[A-Za-z_]\w*")
TOKEN = re.compile(r'(/\*.*?\*/)|(//[^\n]*)|("(?:\\.|[^"\\])*")')
DEF = re.compile(
    r"^([A-Za-z_][A-Za-z0-9_ \t\*]*?)\b([A-Za-z_]\w*)\s*(\([^;{}]*\))\s*\{", re.M
)
KEYWORDS = {"if", "for", "while", "switch", "return", "sizeof", "do", "else"}
TYPE_WORDS = {
    "int", "char", "short", "long", "unsigned", "signed", "void", "float",
    "double", "const", "volatile", "struct", "union", "enum", "u8", "u16",
    "u32", "u64", "s8", "s16", "s32", "s64", "bool", "extern",
}


def mask(text):
    """Blank out comments and string literals, preserving offsets and newlines."""
    out = list(text)
    for match in TOKEN.finditer(text):
        for i in range(match.start(), match.end()):
            if out[i] != "\n":
                out[i] = " "
    return "".join(out)


def rename_identifiers(text, mapping):
    """Rewrite identifiers in code, never inside comments or string literals.

    Comments keep the retail names on purpose: the documentation blocks cite
    ``func_XXXXXXXX`` and ``D_XXXXXXXX`` so a reader can look them up in the
    disassembly.
    """
    literals = [(m.start(), m.end()) for m in TOKEN.finditer(text)]
    out, last = [], 0
    for match in IDENT.finditer(text):
        start, end = match.span()
        if any(begin <= start < finish for begin, finish in literals):
            continue
        if match.group(0) in mapping:
            out.append(text[last:start])
            out.append(mapping[match.group(0)])
            last = end
    out.append(text[last:])
    return "".join(out)


def statement_spans(masked):
    """(start, end) of every top-level ``extern ...;`` statement."""
    spans = []
    depth = 0
    begin = 0
    for index, char in enumerate(masked):
        if char == "{":
            depth += 1
        elif char == "}":
            depth -= 1
            if depth == 0:
                begin = index + 1
        elif char == ";" and depth == 0:
            chunk = masked[begin:index + 1]
            stripped = chunk.lstrip()
            if stripped.startswith("extern"):
                spans.append((begin + len(chunk) - len(stripped), index + 1))
            begin = index + 1
    return spans


def split_top_level(masked, start, end):
    """Split ``masked[start:end]`` at commas outside brackets/parentheses."""
    pieces, depth, piece_start = [], 0, start
    for index in range(start, end):
        char = masked[index]
        if char in "([":
            depth += 1
        elif char in ")]":
            depth -= 1
        elif char == "," and depth == 0:
            pieces.append((piece_start, index))
            piece_start = index + 1
    pieces.append((piece_start, end))
    return pieces


def declarator_name(core_masked):
    """Name declared by a declarator without its ``__asm__`` suffix."""
    pointer = re.search(r"\(\s*\*\s*([A-Za-z_]\w*)\s*\)", core_masked)
    if pointer:
        return pointer.group(1)
    names = IDENT.findall(re.sub(r"\[[^\]]*\]", "", core_masked))
    return names[-1] if names else None


def rewrite_declaration(masked, text, start, end, mapping):
    """Return the replacement lines for one extern statement, or None."""
    statement_masked = masked[start:end]
    body = statement_masked.rstrip(";").rstrip()
    inner_start = start + body.index("extern") + len("extern")
    inner_end = start + len(body)
    spans = split_top_level(masked, inner_start, inner_end)
    cores, names, labels = [], [], []
    for piece_start, piece_end in spans:
        piece_masked = masked[piece_start:piece_end]
        asm_at = piece_masked.find("__asm__")
        core_masked = piece_masked if asm_at < 0 else piece_masked[:asm_at]
        names.append(declarator_name(core_masked))
        cores.append(core_masked.rstrip())
        label = re.search(r'__asm__\s*\(\s*"([^"]*)"',
                          text[piece_start:piece_end])
        labels.append(label.group(1) if label else None)
    if not any(name in mapping or label in mapping
               for name, label in zip(names, labels)):
        return None

    tokens = [core.split() for core in cores]
    type_tokens = []
    for word in tokens[0]:
        if word in TYPE_WORDS:
            type_tokens.append(word)
        else:
            break
    if not type_tokens:
        return None
    type_words = " ".join(type_tokens)
    declarators = []
    for index, core in enumerate(cores):
        words = core.split()
        declarators.append(" ".join(words if index else words[len(type_tokens):]))

    lines = []
    for (piece_start, piece_end), core_masked, name, declarator in zip(
            spans, cores, names, declarators):
        piece_text = text[piece_start:piece_end]
        existing = re.search(r'__asm__\s*\(\s*"([^"]*)"', piece_text)
        retail = existing.group(1) if existing else None
        declarator = " ".join(declarator.split())
        # The mapping is keyed by retail symbol; the C name may already differ.
        if name in mapping:
            target = mapping[name]
        elif retail in mapping:
            target = mapping[retail]
        else:
            target = None
        if target:
            assert name
            renamed = re.sub(r"\b%s\b" % re.escape(name), target, declarator, count=1)
            lines.append(f'extern {type_words} {renamed} __asm__("{retail or name}");')
        elif retail:
            lines.append(f'extern {type_words} {declarator} __asm__("{retail}");')
        else:
            lines.append(f"extern {type_words} {declarator};")
    return "\n".join(lines)


def refers_to(masked, text, retail):
    """True when a file mentions the retail symbol in code or as an asm label."""
    if re.search(r"\b%s\b" % re.escape(retail), masked):
        return True
    return f'"{retail}"' in text


def bind_file(path, functions, globals_):
    """Apply the mapping to one unit file. Returns True when it changed."""
    original = path.read_text(errors="surrogateescape")
    text = original
    masked = mask(text)

    mapping = {retail: cname for retail, cname in functions.items()
               if refers_to(masked, text, retail)}
    mapping.update({retail: cname for retail, cname in globals_.items()
                    if refers_to(masked, text, retail)})
    if not mapping:
        return False

    # 1. Bind every declaration of a renamed name before renaming it.
    replacements = []
    for start, end in statement_spans(masked):
        replacement = rewrite_declaration(masked, text, start, end, mapping)
        if replacement is not None:
            replacements.append((start, end, replacement))
    for start, end, replacement in sorted(replacements, reverse=True):
        text = text[:start] + replacement + text[end:]
    masked = mask(text)

    # 2. Rename identifiers in code and comments; asm labels stay untouched.
    text = rename_identifiers(text, mapping)
    masked = mask(text)

    # 3. Give each renamed definition its own asm-labelled prototype, directly
    #    above its documentation block.
    inserts = []
    for match in DEF.finditer(masked):
        spec, name, params = match.group(1), match.group(2), match.group(3)
        if name in KEYWORDS or "static" in spec:
            continue
        retail = next((r for r, c in mapping.items()
                       if c == name and r.startswith("func_")), None)
        if not retail:
            continue
        if re.search(r"\b%s\s*\([^;{}]*\)\s*__asm__\(" % re.escape(name), masked):
            continue
        inserts.append((match.start(), f'{spec}{name}{params} __asm__("{retail}");'))

    for offset, prototype in sorted(inserts, reverse=True):
        lines = text[:offset].splitlines(keepends=True)
        insert_at = len(lines)
        if lines and not lines[-1].strip() and len(lines) > 1:
            insert_at = len(lines) - 1          # the blank line before the body
        if lines and lines[insert_at - 1].strip().endswith("*/"):
            for index in range(insert_at - 1, -1, -1):
                if "/*" in lines[index]:
                    insert_at = index
                    break
        prefix = "".join(lines[:insert_at])
        suffix = text[len(prefix):]
        if prefix and not prefix.endswith("\n\n"):
            prefix = prefix.rstrip("\n") + "\n\n"
        text = prefix + prototype + "\n\n" + suffix.lstrip("\n")

    # 4. Guard rails: the rename must not lose a body, a label or a declaration.
    for retail, cname in functions.items():
        if not re.search(r"\b%s\s*\([^;{}]*\)\s*\{" % re.escape(retail), masked):
            continue
        if not re.search(r"\b%s\s*\([^;{}]*\)\s*\{" % re.escape(cname), mask(text)):
            raise RuntimeError(f"{path}: body of {cname} disappeared")
    if re.search(r"__asm__\s*\(\s*\)", text):
        raise RuntimeError(f"{path}: empty __asm__ label after rewrite")
    for retail in mapping:
        if f'"{retail}"' in original and f'"{retail}"' not in text:
            raise RuntimeError(f"{path}: retail label {retail} lost")

    if text != original:
        path.write_text(text, errors="surrogateescape")
        return True
    return False


def load_units(source):
    path = Path(source)
    if path.suffix == ".json":
        return [unit["source"] for unit in json.loads(path.read_text())["units"]
                if unit["source"].startswith("src/game/")]
    resolved = []
    for token in path.read_text().split():
        token = token.strip()
        if not token:
            continue
        if (ROOT / token).is_file():
            resolved.append(token)
            continue
        found = unit_paths.resolve_unit_path(token, repo=ROOT, manifest_path=MANIFEST)
        if found is None:
            raise FileNotFoundError(f"cannot resolve unit {token!r} in {source}")
        resolved.append(found.relative_to(ROOT).as_posix())
    return resolved


def environment():
    result = dict(os.environ)
    tool = ROOT / ".toolchain/mipsel-binutils/root/usr/bin"
    lib = ROOT / ".toolchain/mipsel-binutils/root/usr/lib/x86_64-linux-gnu"
    if tool.is_dir():
        result["PATH"] = f"{tool}:{result.get('PATH', '')}"
        result["LD_LIBRARY_PATH"] = f"{lib}:{result.get('LD_LIBRARY_PATH', '')}"
    # Keep temporary build dirs inside the tree: the sandbox only allows writes here.
    scratch = ROOT / "build/fcset-tmp"
    scratch.mkdir(parents=True, exist_ok=True)
    result["TMPDIR"] = str(scratch)
    result["PYTHONDONTWRITEBYTECODE"] = "1"
    return result


def verify(paths, jobs=None):
    manifest = json.loads(MANIFEST.read_text())
    subset = dict(manifest)
    subset["units"] = [unit for unit in manifest["units"]
                       if unit["source"] in set(paths)]
    tmp = Path(tempfile.mkdtemp(prefix="fcset-", dir=environment()["TMPDIR"]))
    config, report = tmp / "set.json", tmp / "report.json"
    config.write_text(json.dumps(subset, indent=1))
    command = [str(ROOT / ".venv/bin/python"),
               str(ROOT / "tools/verify_matching_units.py"),
               "--config", str(config), "--output", str(report)]
    if jobs:
        command += ["--jobs", str(jobs)]
    return subprocess.run(command, cwd=ROOT, env=environment(),
                          capture_output=True, text=True)


def gate(paths):
    """Run the whole acceptance chain for a unit set and report each step."""
    steps = [
        ("set bytes", [str(ROOT / ".venv/bin/python"), str(ROOT / "tools/fc_batch.py"),
                       "verify", str(paths)]),
        ("function map", [str(ROOT / ".venv/bin/python"),
                          str(ROOT / "tools/build_function_map.py")]),
        ("function map check", [str(ROOT / ".venv/bin/python"),
                                str(ROOT / "tools/build_function_map.py"), "--check"]),
        ("all 1771 units", [str(ROOT / ".venv/bin/python"),
                            str(ROOT / "tools/verify_matching_units.py"),
                            "--output", "build/us/matching_report.json"]),
        ("non-code ranges", [str(ROOT / ".venv/bin/python"),
                             str(ROOT / "tools/verify_noncode.py")]),
        ("report", [str(ROOT / ".venv/bin/python"),
                    str(ROOT / "tools/generate_decomp_report.py")]),
        ("unit tests", [str(ROOT / ".venv/bin/python"), "-m", "unittest",
                        "discover", "-s", "tests", "-t", "."]),
    ]
    failed = []
    for label, command in steps:
        result = subprocess.run(command, cwd=ROOT, env=environment(),
                                capture_output=True, text=True)
        tail = (result.stdout or "").strip().splitlines()[-1:] or [""]
        status = "ok" if result.returncode == 0 else "FAILED"
        print(f"{label:20s} {status:7s} {tail[0][:110]}")
        if result.returncode:
            failed.append(label)
            print((result.stderr or "")[-2000:])
    return failed


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("command", choices=["plan", "bind", "verify", "gate"])
    parser.add_argument("units")
    parser.add_argument("mapping", nargs="?")
    parser.add_argument("--jobs", type=int)
    args = parser.parse_args()

    if args.command == "gate":
        return 1 if gate(args.units) else 0

    paths = load_units(args.units)

    if args.command == "bind":
        mapping = json.loads(Path(args.mapping).read_text())
        functions = mapping.get("functions", {})
        globals_ = mapping.get("globals", {})
        changed = [rel for rel in paths if bind_file(ROOT / rel, functions, globals_)]
        print(f"bind: {len(changed)} of {len(paths)} files changed")
        return 0

    if args.command == "plan":
        manifest = {unit["source"]: unit
                    for unit in json.loads(MANIFEST.read_text())["units"]}
        facts = []
        for rel in paths:
            text = (ROOT / rel).read_text(errors="ignore")
            masked = mask(text)
            unit = manifest.get(rel, {})
            facts.append({
                "source": rel, "name": unit.get("name"), "size": unit.get("size"),
                "address": unit.get("address"),
                "definitions": [m.group(2) for m in DEF.finditer(masked)
                                if m.group(2) not in KEYWORDS],
                "callees": sorted(set(re.findall(r"\bfunc_[0-9A-F]{8}\b", masked))
                                  - {unit.get("name")}),
                "globals": sorted(set(re.findall(r"\bD_[0-9A-F]{8}\b", masked))),
                "lines": len(text.splitlines()),
            })
        print(json.dumps(facts, indent=1))
        return 0

    result = verify(paths, args.jobs)
    output = (result.stdout or "") + (result.stderr or "")
    failing = [line for line in output.splitlines()
               if line.startswith("FAIL") or "failed" in line
               or "error" in line.lower()]
    passes = sum(1 for line in output.splitlines() if line.startswith("PASS"))
    print(f"verify: {passes}/{len(paths)} pass")
    for line in failing[:40]:
        print(line)
    return 0 if passes == len(paths) and result.returncode == 0 else 1


if __name__ == "__main__":
    sys.exit(main())
