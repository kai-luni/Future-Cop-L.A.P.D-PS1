#!/usr/bin/env python3
"""Line coverage for the host tests of the decompiled C.

The repo's host tests run the real target code under qemu-mipsel. Neither gcov
nor lcov can be used: the toolchain is gcc 2.7.2 (no -fprofile-arcs) and the
tests are MIPS ELFs. What is available is enough to measure line coverage:

  * gcc 2.7.2 with -g emits .loc directives, so `mipsel-linux-gnu-objdump -l`
    maps every instruction to a source line;
  * qemu-mipsel -d in_asm prints every translated (i.e. executed) basic block
    with its addresses.

The probe compiles one unit at the same flags the test harness uses, disassembles
it with line info, then runs the matching test binary and intersects the
executed addresses with the line map. Output is a per-file table plus an
optional LCOV tracefile.

Usage:
    python3 tools/host_coverage.py                     # all host tests, summary
    python3 tools/host_coverage.py --lcov cov.info     # also write LCOV
    python3 tools/host_coverage.py --test weapon_input # one test only
    python3 tools/host_coverage.py --unit 8006BA50     # one unit only

The measurement is honest about its limits: it counts a line as covered when at
least one instruction attributed to it executed, and it can only see lines that
produced code. Declarations, comments and pure struct definitions never count.
"""

import argparse
import json
import os
import re
import subprocess
import sys
import tempfile
from pathlib import Path

REPO = Path(__file__).resolve().parent.parent
TOOLCHAIN = REPO / ".toolchain"
GCC = TOOLCHAIN / "gcc-2.7.2-cdk" / "gcc"
BINUTILS = TOOLCHAIN / "mipsel-binutils" / "root" / "usr" / "bin"
QEMU = Path(
    os.environ.get(
        "QEMU", Path.home() / ".local/share/qemu-user/extracted/usr/bin/qemu-mipsel"
    )
)

# Test script -> the units it compiles, as named in the script's unit loop.
TEST_RE = re.compile(r"^for unit in ([\dA-Fa-f ]+); do", re.M)


def toolchain_env():
    env = dict(os.environ)
    env["PATH"] = f"{BINUTILS}:{env.get('PATH', '')}"
    env["LD_LIBRARY_PATH"] = (
        f"{TOOLCHAIN}/mipsel-binutils/root/usr/lib/x86_64-linux-gnu"
        f"{':' + env['LD_LIBRARY_PATH'] if env.get('LD_LIBRARY_PATH') else ''}"
    )
    return env


def compile_unit(unit, outdir, env):
    """Compile one unit the way the harness does and return the object path."""
    src = REPO / "src" / "game" / f"{unit}.c"
    if not src.exists():
        return None, f"missing source {src}"

    raw = outdir / f"{unit}.s"
    mas = outdir / f"{unit}.mas.s"
    obj = outdir / f"{unit}.o"

    step = [
        [str(GCC), f"-B{TOOLCHAIN / 'gcc-2.7.2-cdk'}/", f"-I{REPO / 'include'}",
         "-O1", "-G0", "-g", "-S", str(src), "-o", str(raw)],
    ]
    for cmd in step:
        r = subprocess.run(cmd, capture_output=True, text=True, env=env)
        if r.returncode != 0:
            return None, f"gcc failed: {r.stderr.strip()[:300]}"

    maspsx = sorted(TOOLCHAIN.glob("maspsx-*/maspsx.py"))
    if not maspsx:
        return None, "maspsx not found"
    with open(mas, "w") as fh:
        r = subprocess.run(
            [sys.executable, str(maspsx[0]), str(raw)],
            stdout=fh, stderr=subprocess.DEVNULL, text=True, env=env,
        )
    if r.returncode != 0:
        return None, "maspsx failed"

    # The harness pipes maspsx through fix_align.py; do the same so the
    # addresses match the linked test binary.
    fix = REPO / "tests" / "host" / "fix_align.py"
    if fix.exists():
        fixed = outdir / f"{unit}.fixed.s"
        with open(fixed, "w") as fh:
            r = subprocess.run(
                [sys.executable, str(fix)], stdin=open(mas), stdout=fh,
                text=True, env=env,
            )
        if r.returncode == 0:
            mas = fixed

    r = subprocess.run(
        ["mipsel-linux-gnu-as", "-o", str(obj), str(mas)],
        capture_output=True, text=True, env=env,
    )
    if r.returncode != 0:
        return None, f"as failed: {r.stderr.strip()[:300]}"
    return obj, None


def line_map(obj, env):
    """Map instruction address (relative to the unit) to source line."""
    r = subprocess.run(
        ["mipsel-linux-gnu-objdump", "-d", "-l", str(obj)],
        capture_output=True, text=True, env=env,
    )
    if r.returncode != 0:
        return {}

    mapping = {}
    current_line = None
    current_file = None
    for line in r.stdout.splitlines():
        m = re.match(r"^(/\S+):(\d+)$", line.strip())
        if m:
            current_file = m.group(1)
            current_line = int(m.group(2))
            continue
        # 18:\t8e5000ac \tlw\ts0,172(s2)
        m = re.match(r"^\s*([0-9a-f]+):\t", line)
        if m and current_line is not None:
            mapping.setdefault(current_file, {}).setdefault(current_line, set()).add(
                int(m.group(1), 16)
            )
    return mapping


def executed_addresses(binary, env):
    """Every instruction address qemu translated, i.e. actually executed."""
    r = subprocess.run(
        [str(QEMU), "-d", "in_asm", str(binary)],
        capture_output=True, text=True, env=env, timeout=600,
    )
    if r.returncode != 0:
        return set(), f"qemu failed: {r.stderr.strip()[:200]}"
    addrs = set()
    text = r.stderr if r.stderr else r.stdout
    for line in text.splitlines():
        m = re.match(r"^0x([0-9a-f]+):\s", line)
        if m:
            addrs.add(int(m.group(1), 16))
    if not addrs:
        return set(), "qemu produced no in_asm trace"
    return addrs, None


def binary_symbols(binary, env):
    """Address (absolute) -> size for the units in a linked test binary."""
    r = subprocess.run(
        ["mipsel-linux-gnu-nm", "-S", "--defined-only", str(binary)],
        capture_output=True, text=True, env=env,
    )
    syms = {}
    for line in r.stdout.splitlines():
        parts = line.split()
        if len(parts) == 4:
            addr, size, typ, name = parts
            if typ.lower() == "t":
                syms[name] = (int(addr, 16), int(size, 16))
    return syms


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--lcov", help="write an LCOV tracefile here")
    ap.add_argument("--test", help="only this test script stem, e.g. weapon_input")
    ap.add_argument("--unit", action="append", help="only this unit, repeatable")
    ap.add_argument("--json", help="write the summary as JSON here")
    ap.add_argument("--verbose", action="store_true")
    args = ap.parse_args()

    env = toolchain_env()
    tests_dir = REPO / "tests" / "host"
    report = {}

    scripts = sorted(tests_dir.glob("*_test.sh"))
    if args.test:
        scripts = [s for s in scripts if s.stem == f"{args.test}_test"
                   or s.stem == args.test]

    if not scripts:
        print("no matching test script found", file=sys.stderr)
        return 2

    for script in scripts:
        text = script.read_text(errors="ignore")
        units = []
        for m in TEST_RE.finditer(text):
            units.extend(u for u in m.group(1).split() if u)
        if not units:
            if args.verbose:
                print(f"skip {script.name}: no statically known unit list")
            continue
        if args.unit:
            units = [u for u in units if u in args.unit]
            if not units:
                continue

        build = tests_dir / f"build-{script.stem.replace('_test', '').replace('_', '-')}"
        binary = None
        if build.is_dir():
            for cand in build.iterdir():
                if cand.is_file() and os.access(cand, os.X_OK) and not cand.suffix:
                    binary = cand
        if binary is None:
            print(f"skip {script.name}: no built binary (run the test first)",
                  file=sys.stderr)
            continue

        ex_addrs, err = executed_addresses(binary, env)
        if err:
            print(f"skip {script.name}: {err}", file=sys.stderr)
            continue
        syms = binary_symbols(binary, env)

        with tempfile.TemporaryDirectory() as tmp:
            for unit in units:
                sym = f"func_{unit}"
                if sym not in syms:
                    if args.verbose:
                        print(f"  {unit}: not linked into {binary.name}")
                    continue
                base, size = syms[sym]
                obj, err = compile_unit(unit, Path(tmp), env)
                if err:
                    print(f"  {unit}: {err}", file=sys.stderr)
                    continue
                lines = line_map(obj, env)

                for fname, per_line in lines.items():
                    if fname is None:
                        continue
                    rel = os.path.relpath(fname, REPO)
                    entry = report.setdefault(rel, {"covered": set(), "total": set()})
                    for lineno, offsets in per_line.items():
                        entry["total"].add(lineno)
                        if any(base + off in ex_addrs for off in offsets):
                            entry["covered"].add(lineno)

    if not report:
        print("nothing measured", file=sys.stderr)
        return 1

    # Summary table
    rows = []
    tot_c = tot_t = 0
    for fname in sorted(report):
        c = len(report[fname]["covered"])
        t = len(report[fname]["total"])
        tot_c += c
        tot_t += t
        pct = (100.0 * c / t) if t else 0.0
        rows.append((rel_width := fname, c, t, pct))

    w = max((len(r[0]) for r in rows), default=10)
    print(f"{'file'.ljust(w)}  covered  total    pct")
    print("-" * (w + 26))
    for fname, c, t, pct in rows:
        print(f"{fname.ljust(w)}  {c:7d}  {t:5d}  {pct:5.1f}%")
    print("-" * (w + 26))
    overall = (100.0 * tot_c / tot_t) if tot_t else 0.0
    print(f"{'TOTAL'.ljust(w)}  {tot_c:7d}  {tot_t:5d}  {overall:5.1f}%")

    if args.lcov:
        with open(args.lcov, "w") as fh:
            for fname in sorted(report):
                fh.write(f"SF:{REPO / fname}\n")
                for lineno in sorted(report[fname]["total"]):
                    hits = 1 if lineno in report[fname]["covered"] else 0
                    fh.write(f"DA:{lineno},{hits}\n")
                c = len(report[fname]["covered"])
                t = len(report[fname]["total"])
                fh.write(f"LF:{t}\nLH:{c}\nend_of_record\n")
        print(f"\nLCOV written to {args.lcov}")

    if args.json:
        out = {
            fname: {
                "covered": sorted(report[fname]["covered"]),
                "total": sorted(report[fname]["total"]),
                "pct": round(100.0 * len(report[fname]["covered"])
                             / max(1, len(report[fname]["total"])), 1),
            }
            for fname in report
        }
        out["_summary"] = {"covered": tot_c, "total": tot_t,
                           "pct": round(overall, 1)}
        Path(args.json).write_text(json.dumps(out, indent=1))
        print(f"JSON written to {args.json}")

    return 0


if __name__ == "__main__":
    sys.exit(main())
