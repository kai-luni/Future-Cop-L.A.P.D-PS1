#!/usr/bin/env python3
"""How many decompiled functions do the host tests actually execute?

This is the honest denominator question. `tools/host_coverage.py` measures only
the units a test script names explicitly. A test binary also links their real
dependencies, and harnesses stub out others. So the useful number is: of every
`func_` symbol linked into a test binary, how many have at least one source line
that really ran?

The measurement is the same as the coverage tool - .loc line info from a -g
compile intersected with qemu's in_asm trace - but applied to every linked
symbol, not just the listed units.

Output: a per-function verdict plus the two totals.

    python3 tools/host_test_reach.py
    python3 tools/host_test_reach.py --json reach.json --list
"""

import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import host_coverage as hc  # noqa: E402



def _test_binaries(build, env):
    """Linked test executables in a build dir: ELF of type EXEC, not objects."""
    out = []
    for cand in sorted(build.iterdir()):
        if not cand.is_file() or not os.access(cand, os.X_OK):
            continue
        try:
            with open(cand, "rb") as fh:
                if fh.read(4) != b"\x7fELF":
                    continue
        except OSError:
            continue
        r = subprocess.run(["mipsel-linux-gnu-readelf", "-h", str(cand)],
                           capture_output=True, text=True, env=env)
        if "EXEC" in r.stdout:
            out.append(cand)
    return out


def linked_functions(env):
    """func_ symbol -> list of test binaries that define it."""
    found = {}
    for build in sorted((hc.REPO / "tests" / "host").glob("build-*")):
        if not build.is_dir():
            continue
        for cand in _test_binaries(build, env):
            r = subprocess.run(
                ["mipsel-linux-gnu-nm", "--defined-only", str(cand)],
                capture_output=True, text=True, env=env,
            )
            for line in r.stdout.splitlines():
                parts = line.split()
                if (len(parts) == 3 and parts[1] in ("T", "t")
                        and parts[2].startswith("func_")):
                    found.setdefault(parts[2], []).append(cand)
    return found


TRACE_DIR = Path(os.environ.get("QEMU_TRACE_DIR", "/tmp/qemu-traces"))
_trace_cache = {}


def trace_addresses(binary, env):
    """Executed addresses: from a recorded trace if there is one, else live."""
    name = Path(binary).name
    if name in _trace_cache:
        return _trace_cache[name]
    trace = TRACE_DIR / f"{name}.in_asm"
    addrs = set()
    if trace.exists():
        for line in trace.read_text(errors="ignore").splitlines():
            m = re.match(r"^0x([0-9a-f]+):\s", line)
            if m:
                addrs.add(int(m.group(1), 16))
    if addrs:
        _trace_cache[name] = (addrs, None)
        return _trace_cache[name]
    got = hc.executed_addresses(binary, env)
    _trace_cache[name] = got
    return got


def measure_unit(unit, binaries, env, outdir):
    """Union of covered lines for one unit across every binary that links it.

    A unit can appear in several test binaries and different tests reach
    different branches, so the honest number unions the covered lines instead of
    reporting whichever binary happened to come first.
    """
    obj, err = hc.compile_unit(unit, outdir, env)
    if err:
        return None, err
    lines = hc.line_map(obj, env)
    all_lines = set()
    handwritten = not any(per_line for per_line in lines.values())
    covered = set()
    for per_line in lines.values():
        all_lines.update(per_line)
    for binary in binaries:
        base = symbol_address(binary, f"func_{unit}", env)
        if base is None:
            continue
        ex, err = trace_addresses(binary, env)
        if err:
            continue
        for per_line in lines.values():
            for lineno, offsets in per_line.items():
                if any(base + off in ex for off in offsets):
                    covered.add(lineno)
    if handwritten:
        # A transcription-only unit (inline asm, .word stream) carries no .loc
        # lines, so line coverage cannot speak about it at all. Report whether it
        # executed by looking for any executed address inside its extent.
        for binary in binaries:
            base = symbol_address(binary, f"func_{unit}", env)
            if base is None:
                continue
            ex, err = trace_addresses(binary, env)
            if err:
                continue
            if any(base <= a < base + 0x800 for a in ex):
                return (0, 0, True), None
        return (0, 0, False), None
    return (len(covered), len(all_lines), None), None


def symbol_address(binary, sym, env):
    r = subprocess.run(
        ["mipsel-linux-gnu-nm", "-S", "--defined-only", str(binary)],
        capture_output=True, text=True, env=env,
    )
    for line in r.stdout.splitlines():
        parts = line.split()
        if len(parts) == 4 and parts[3] == sym:
            return int(parts[0], 16)
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json", help="write the per-function result here")
    ap.add_argument("--list", action="store_true", help="list every function")
    args = ap.parse_args()

    env = hc.toolchain_env()
    linked = linked_functions(env)
    if not linked:
        print("no test binaries found - run make check first", file=sys.stderr)
        return 1

    results = {}
    with tempfile.TemporaryDirectory() as tmp:
        for sym in sorted(linked):
            unit = sym[5:]
            src = hc.REPO / "src" / "game" / f"{unit}.c"
            entry = {"unit": unit, "binaries": [b.name for b in linked[sym]]}
            if not src.exists():
                entry.update(state="no source", covered=0, total=0)
                results[sym] = entry
                continue
            got, err = measure_unit(unit, linked[sym], env, Path(tmp))
            if err:
                entry.update(state=f"error: {err[:80]}", covered=0, total=0)
            else:
                covered, total, hand = got
                if hand is True:
                    entry.update(state="executed (handwritten asm, no line info)",
                                 covered=0, total=0)
                elif hand is False:
                    entry.update(state="linked but not executed", covered=0, total=0)
                else:
                    entry.update(
                        state="executed" if covered else "linked but not executed",
                        covered=covered,
                        total=total,
                    )
            results[sym] = entry

    ex = [s for s, v in results.items() if v["state"].startswith("executed")]
    partial = [s for s in ex if 0 < results[s]["covered"] < results[s]["total"]]
    lines_c = sum(v["covered"] for v in results.values())
    lines_t = sum(v["total"] for v in results.values())

    print(f"test binaries:            {len({b for v in results.values() for b in v['binaries']})}")
    print(f"linked func_ symbols:     {len(results)}")
    print(f"  executed by a test:     {len(ex)}")
    print(f"  linked but never run:   {len(results) - len(ex)}")
    hand = [s for s, v in results.items() if "handwritten" in v["state"]]
    print(f"  of those handwritten:   {len(hand)}  (asm-only, no line info)")
    print(f"  executed only partly:   {len(partial)}")
    print(f"source lines measured:    {lines_t}")
    print(f"  of which executed:      {lines_c}"
          f"  ({100.0 * lines_c / lines_t:.1f}%)" if lines_t else "")

    if args.list:
        print("\nnot executed:")
        for s in sorted(results):
            if results[s]["state"] != "executed":
                print(f"  {s}  {results[s]['state']}")

    if args.json:
        Path(args.json).write_text(json.dumps(results, indent=1))
        print(f"\nJSON written to {args.json}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
