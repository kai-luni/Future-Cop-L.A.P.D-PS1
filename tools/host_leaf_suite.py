#!/usr/bin/env python3
"""Reusable generator/runner for leaf-function host suites.

A leaf suite is one harness, tests/host/<suite>_harness.c, that links many
unchanged single-object units from src/game/ and checks each with literal or
independently generated expectations. Each tested function is declared by a
marker comment in the harness:

    /* @leaf 80092488 spec PSn00bSDK setPolyG3 */
              ^addr    ^evidence class  ^free-text justification

and its checks are grouped by LK_FUNC("func_80092488") (tests/host/leaf_kit.h),
so the run prints per-function check counts.

Subcommands

  scaffold ADDR...     print a harness block for each address: the prototype
                       copied from the unit's own __asm__ declaration, the data
                       globals the linker needs, and whether the unit already
                       runs in another test. It never writes expected values.
  gen-script SUITE     (re)write tests/host/<suite>_test.sh from the markers,
                       with a literal `for unit in ...` line so
                       tools/host_coverage.py and host_test_reach.py see it.
  report SUITE         build + run the suite, join the LEAF lines with the
                       markers, print/write the test-to-function map.
  mutate SUITE         mutation sample: for chosen units, apply one source
                       mutation at a time to a *temporary copy* of the unit,
                       relink, rerun, and record whether that function's
                       checks fail (killed) or not (survived). src/game is
                       never written.

Evidence classes: spec, math, libc, semantic, source (see the harness header).
"""

import argparse
import json
import os
import re
import signal
import shutil
import subprocess
import sys
import tempfile
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
TOOLS_DIR = Path(__file__).resolve().parent
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))
import unit_paths  # noqa: E402
HOST = REPO / "tests" / "host"
T = REPO / ".toolchain"
GCC = T / "gcc-2.7.2-cdk" / "gcc"
QEMU = os.environ.get(
    "QEMU", str(Path.home() / ".local/share/qemu-user/extracted/usr/bin/qemu-mipsel"))
MARK_RE = re.compile(r"/\*\s*@leaf\s+([0-9A-Fa-f]{8})\s+(\w+)\s+(.*?)\s*\*/")
LEAF_RE = re.compile(r"^LEAF (func_[0-9A-F]{8}) checks=(\d+) failures=(\d+)$", re.M)
EVIDENCE = ("spec", "math", "libc", "semantic", "source")
# Original dependencies may be linked without claiming new direct coverage.
LINK_RE = re.compile(r"/\*\s*@link\s+([0-9A-Fa-f]{8})\s*\*/")
# Opt-in shared mocks beyond bios_mock: `/* @mock sound_mock */` in a harness
# links tests/host/mocks/sound_mock.c instead of the harness carrying a private
# copy of a stand-in the library already provides.
MOCK_RE = re.compile(r"/\*\s*@mock\s+(\w+_mock)\s*\*/")


def env():
    e = dict(os.environ)
    e["PATH"] = f"{T}/mipsel-binutils/root/usr/bin:" + e.get("PATH", "")
    lib = f"{T}/mipsel-binutils/root/usr/lib/x86_64-linux-gnu"
    e["LD_LIBRARY_PATH"] = lib + (":" + e["LD_LIBRARY_PATH"] if e.get("LD_LIBRARY_PATH") else "")
    return e


def markers(suite):
    text = (HOST / f"{suite}_harness.c").read_text()
    out, seen = [], set()
    for m in MARK_RE.finditer(text):
        addr = m.group(1).upper()
        if addr in seen:
            continue
        if m.group(2) not in EVIDENCE:
            sys.exit(f"{suite}: unknown evidence class {m.group(2)!r} for {addr}")
        seen.add(addr)
        out.append({"unit": addr, "evidence": m.group(2), "why": m.group(3)})
    return out


# ------------------------------------------------------------------ scaffold

def prototype(addr):
    src_path = unit_paths.resolve_unit_path(addr, repo=REPO)
    if src_path is None:
        return None, f"missing source for {addr}"
    src = src_path.read_text(errors="ignore")
    m = re.search(r"^([^\n;]*?\b(\w+)\s*\([^;]*?\))\s*__asm__\(\s*\"func_%s\"\s*\)\s*;" % addr,
                  src, re.M | re.S)
    if m:
        return " ".join(m.group(1).split()) + f' __asm__("func_{addr}");', m.group(2)
    if re.search(r"\.globl\s+func_%s" % addr, src):
        return None, "handwritten asm (no C prototype; derive from disassembly)"
    return None, "no __asm__ alias found"


def scaffold(args):
    build = json.load(open(args.buildability)) if args.buildability else {}
    reach = json.load(open(args.reach)) if args.reach else {}
    for addr in (a.upper() for a in args.addr):
        proto, name = prototype(addr)
        b = build.get(addr, {})
        print(f"/* ---- {addr}  state={b.get('state', '?')} chain={b.get('chain', '?')}"
              f" class={b.get('class', '?')}"
              f"{'  ALREADY REACHED' if f'func_{addr}' in reach else ''} */")
        if b.get("data_globals"):
            print(f"/* data globals the unit references (define + set them per test;"
                  f" never trust a zeroed slot): {' '.join(b['data_globals'])} */")
        print(proto or f"/* {name} */")
        print(f"    /* @leaf {addr} EVIDENCE TODO: cite the independent source */")
        print(f'    LK_FUNC("func_{addr}");')
        print("    /* TODO: expected values from the cited source - never from running the unit */")
        print()


# ----------------------------------------------------------------- gen-script

SCRIPT = """#!/bin/bash
# GENERATED by tools/host_leaf_suite.py gen-script {suite} - edit the @leaf
# markers in {suite}_harness.c, not this file.
# Compiles the unchanged decomp units below, links them with the leaf harness
# and runs the per-function checks under QEMU.
set -euo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
DECOMP="${{DECOMP:-$(cd "$HERE/../.." && pwd)}}"
T="${{TOOLCHAIN:-$DECOMP/.toolchain}}"
OUT="${{OUT:-$HERE/build-{dashed}}}"
QEMU="${{QEMU:-$HOME/.local/share/qemu-user/extracted/usr/bin/qemu-mipsel}}"
GCC="$T/gcc-2.7.2-cdk/gcc"
MASPSX="$(echo "$T"/maspsx-*/maspsx.py)"
mkdir -p "$OUT"
export PATH="$T/mipsel-binutils/root/usr/bin:$PATH"
export LD_LIBRARY_PATH="$T/mipsel-binutils/root/usr/lib/x86_64-linux-gnu${{LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}}"

compile() {{
    local src="$1" obj="$2"; shift 2
    "$GCC" -B"$T/gcc-2.7.2-cdk/" -I"$DECOMP/include" -I"$HERE/mocks" "$@" -O1 -G0 -S "$src" -o "$OUT/tmp.s"
    python3 "$MASPSX" "$OUT/tmp.s" | python3 "$HERE/fix_align.py" > "$OUT/tmp.mas.s"
    mipsel-linux-gnu-as -o "$obj" "$OUT/tmp.mas.s"
}}

source "$HERE/unit_path.sh"

objects=()
for unit in {units}; do
    compile "$(unit_src "$unit")" "$OUT/$unit.o"
    objects+=("$OUT/$unit.o")
done
compile "$HERE/mocks/bios_mock.c" "$OUT/bios_mock.o"
{mock_compiles}python3 "$HERE/leaf_vectors.py" > "$OUT/leaf_vectors.h"
compile "$HERE/{suite}_harness.c" "$OUT/{suite}_harness.o" -I"$OUT" -I"$HERE"
mipsel-linux-gnu-as -o "$OUT/crt0.o" "$HERE/crt0.s" 2>/dev/null
mipsel-linux-gnu-ld -static -e __start -o "$OUT/{suite}" \\
    "$OUT/crt0.o" "$OUT/{suite}_harness.o" "$OUT/bios_mock.o"{mock_objects} "${{objects[@]}}"
"$QEMU" "$OUT/{suite}"
"""


def extra_mocks(suite):
    """Shared mocks a harness opts into with `/* @mock <name>_mock */`, in order."""
    text = (HOST / f"{suite}_harness.c").read_text()
    out = []
    for name in MOCK_RE.findall(text):
        if name == "bios_mock" or name in out:
            continue
        if not (HOST / "mocks" / f"{name}.c").exists():
            sys.exit(f"{suite}: @mock {name}: tests/host/mocks/{name}.c does not exist")
        out.append(name)
    return out


def gen_script(args):
    units = [m["unit"] for m in markers(args.suite)]
    harness = (HOST / f"{args.suite}_harness.c").read_text()
    units.extend(unit.upper() for unit in LINK_RE.findall(harness))
    units = list(dict.fromkeys(units))
    mocks = extra_mocks(args.suite)
    path = HOST / f"{args.suite}_test.sh"
    path.write_text(SCRIPT.format(
        suite=args.suite, dashed=args.suite.replace("_", "-"), units=" ".join(units),
        mock_compiles="".join(f'compile "$HERE/mocks/{m}.c" "$OUT/{m}.o"\n' for m in mocks),
        mock_objects="".join(f' "$OUT/{m}.o"' for m in mocks)))
    path.chmod(0o755)
    print(f"{path.relative_to(REPO)}: {len(units)} units")


# --------------------------------------------------------------------- report

def run_suite(suite, decomp=None, out=None, host=HOST, timeout=None):
    """Run the generated script. A mutant can loop forever (e.g. a broken loop
    bound), so mutation runs pass a hard timeout; rc 124 then means 'hung'."""
    e = env()
    if decomp:
        e["DECOMP"] = str(decomp)
        e["TOOLCHAIN"] = str(T)
    if out:
        e["OUT"] = str(out)
    proc = subprocess.Popen(["bash", str(Path(host) / f"{suite}_test.sh")],
                            stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                            text=True, env=e, start_new_session=True)
    try:
        stdout, stderr = proc.communicate(timeout=timeout)
    except subprocess.TimeoutExpired:
        try:
            os.killpg(proc.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
        stdout, _ = proc.communicate()
        return 124, stdout, "timeout"
    return proc.returncode, stdout, stderr


def parse(stdout):
    per = {}
    for name, n, f in LEAF_RE.findall(stdout):
        c = per.setdefault(name, {"checks": 0, "failures": 0})
        c["checks"] += int(n)
        c["failures"] += int(f)
    return per


def report(args):
    marks = markers(args.suite)
    rc, out, err = run_suite(args.suite)
    per = parse(out)
    rows, problems = [], []
    for m in marks:
        name = f"func_{m['unit']}"
        c = per.get(name)
        if c is None:
            problems.append(f"{name}: marker without LK_FUNC block")
            c = {"checks": 0, "failures": 0}
        rows.append({"function": name, **m, **c,
                     "passed": c["checks"] > 0 and c["failures"] == 0})
    for name in per:
        if name[5:] not in {m["unit"] for m in marks}:
            problems.append(f"{name}: LK_FUNC block without @leaf marker")
    summary = {
        "suite": args.suite,
        "exit_code": rc,
        "functions": len(rows),
        "passed": sum(r["passed"] for r in rows),
        "by_evidence": {k: sum(1 for r in rows if r["evidence"] == k and r["passed"])
                        for k in EVIDENCE},
        "checks": sum(r["checks"] for r in rows),
        "problems": problems,
        "tests": rows,
    }
    if args.json:
        Path(args.json).write_text(json.dumps(summary, indent=1) + "\n")
    for r in rows:
        print(f"{r['function']}  {r['evidence']:8s} checks={r['checks']:3d} "
              f"failures={r['failures']}  {r['why']}")
    print(f"{summary['passed']}/{summary['functions']} functions passed, "
          f"{summary['checks']} checks, exit {rc}; by evidence {summary['by_evidence']}")
    for p in problems:
        print("PROBLEM:", p)
    if rc != 0 and not per:
        sys.stderr.write(err[-3000:])
    return 0 if rc == 0 and not problems and summary["passed"] == len(rows) else 1


# --------------------------------------------------------------------- mutate

# One mutation operator = (label, regex, replacement). Each is applied to one
# occurrence at a time, only in the code part of the unit (comments blanked
# before matching, so offsets stay valid).
OPERATORS = [
    ("lt->le", r"(?<![<>=!])<(?![<=])", "<="),
    ("le->lt", r"(?<!<)<=", "<"),
    ("gt->ge", r"(?<![<>=\-])>(?![>=])", ">="),
    ("ge->gt", r"(?<!>)>=", ">"),
    ("eq->ne", r"==", "!="),
    ("ne->eq", r"!=", "=="),
    ("plus->minus", r"(?<![+\[])\+(?![+=])", "-"),
    ("shl->shr", r"<<(?!=)", ">>"),
    ("shr->shl", r">>(?!=)", "<<"),
    ("and->or", r"(?<!&)&(?![&=])", "|"),
    ("int+1", r"(?<![\w.])(0x[0-9A-Fa-f]+|\d+)(?![\w.])", None),
    ("return0", r"\breturn\s+[^;]+;", "return 0;"),
]


def code_mask(text):
    """Text with comments and preprocessor lines blanked (same length)."""
    def blank(m):
        return re.sub(r"[^\n]", " ", m.group(0))
    t = re.sub(r"/\*.*?\*/", blank, text, flags=re.S)
    t = re.sub(r"//[^\n]*", blank, t)
    t = re.sub(r"^\s*#[^\n]*", blank, t, flags=re.M)
    t = re.sub(r"__asm__\([^)]*\)", blank, t)        # keep the label alias intact
    t = re.sub(r"\"[^\"\n]*\"", blank, t)
    return t


def body_start(text, unit):
    """Offset of the function body: first '{' after the last `__asm__("func_X")`.

    Struct definitions above the prototype are layout, not behaviour; mutating
    them tests the fixture more than the function, so they are skipped."""
    alias = f'__asm__("func_{unit}")'
    pos = text.rfind(alias)
    if pos < 0:
        return text.find("{")
    return text.find("{", pos)


def mutants(text, limit, unit=""):
    mask = code_mask(text)
    body = body_start(text, unit)
    out = []
    for label, rx, rep in OPERATORS:
        for m in re.finditer(rx, mask):
            if m.start() < body:
                continue
            if rep is None:
                lit = m.group(1)
                val = int(lit, 0)
                new = (hex(val + 1) if lit.lower().startswith("0x") else str(val + 1))
            else:
                new = rep
            mutated = text[:m.start()] + new + text[m.end():]
            line = text.count("\n", 0, m.start()) + 1
            out.append((label, line, mutated))
            break  # first occurrence per operator keeps the sample small
    return out[:limit]


def mutate(args):
    marks = {m["unit"]: m for m in markers(args.suite)}
    units = [u.upper() for u in args.unit] if args.unit else list(marks)
    results = []
    with tempfile.TemporaryDirectory(dir=os.environ.get("TMPDIR")) as tmp:
        tmp = Path(tmp)
        (tmp / "src").mkdir()
        shutil.copytree(REPO / "src" / "game", tmp / "src" / "game")
        os.symlink(REPO / "include", tmp / "include")
        os.symlink(REPO / "config", tmp / "config")
        os.symlink(REPO / "tools", tmp / "tools")
        # Snapshot the harness side too, so editing tests/host while a long
        # mutation run is in flight cannot change what the mutants are run
        # against. Build directories are not copied.
        host = tmp / "tests" / "host"
        host.mkdir(parents=True)
        for f in HOST.iterdir():
            if f.is_file() and f.suffix in (".c", ".h", ".py", ".s", ".sh"):
                shutil.copy2(f, host / f.name)
        # Generated scripts compile and include the shared mock library.
        if (HOST / "mocks").is_dir():
            shutil.copytree(HOST / "mocks", host / "mocks")
        out = tmp / "build"
        rc, stdout, _ = run_suite(args.suite, tmp, out, host)
        if rc != 0:
            sys.exit("baseline run of the copy failed; fix the suite first")
        import time
        t0 = time.time()
        for unit in units:
            if time.time() - t0 > args.budget:
                print(f"budget of {args.budget}s reached, stopping before {unit}")
                break
            path = unit_paths.resolve_unit_path(unit, repo=tmp)
            if path is None:
                sys.exit(f"missing source for {unit}")
            original = path.read_text()
            for label, line, text in mutants(original, args.per_unit, unit):
                path.write_text(text)
                rc, stdout, stderr = run_suite(args.suite, tmp, out, host, args.timeout)
                per = parse(stdout)
                me = per.get(f"func_{unit}", {"checks": 0, "failures": 0})
                if rc == 124 and stderr == "timeout":
                    verdict = "killed-timeout"    # hung: detected, but only by the time limit
                elif rc != 0 and not per:
                    # nothing ran: either the mutant does not compile/link or
                    # the binary died before the first function reported
                    verdict = "invalid" if "Error" in stderr or "error" in stderr else "killed-crash"
                elif me["failures"] > 0:
                    verdict = "killed"
                elif f"func_{unit}" not in per:
                    verdict = "killed-crash"      # died inside this function's block
                elif rc != 0:
                    verdict = "killed-elsewhere"  # its own checks passed, another failed
                else:
                    verdict = "survived"
                results.append({"unit": unit, "operator": label, "line": line,
                                "verdict": verdict, "failures": me["failures"]})
                print(f"{unit} {label:12s} line {line:3d}: {verdict}")
            path.write_text(original)
    summary = {
        "suite": args.suite,
        "mutants": len(results),
        "killed": sum(r["verdict"] == "killed" for r in results),
        "killed_timeout": sum(r["verdict"] == "killed-timeout" for r in results),
        "killed_crash": sum(r["verdict"] == "killed-crash" for r in results),
        "killed_elsewhere": sum(r["verdict"] == "killed-elsewhere" for r in results),
        "survived": sum(r["verdict"] == "survived" for r in results),
        "invalid": sum(r["verdict"] == "invalid" for r in results),
        "results": results,
    }
    if args.json:
        Path(args.json).write_text(json.dumps(summary, indent=1) + "\n")
    print({k: v for k, v in summary.items() if k != "results"})
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("scaffold")
    s.add_argument("addr", nargs="+")
    s.add_argument("--buildability", help="JSON from host_test_buildability.py")
    s.add_argument("--reach", help="JSON from host_test_reach.py --json")
    s.set_defaults(fn=scaffold)
    s = sub.add_parser("gen-script")
    s.add_argument("suite")
    s.set_defaults(fn=gen_script)
    s = sub.add_parser("report")
    s.add_argument("suite")
    s.add_argument("--json")
    s.set_defaults(fn=report)
    s = sub.add_parser("mutate")
    s.add_argument("suite")
    s.add_argument("--unit", action="append")
    s.add_argument("--per-unit", type=int, default=3)
    s.add_argument("--timeout", type=int, default=30, help="seconds per mutant run")
    s.add_argument("--budget", type=int, default=900, help="stop after this many seconds")
    s.add_argument("--json")
    s.set_defaults(fn=mutate)
    args = ap.parse_args()
    return args.fn(args) or 0


if __name__ == "__main__":
    sys.exit(main())
