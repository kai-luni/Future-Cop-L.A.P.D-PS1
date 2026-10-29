#!/usr/bin/env python3
"""Run resumable, compiler-guided C reconstruction jobs on local hardware."""

from __future__ import annotations

import argparse
import concurrent.futures
import hashlib
import json
import os
import re
import shlex
import subprocess
import sys
import tempfile
import threading
from dataclasses import dataclass
from pathlib import Path
from typing import Any

import verify_matching_units as matching


ROOT = Path(__file__).resolve().parents[1]
DEFAULT_ASSEMBLY = ROOT / "asm/us/main.s"
DEFAULT_EXECUTABLE = ROOT / "baseroms/us/SLUS_007.39"
DEFAULT_OUTPUT = ROOT / "build/us/offline-decomp"
MATCHING_MANIFEST = ROOT / "config/us/matching_units.json"
RULES = ROOT / "config/us/reconstruction_rules.md"
CANDIDATE_MANIFEST = ROOT / "config/us/candidate_units.json"
FUNCTION_HEADER = re.compile(
    r"^nonmatching\s+(?P<name>func_[0-9A-Fa-f]{8}|entrypoint),\s*"
    r"(?P<size>0x[0-9A-Fa-f]+)\s*$"
)
GLOBAL_SYMBOL = re.compile(r"\b(?:func|D)_[0-9A-Fa-f]{8}\b")
# A BIOS table dispatch loads the entry address into $t2 and jumps through it. The
# pinned GCCs append their own prologue/epilogue to any C function, so these ranges
# cannot be reproduced as C no matter how the source is written (hand-checked).
BIOS_DISPATCH = re.compile(r"jr\s+\$t2\b")
# Hardware-only constructs: GTE transfers, syscalls and breakpoints have no C form,
# so a campaign that wants reconstructed C should not spend attempts on them.
HARDWARE_ONLY = re.compile(
    r"\b(?:cop2|lwc2|swc2|ctc2|cfc2|mtc2|mfc2|syscall|break)\b")
GP_REFERENCE = re.compile(r"\b(?:lb|lbu|lh|lhu|lw|sb|sh|sw)\s+\$\w+,\s*(0x[0-9A-Fa-f]+|\(?0\)?)\(\$gp\)")
#: $gp base of this target, taken from the verified units' symbols (``_gp``).
GP_BASE = 0x8009F214
# Symbol names become `.set` definitions and C identifiers, so any plain identifier
# is acceptable; the strict address-shaped pattern rejected legitimate proposals.
SAFE_SYMBOL = re.compile(r"^[A-Za-z_][A-Za-z0-9_]{0,63}$")
NON_C_SOURCE = re.compile(
    r"(?:^\s*\.word\b|^\s*\.include\b|^\s*\.set\b)",
    re.MULTILINE,
)
# A proposal may use one inline __asm__ block (BIOS dispatch stubs need it), but it
# must stay a C unit: an assembly listing smuggled in as lines of mnemonics is not
# a reconstruction.
MIPS_MNEMONIC = re.compile(
    r"^\s*(?:lui|lw|sw|lb|sb|addiu|addi|addu|subu|sltu|slt|jr|jal|j|nop|sll|srl|sra|"
    r"multu|divu|mfhi|mflo|break|syscall|cop2)\s",
    re.MULTILINE,
)
ASM_BLOCK = re.compile(r"__asm__\s*(?:volatile\s*)?\((?:[^()]|\([^()]*\))*\)", re.DOTALL)


def _listing_without_asm_blocks(source: str) -> str:
    """Source with every inline-assembly block blanked out."""
    return ASM_BLOCK.sub("", source)


# Inline assembly is tolerated for BIOS dispatch stubs, but only as bare
# instructions inside a C function. Directives, symbol tables and label
# definitions would turn the proposal into an assembler file that emits the
# retail bytes verbatim, which is not a C reconstruction.
ASM_DIRECTIVE = re.compile(
    r"\.(?:word|half|byte|ascii|asciiz|text|data|section|globl|global|type|"
    r"ent|end|size|set|align|include)\b"
)
ASM_LABEL = re.compile(r"(?m)^\s*[A-Za-z_][A-Za-z0-9_]*\s*:\s*$")
C_FUNCTION = re.compile(r"[A-Za-z_][A-Za-z0-9_\s\*]*\b[A-Za-z_]\w*\s*\([^;]*\)\s*\{")


def reject_assembly_listing(source: str) -> None:
    if MIPS_MNEMONIC.search(_listing_without_asm_blocks(source)):
        raise ValueError("source contains assembly; reconstructed C is required")
    for block in ASM_BLOCK.finditer(source):
        text = block.group(0)
        if ASM_DIRECTIVE.search(text):
            raise ValueError("inline assembly must not contain assembler directives")
        if ASM_LABEL.search(text):
            raise ValueError("inline assembly must not define labels")
    if "__asm__" in source and not C_FUNCTION.search(source):
        raise ValueError("source must be a C function, not an assembly listing")


def _decode_extras(value: dict[str, Any]) -> dict[str, Any]:
    """Optional per-proposal compiler/flags, so a unit can pick its pinned toolchain."""
    extras: dict[str, Any] = {}
    compiler = value.get("compiler")
    if compiler is not None:
        if compiler not in ALLOWED_COMPILERS:
            raise ValueError(f"unsupported compiler: {compiler!r}")
        extras["compiler"] = compiler
    flags = value.get("compiler_flags_extra", [])
    if flags:
        if not isinstance(flags, list):
            raise ValueError("compiler_flags_extra must be a list")
        for flag in flags:
            if not isinstance(flag, str) or not SAFE_FLAG.fullmatch(flag):
                raise ValueError(f"unsafe compiler flag: {flag!r}")
        extras["compiler_flags_extra"] = flags
    return extras


@dataclass(frozen=True)
class Target:
    name: str
    address: int
    size: int
    assembly: str


def _recent_sources(output: Path, target: "Target", state: dict[str, Any],
                    limit: int = 3, chars: int = 1200) -> list[str]:
    """The last few candidate sources of one target, for the 'do not repeat' hint."""
    sources: list[str] = []
    attempts = sorted((output / "work" / target.name / "attempts").glob("*/candidate.c"))
    for path in reversed(attempts[-limit:]):
        try:
            sources.append(path.read_text()[:chars])
        except OSError:
            continue
    return sources


MISSING = object()


def mnemonic_profile(assembly: str) -> set[str]:
    """Instruction mnemonics of one oracle block, ignoring registers and constants."""
    profile = set()
    for line in assembly.splitlines():
        match = re.search(r"\*/\s+([a-z][a-z0-9.]*)\s", line)
        if match:
            profile.add(match.group(1))
    return profile


def similar_verified_units(target: "Target", targets: dict[str, "Target"],
                           verified: dict[str, str], limit: int = 2,
                           chars: int = 1400) -> list[dict[str, Any]]:
    """Already-verified units whose instruction mix resembles this one.

    The verifier rewards this compiler's quirks, so a nearby verified source is a
    better template than any general rule. Similarity is the Jaccard index of the
    mnemonic sets, tie-broken by the distance in size.
    """
    wanted = mnemonic_profile(target.assembly)
    if not wanted:
        return []
    scored = []
    for name, source in verified.items():
        other = targets.get(name)
        if other is None:
            continue
        profile = mnemonic_profile(other.assembly)
        if not profile:
            continue
        jaccard = len(wanted & profile) / len(wanted | profile)
        if jaccard < 0.6:
            continue
        scored.append((jaccard, -abs(other.size - target.size), name, source))
    scored.sort(reverse=True)
    examples = []
    for jaccard, _size_delta, name, source in scored[:limit]:
        examples.append({"name": name, "similarity": round(jaccard, 2),
                         "source": source[:chars]})
    return examples


def gp_objects(assembly: str) -> dict[str, int]:
    """Object addresses referenced through ``$gp`` in one oracle block.

    The oracle offset plus the ``$gp`` base names the object, so a proposal can
    declare it as a plain ``extern`` instead of inventing an absolute address.
    Returns ``{symbol_name: address}``.
    """
    objects: dict[str, int] = {}
    for match in GP_REFERENCE.finditer(assembly):
        offset = int(match.group(1), 0) if match.group(1).startswith("0x") else 0
        address = GP_BASE + offset
        objects[f"D_{address:08X}"] = address
    return objects


def atomic_json(path: Path, value: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + ".tmp")
    temporary.write_text(json.dumps(value, indent=2, sort_keys=True) + "\n")
    temporary.replace(path)


def load_json(path: Path, default: Any) -> Any:
    return json.loads(path.read_text()) if path.is_file() else default


def parse_targets(text: str) -> list[Target]:
    """Read Splat function blocks without treating their labels as proof."""
    lines = text.splitlines()
    targets: list[Target] = []
    index = 0
    while index < len(lines):
        match = FUNCTION_HEADER.match(lines[index].strip())
        if not match:
            index += 1
            continue
        name = match.group("name")
        size = int(match.group("size"), 0)
        address = 0x80096520 if name == "entrypoint" else int(name[5:], 16)
        start = index
        index += 1
        terminator = f"endlabel {name}"
        while index < len(lines) and lines[index].strip() != terminator:
            index += 1
        if index == len(lines):
            raise ValueError(f"{name}: missing {terminator}")
        index += 1
        targets.append(Target(name, address, size, "\n".join(lines[start:index]).strip()))
    return targets


def inferred_symbols(target: Target) -> dict[str, str]:
    symbols = {}
    for name in sorted(set(GLOBAL_SYMBOL.findall(target.assembly)) - {target.name}):
        symbols[name] = f"0x{int(name.rsplit('_', 1)[1], 16):08X}"
    return symbols


def score_bytes(actual: bytes, expected: bytes) -> dict[str, Any]:
    padded = actual[:len(expected)] + bytes(max(0, len(expected) - len(actual)))
    equal_bytes = sum(left == right for left, right in zip(padded, expected))
    words = len(expected) // 4
    equal_words = sum(
        padded[offset:offset + 4] == expected[offset:offset + 4]
        for offset in range(0, words * 4, 4)
    )
    prefix = 0
    for left, right in zip(padded, expected):
        if left != right:
            break
        prefix += 1
    trailing = len(actual) > len(expected) and any(actual[len(expected):])
    mismatches = []
    for offset in range(0, len(expected), 4):
        expected_word = expected[offset:offset + 4]
        actual_word = padded[offset:offset + 4]
        if actual_word != expected_word:
            mismatches.append({
                "offset": f"0x{offset:X}",
                "expected_le": expected_word.hex().upper(),
                "actual_le": actual_word.hex().upper(),
            })
    return {
        "exact": padded == expected and not trailing,
        "equal_bytes": equal_bytes,
        "total_bytes": len(expected),
        "equal_words": equal_words,
        "total_words": words,
        "matching_prefix_bytes": prefix,
        "nonzero_bytes_after_extent": trailing,
        "compiled_size": len(actual),
        "mismatches": mismatches[:64],
        "mismatch_list_truncated": len(mismatches) > 64,
        "output_sha256": hashlib.sha256(actual).hexdigest(),
    }


ALLOWED_COMPILERS = {"gcc-2.6.3-psx", "gcc-2.7.2-cdk", "gcc-2.7.2-psx",
                      "gcc-2.8.0-psx", "gcc-2.8.1-psx"}
SAFE_FLAG = re.compile(r"^-[A-Za-z0-9_,=+-]+$")


def decode_backend_output(text: str) -> tuple[str, dict[str, str], str, dict[str, Any]]:
    """Validate the deliberately narrow proposal-command response."""
    value = json.loads(text)
    if not isinstance(value, dict) or not isinstance(value.get("source"), str):
        raise ValueError("response must be an object containing a source string")
    source = value["source"].strip().replace(
        '#include "../../include/', '#include "'
    ) + "\n"
    if not source.strip():
        raise ValueError("source must not be empty")
    if NON_C_SOURCE.search(source):
        raise ValueError("source contains assembly; reconstructed C is required")
    reject_assembly_listing(source)
    raw_symbols = value.get("symbols", {})
    if not isinstance(raw_symbols, dict):
        raise ValueError("symbols must be an object")
    symbols = {}
    for name, address in raw_symbols.items():
        if not isinstance(name, str) or not SAFE_SYMBOL.fullmatch(name):
            raise ValueError(f"unsafe symbol name: {name!r}")
        if isinstance(address, bool) or not isinstance(address, (str, int)):
            raise ValueError(f"symbol address must be a string or integer: {name}")
        parsed = int(address, 0) if isinstance(address, str) else int(address)
        if not 0 <= parsed <= 0xFFFFFFFF:
            raise ValueError(f"symbol address outside 32-bit range: {name}")
        symbols[name] = f"0x{parsed:08X}"
    note = value.get("note", "")
    if not isinstance(note, str):
        raise ValueError("note must be a string")
    return source, symbols, note, _decode_extras(value)


def compiler_environment() -> dict[str, str]:
    environment = os.environ.copy()
    bin_dir = ROOT / ".toolchain/mipsel-binutils/root/usr/bin"
    lib_dir = ROOT / ".toolchain/mipsel-binutils/root/usr/lib/x86_64-linux-gnu"
    environment["PATH"] = f"{bin_dir}{os.pathsep}{environment.get('PATH', '')}"
    current = environment.get("LD_LIBRARY_PATH", "")
    environment["LD_LIBRARY_PATH"] = str(lib_dir) + (os.pathsep + current if current else "")
    return environment


class Runner:
    def __init__(self, args: argparse.Namespace):
        self.args = args
        self.output = args.output.resolve()
        try:
            self.output.relative_to((ROOT / "build").resolve())
        except ValueError as error:
            raise ValueError("worker output must remain beneath the ignored build directory") from error
        self.executable = args.executable.read_bytes()
        self.config = load_json(MATCHING_MANIFEST, {})
        self.candidate_config = load_json(CANDIDATE_MANIFEST, {"units": []})
        self.command = shlex.split(args.backend_command)
        if not self.command:
            raise ValueError("proposal command is empty")
        self.print_lock = threading.Lock()
        self.targets = {target.name: target for target in parse_targets(args.assembly.read_text())}
        self.verified_sources = {
            unit["name"]: (ROOT / unit["source"]).read_text()
            for unit in self.config.get("units", [])
            if (ROOT / unit["source"]).is_file()
        }

    def expected(self, target: Target) -> bytes:
        offset = matching.HEADER_SIZE + target.address - matching.LOAD_ADDRESS
        result = self.executable[offset:offset + target.size]
        if len(result) != target.size:
            raise ValueError(f"{target.name}: range is outside the executable")
        return result

    def initial_source(self, target: Target) -> str | None:
        for unit in self.candidate_config.get("units", []):
            if unit.get("name") == target.name:
                path = ROOT / unit["source"]
                return path.read_text() if path.is_file() else None
        return None

    def request(self, target: Target, state: dict[str, Any]) -> dict[str, Any]:
        best = state.get("best")
        return {
            "protocol": 1,
            "task": "reconstruct_byte_matching_c",
            "target": {
                "name": target.name,
                "address": f"0x{target.address:08X}",
                "size": target.size,
                "architecture": "MIPS R3000A, little-endian, 32-bit",
                "compiler": self.config["compiler"],
                "compiler_flags": self.config["compiler_flags"],
                "assembly_oracle": target.assembly,
                "known_symbols": inferred_symbols(target) | {
                    name: f"0x{address:08X}" for name, address in gp_objects(target.assembly).items()
                },
                "similar_verified_units": similar_verified_units(
                    target, self.targets, self.verified_sources),
                "gp_base": f"0x{GP_BASE:08X}",
                "gp_objects": {name: f"0x{address:08X}"
                               for name, address in gp_objects(target.assembly).items()},
            },
            "attempt": state.get("attempts", 0) + 1,
            # Proposals repeat themselves when they cannot see what was already
            # tried: the newest few sources are shown as rejected variants, and the
            # mismatch table of the best attempt says which words still differ.
            "already_tried": [
                {"source": text, "note": "rejected variant, do not repeat"}
                for text in _recent_sources(self.output, target, state)
            ],
            "best_candidate": None if best is None else {
                "source": (self.output / best["source"]).read_text(),
                "symbols": best["symbols"],
                "score": best["score"],
                "compiler_diagnostic": best.get("compiler_diagnostic", ""),
            },
            "guidance": RULES.read_text() if RULES.is_file() else "",
            "requirements": [
                "Return reconstructed C source, not assembly.",
                "Use ANSI C accepted by the pinned compiler.",
                "Use project includes such as #include \"game/types.h\" when needed.",
                "Preserve 32-bit PS1 address and integer semantics.",
                "Return only one JSON object: source (string), symbols (object), note (string), "
                "and optionally compiler (one pinned name) and compiler_flags_extra (list).",
                "Follow the guidance rules before guessing.",
            ],
        }

    def call_backend(self, payload: dict[str, Any]
                     ) -> tuple[str, dict[str, str], str, dict[str, Any]]:
        completed = subprocess.run(
            self.command,
            input=json.dumps(payload),
            text=True,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            timeout=self.args.timeout,
            cwd=ROOT,
            env=compiler_environment(),
            check=False,
        )
        if completed.returncode:
            detail = completed.stderr.strip()[-2000:]
            raise RuntimeError(f"proposal command exited {completed.returncode}: {detail}")
        return decode_backend_output(completed.stdout)

    def compile(self, target: Target, source_path: Path,
                symbols: dict[str, str], extras: dict[str, Any] | None = None
                ) -> tuple[bytes | None, str]:
        relative = source_path.relative_to(ROOT)
        unit = {
            "name": target.name,
            "address": target.address,
            "size": target.size,
            "source": str(relative),
            "symbols": inferred_symbols(target) | symbols,
        }
        for key, value in (extras or {}).items():
            unit[key] = value
        # A gp-relative oracle means the object must sit in small data. Unless the
        # proposal declares it itself, place every referenced object with size 4 and
        # pin the small-data threshold that makes it reachable through $gp.
        objects = gp_objects(target.assembly)
        if objects and "small_data_symbols" not in unit:
            unit["small_data_symbols"] = {name: 4 for name in objects}
            flags = list(unit.get("compiler_flags",
                                  self.config.get("compiler_flags", [])))
            if not any(flag.startswith("-G") and flag not in ("-G0",) for flag in flags):
                flags = [flag for flag in flags if not flag.startswith("-G")] + ["-G4"]
                unit["compiler_flags"] = flags
        try:
            with tempfile.TemporaryDirectory(prefix="future-cop-worker-") as directory:
                actual = matching.compile_unit(
                    unit, self.config, self.args.binutils_prefix, Path(directory),
                    environment=compiler_environment(),
                )
            return actual, ""
        except (subprocess.CalledProcessError, OSError, ValueError) as error:
            return None, str(error)

    @staticmethod
    def failed_score(expected: bytes) -> dict[str, Any]:
        score = score_bytes(b"", expected)
        score["exact"] = False
        score["compile_failed"] = True
        return score

    @staticmethod
    def better(score: dict[str, Any], best: dict[str, Any] | None) -> bool:
        if best is None:
            return True
        previous = best["score"]
        return (score["exact"], score["equal_words"], score["equal_bytes"],
                score["matching_prefix_bytes"], -score["compiled_size"]) > (
                    previous["exact"], previous["equal_words"], previous["equal_bytes"],
                    previous["matching_prefix_bytes"], -previous["compiled_size"]
                )

    def run_target(self, target: Target) -> dict[str, Any]:
        target_dir = self.output / "work" / target.name
        state_path = target_dir / "state.json"
        state = load_json(state_path, {
            "schema": 1,
            "name": target.name,
            "address": f"0x{target.address:08X}",
            "size": target.size,
            "attempts": 0,
            "status": "pending",
        })
        if state.get("status") == "exact":
            return state

        initial = self.initial_source(target)
        if initial is not None and state["attempts"] == 0 and "best" not in state:
            seed_path = target_dir / "seed.c"
            seed_path.parent.mkdir(parents=True, exist_ok=True)
            seed_path.write_text(initial)
            actual, diagnostic = self.compile(target, seed_path, {})
            score = (score_bytes(actual, self.expected(target)) if actual is not None
                     else self.failed_score(self.expected(target)))
            state["best"] = {
                "source": str(seed_path.relative_to(self.output)),
                "symbols": {},
                "score": score,
                "compiler_diagnostic": diagnostic,
            }
            atomic_json(state_path, state)

        while state["attempts"] < self.args.attempts and state.get("status") != "exact":
            attempt = state["attempts"] + 1
            attempt_dir = target_dir / "attempts" / f"{attempt:06d}"
            attempt_dir.mkdir(parents=True, exist_ok=True)
            try:
                source, symbols, note, extras = self.call_backend(self.request(target, state))
                source_hash = hashlib.sha256(source.encode()).hexdigest()
                seen_sources = set(state.get("seen_source_sha256", []))
                if source_hash in seen_sources:
                    raise ValueError("proposal repeated an already evaluated source")
                seen_sources.add(source_hash)
                state["seen_source_sha256"] = sorted(seen_sources)
                source_path = attempt_dir / "candidate.c"
                source_path.write_text(source)
                actual, diagnostic = self.compile(target, source_path, symbols, extras)
                score = (score_bytes(actual, self.expected(target)) if actual is not None
                         else self.failed_score(self.expected(target)))
                result = {"score": score, "symbols": symbols, "note": note,
                          "extras": extras,
                          "compiler_diagnostic": diagnostic,
                          "source_sha256": source_hash}
                atomic_json(attempt_dir / "result.json", result)
                if self.better(score, state.get("best")):
                    state["best"] = {
                        "source": str(source_path.relative_to(self.output)),
                        "symbols": symbols,
                        "score": score,
                        "compiler_diagnostic": diagnostic,
                    }
                if score["exact"]:
                    inbox = self.output / "inbox" / target.name
                    inbox.mkdir(parents=True, exist_ok=True)
                    (inbox / "candidate.c").write_text(source)
                    atomic_json(inbox / "unit.json", {
                        "name": target.name,
                        "address": f"0x{target.address:08X}",
                        "size": f"0x{target.size:X}",
                        "source": "candidate.c",
                        "symbols": inferred_symbols(target) | symbols,
                        "source_sha256": hashlib.sha256(source.encode()).hexdigest(),
                        "matched": True,
                        "semantic_review_required": True,
                    })
                    state["status"] = "exact"
            except (json.JSONDecodeError, subprocess.TimeoutExpired, RuntimeError,
                    ValueError) as error:
                atomic_json(attempt_dir / "result.json", {"error": str(error)})
                state["last_error"] = str(error)
            state["attempts"] = attempt
            atomic_json(state_path, state)
            with self.print_lock:
                best = state.get("best", {}).get("score", {})
                print(f"{target.name} attempt {attempt}: {state.get('status')} "
                      f"words={best.get('equal_words', 0)}/{best.get('total_words', 0)}",
                      flush=True)
        return state


def matched_names() -> set[str]:
    return {unit["name"] for unit in load_json(MATCHING_MANIFEST, {"units": []})["units"]}


def select_targets(args: argparse.Namespace) -> list[Target]:
    targets = parse_targets(args.assembly.read_text())
    completed = matched_names()
    requested = set(args.function or [])
    skipped = [target for target in targets
               if target.name not in completed
               and args.min_size <= target.size <= args.max_size
               and (not requested or target.name in requested)
               and BIOS_DISPATCH.search(target.assembly)]
    selected = [target for target in targets
                if target.name not in completed
                and args.min_size <= target.size <= args.max_size
                and (not requested or target.name in requested)
                and not BIOS_DISPATCH.search(target.assembly)]
    if skipped:
        print(f"Skipping {len(skipped)} BIOS dispatch target(s): $t2 jumps cannot be "
              f"reproduced in C because the pinned compiler always emits an epilogue.")
    if getattr(args, "pure_c", False):
        hardware = [target for target in selected
                    if HARDWARE_ONLY.search(target.assembly)]
        selected = [target for target in selected
                    if not HARDWARE_ONLY.search(target.assembly)]
        if hardware:
            print(f"Skipping {len(hardware)} hardware-only target(s) (GTE/syscall): "
                  f"they need inline assembly, not reconstructed C.")
    selected.sort(key=lambda target: (target.size, target.address))
    if requested - {target.name for target in selected}:
        missing = ", ".join(sorted(requested - {target.name for target in selected}))
        raise SystemExit(f"requested targets are unavailable or outside size limits: {missing}")
    return selected[:args.targets]


def run_command(args: argparse.Namespace) -> int:
    targets = select_targets(args)
    if not targets:
        print("No eligible unmatched targets.")
        return 0
    runner = Runner(args)
    print(f"Running {len(targets)} target(s), {args.jobs} parallel job(s), "
          f"up to {args.attempts} cumulative attempt(s) per target.")
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as executor:
        states = list(executor.map(runner.run_target, targets))
    exact = sum(state.get("status") == "exact" for state in states)
    print(f"Run complete: {exact} exact match(es); review {runner.output / 'inbox'}")
    return 0


def promote_command(args: argparse.Namespace) -> int:
    """Copy confirmed inbox candidates into src/game/ and the matching manifest.

    The manifest row carries the acceptance parameters, and compiler flags are
    **merged** with the project defaults: replacing them (a proposal only supplies
    its extra flags) silently changes optimisation and breaks the match.
    """
    inbox = args.output.resolve() / "inbox"
    if not inbox.is_dir():
        print("no inbox to promote")
        return 0
    config = load_json(MATCHING_MANIFEST, {})
    defaults = config.get("compiler_flags", [])
    existing = {unit["name"] for unit in config.get("units", [])}
    added = []
    for unit_path in sorted(inbox.glob("*/unit.json")):
        candidate = json.loads(unit_path.read_text())
        if candidate["name"] in existing:
            continue
        address = int(candidate["address"], 0)
        source = ROOT / f"src/game/{address:08X}.c"
        source.parent.mkdir(parents=True, exist_ok=True)
        source.write_text((unit_path.parent / candidate["source"]).read_text())
        extras: dict[str, Any] = {}
        work_state = inbox.parent / "work" / candidate["name"] / "state.json"
        if work_state.is_file():
            state = json.loads(work_state.read_text())
            best_source = state.get("best", {}).get("source")
            if best_source:
                result_path = (inbox.parent / best_source).parent / "result.json"
                if result_path.is_file():
                    extras = json.loads(result_path.read_text()).get("extras", {})
        flags = list(defaults)
        flags += [flag for flag in extras.get("compiler_flags_extra", [])
                  if flag not in flags]
        config["units"].append({
            "name": candidate["name"],
            "address": candidate["address"],
            "size": candidate["size"],
            "source": str(source.relative_to(ROOT)),
            "compiler": extras.get("compiler", config["compiler"]),
            "compiler_flags": flags,
            "symbols": candidate.get("symbols", {}),
            "link": False,
        })
        added.append(candidate["name"])
    if added:
        config["units"].sort(key=lambda unit: int(unit["address"], 0))
        MATCHING_MANIFEST.write_text(json.dumps(config, indent=2) + "\n")
    print(f"promoted {len(added)}: {', '.join(added) if added else 'nothing'}")
    return 0


def preview_command(args: argparse.Namespace) -> int:
    """Compile the review-inbox candidates independently and print a verdict per unit.

    Applies the project's own extent rule (``verify_matching_units.verify``): the
    declared range must match exactly and any extra bytes must be zero padding.
    """
    inbox = args.output.resolve() / "inbox"
    if not inbox.is_dir():
        print("no inbox to review")
        return 0
    config = load_json(MATCHING_MANIFEST, {})
    executable = args.executable.read_bytes()
    confirmed = zero_tail = failed = 0
    for unit_path in sorted(inbox.glob("*/unit.json")):
        candidate = json.loads(unit_path.read_text())
        source = unit_path.parent / candidate["source"]
        address, size = int(candidate["address"], 0), int(candidate["size"], 0)
        unit = {
            "name": candidate["name"], "address": address, "size": size,
            "source": str(source),
            "symbols": candidate.get("symbols", {}),
            # gp objects are resolved from the oracle, because the inbox unit.json
            # only carries the raw proposal.
            "small_data_symbols": {}, "compiler_flags": config.get("compiler_flags", []),
        }
        target = next((t for t in parse_targets(args.assembly.read_text())
                       if t.name == candidate["name"]), None)
        if target is not None and gp_objects(target.assembly):
            unit["small_data_symbols"] = {name: 4 for name in gp_objects(target.assembly)}
            unit["compiler_flags"] = [flag for flag in unit["compiler_flags"]
                                      if not flag.startswith("-G")] + ["-G4"]
        # The acceptance parameters live in the winning attempt, not in the inbox
        # stub: without them a compiler/flag choice would look like a failure.
        work_state = inbox.parent / "work" / candidate["name"] / "state.json"
        if work_state.is_file():
            state = json.loads(work_state.read_text())
            best_source = state.get("best", {}).get("source")
            if best_source:
                attempt_result = (inbox.parent / best_source).parent / "result.json"
                if attempt_result.is_file():
                    extras = json.loads(attempt_result.read_text()).get("extras", {})
                    unit.update(extras)
        text = source.read_text()
        # Inline-assembly units must compile with the same flags they were accepted
        # with: a different -G threshold changes nothing in the asm block itself but
        # keeps the acceptance reproducible.
        if "__asm__" in text:
            unit["compiler_flags"] = config.get("compiler_flags", [])
        try:
            with tempfile.TemporaryDirectory(prefix="future-cop-review-") as directory:
                actual = matching.compile_unit(unit, config, args.binutils_prefix,
                                               Path(directory),
                                               environment=compiler_environment())
        except (subprocess.CalledProcessError, OSError, ValueError) as error:
            print(f"FAIL {candidate['name']}: {error}")
            failed += 1
            continue
        offset = matching.HEADER_SIZE + address - matching.LOAD_ADDRESS
        expected = executable[offset:offset + size]
        tail = actual[size:]
        if actual[:size] != expected or any(tail):
            print(f"FAIL {candidate['name']}: bytes differ or nonzero tail")
            failed += 1
        elif tail:
            print(f"OK   {candidate['name']}: {size} bytes, {len(tail)} bytes zero padding "
                  f"({'inline asm' if '__asm__' in text else 'C'})")
            confirmed += 1
            zero_tail += 1
        else:
            print(f"OK   {candidate['name']}: {size} bytes exact "
                  f"({'inline asm' if '__asm__' in text else 'C'})")
            confirmed += 1
    print(f"confirmed {confirmed}, of those {zero_tail} with zero padding, failed {failed}")
    return 0 if failed == 0 else 1


def status_command(args: argparse.Namespace) -> int:
    states = []
    for path in sorted((args.output / "work").glob("*/state.json")):
        states.append(load_json(path, {}))
    exact = [state for state in states if state.get("status") == "exact"]
    attempts = sum(state.get("attempts", 0) for state in states)
    print(f"targets={len(states)} exact={len(exact)} attempts={attempts}")
    for state in exact:
        print(f"EXACT {state['name']} {state['size']} bytes")
    return 0


def parser() -> argparse.ArgumentParser:
    result = argparse.ArgumentParser(description=__doc__)
    subparsers = result.add_subparsers(dest="action", required=True)
    run_parser = subparsers.add_parser("run", help="run or resume reconstruction jobs")
    run_parser.add_argument("--backend-command", required=True,
                            help="local JSON proposal command, parsed like a shell command")
    run_parser.add_argument("--jobs", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    run_parser.add_argument("--attempts", type=int, default=50,
                            help="cumulative attempt ceiling for each target")
    run_parser.add_argument("--targets", type=int, default=16)
    run_parser.add_argument("--min-size", type=lambda value: int(value, 0), default=8)
    run_parser.add_argument("--max-size", type=lambda value: int(value, 0), default=0x80)
    run_parser.add_argument("--function", action="append",
                            help="restrict work to a function; may be repeated")
    run_parser.add_argument("--pure-c", action="store_true",
                            help="skip targets that can only be written as inline assembly")
    run_parser.add_argument("--timeout", type=int, default=600)
    run_parser.add_argument("--assembly", type=Path, default=DEFAULT_ASSEMBLY)
    run_parser.add_argument("--executable", type=Path, default=DEFAULT_EXECUTABLE)
    run_parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    run_parser.add_argument("--binutils-prefix", default=os.environ.get(
        "MIPSEL_PREFIX", "mipsel-linux-gnu-"))
    run_parser.set_defaults(handler=run_command)
    status_parser = subparsers.add_parser("status", help="summarize saved worker state")
    status_parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    status_parser.set_defaults(handler=status_command)
    preview_parser = subparsers.add_parser(
        "preview", help="recompile review-inbox candidates and print a verdict")
    preview_parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    preview_parser.add_argument("--assembly", type=Path, default=DEFAULT_ASSEMBLY)
    preview_parser.add_argument("--executable", type=Path, default=DEFAULT_EXECUTABLE)
    preview_parser.add_argument("--binutils-prefix", default=os.environ.get(
        "MIPSEL_PREFIX", "mipsel-linux-gnu-"))
    preview_parser.set_defaults(handler=preview_command)
    promote_parser = subparsers.add_parser(
        "promote", help="copy and register confirmed inbox candidates")
    promote_parser.add_argument("--output", type=Path, default=DEFAULT_OUTPUT)
    promote_parser.set_defaults(handler=promote_command)
    return result


if __name__ == "__main__":
    arguments = parser().parse_args()
    if getattr(arguments, "jobs", 1) < 1 or getattr(arguments, "attempts", 1) < 1:
        raise SystemExit("jobs and attempts must be positive")
    sys.exit(arguments.handler(arguments))
