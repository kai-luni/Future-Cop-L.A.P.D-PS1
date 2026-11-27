#!/usr/bin/env python3
"""Conservative hardware/unknown classification combining compiled objects and static analysis.

Phase 1.1 verification tool. Combines:
  * disassembly signatures (`analysis/analysis.json`),
  * static direct + indirect edges (`analysis/edges.json`),
  * indirect jump/call site records (`analysis/indirect.json`), and
  * compiled MIPS object symbols, relocations, and instruction scans
    (`nm`, `objdump -r`, `objdump -d`) for every `src/game/*.c` unit.

Uses `tools.host_test_evidence.classify_graph` to compute the conservative
closure with three mutually exclusive classes:
  * `candidate`: no known hardware path and no detected call/source/symbol/cycle
    gap within the merged model; not proof of hardware freedom.
  * `hw`: direct or transitive GTE/COP0/MMIO/Scratchpad/BIOS/syscall access.
  * `unknown`: not completely analyzable (unresolved indirect call, missing
    source unit, out-degree gap, non-standard external symbol, compile failure,
    or non-hardware cycle).

Export mismatches (units whose compiled `.o` does not export `func_<ADDR>`,
such as `80096748` -> `gDivModUnsigned64` and `8007D138` -> `D_8007D138`) are
tracked separately so link-export defects are never confused with hardware.

Usage:
    python3 tools/host_hw_classification.py
    python3 tools/host_hw_classification.py --json build/hw_classification.json
    python3 tools/host_hw_classification.py --list unknown
    python3 tools/host_hw_classification.py --compare-buildability /tmp/buildability_fixed.json
"""

import argparse
import collections
import concurrent.futures as futures
import json
import os
import pathlib
import re
import subprocess
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
TOOLS_DIR = pathlib.Path(__file__).resolve().parent
if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))

import host_coverage as hc  # noqa: E402
import host_test_buildability as htb  # noqa: E402
import host_test_candidates as cand  # noqa: E402
from host_test_evidence import classify_graph  # noqa: E402
import unit_paths  # noqa: E402

SRC = REPO / "src" / "game"
DEFAULT_OBJ_DIR = REPO / "build" / "host-hw-classification-objs"

FUNC_RE = re.compile(r"^func_([0-9A-Fa-f]{8})$")
FUNC_SUFFIX_RE = re.compile(r"^func_([0-9A-Fa-f]{8})_(.+)$")
DATA_RE = re.compile(r"^D_([0-9A-Fa-f]{8})(?:_.+)?$")

COP2_RE = re.compile(
    r"\b(c2|cop2|mfc2|mtc2|cfc2|ctc2|lwc2|swc2|rtps|rtpt|nclip|avsz3|avsz4|"
    r"mvmva|dcpl|dpcs|dpct|intpl|sqr|ncs|nct|ncds|ncdt|nccs|ncct|cdp|cc|gpf|gpl|op)\b"
)
COP0_RE = re.compile(r"\b(mfc0|mtc0|cfc0|ctc0|rfe|tlbr|tlbwi|tlbwr|tlbp)\b")
SYSCALL_RE = re.compile(r"\bsyscall\b")
MMIO_LUI_RE = re.compile(r"\blui\s+[^,]+,0x(1f80|9f80|bf80)\b", re.I)
JALR_RE = re.compile(r"\bjalr\b")


def is_ps1_mmio_or_scratchpad_addr(addr):
    """True for PS1 scratchpad (0x1F800000..03FF) and MMIO (0x1F801000..3FFF)."""
    return (
        (0x1F800000 <= addr <= 0x1F803FFF)
        or (0x9F800000 <= addr <= 0x9F803FFF)
        or (0xBF800000 <= addr <= 0xBF803FFF)
    )


def load_static_inputs(repo=REPO):
    """Load analysis.json, edges.json, indirect.json, and src/game/*.c map."""
    analysis = json.loads((repo / "analysis" / "analysis.json").read_text())
    edges_raw = json.loads((repo / "analysis" / "edges.json").read_text())["edges"]
    indirect_path = repo / "analysis" / "indirect.json"
    indirect_raw = (
        json.loads(indirect_path.read_text()).get("sites", [])
        if indirect_path.exists()
        else []
    )
    byaddr = {f["addr"]: f for f in analysis["functions"]}
    src_units = unit_paths.all_unit_paths(repo)
    return analysis, byaddr, edges_raw, indirect_raw, src_units


def compile_all_units(src_units, obj_dir, env, jobs=8, force=False):
    """Compile all src/game/*.c units incrementally into obj_dir."""
    obj_dir = pathlib.Path(obj_dir)
    obj_dir.mkdir(parents=True, exist_ok=True)

    def _one(item):
        addr, src = item
        unit = f"{addr:08X}"
        obj = obj_dir / f"{unit}.o"
        if not force and obj.exists() and obj.stat().st_mtime >= src.stat().st_mtime:
            return addr, obj, None
        o, err = htb.compile_unit(unit, obj_dir, env)
        return addr, o, err

    objs = {}
    errors = {}
    with futures.ThreadPoolExecutor(max_workers=max(1, jobs)) as pool:
        for addr, obj, err in pool.map(_one, sorted(src_units.items())):
            if err or obj is None:
                errors[addr] = err or "compile failed"
            else:
                objs[addr] = obj
    return objs, errors


def parse_nm_output(text):
    """Parse `nm -A` output -> (und, dfn_global, dfn_local) keyed by int addr."""
    und = collections.defaultdict(set)
    dfn_global = collections.defaultdict(set)
    dfn_local = collections.defaultdict(set)
    for line in text.splitlines():
        if ":" not in line:
            continue
        path_part, rest = line.split(":", 1)
        stem = pathlib.Path(path_part).stem
        try:
            addr = int(stem, 16)
        except ValueError:
            continue
        parts = rest.strip().split()
        if len(parts) == 2 and parts[0] == "U":
            und[addr].add(parts[1])
        elif len(parts) >= 3:
            sym_type, sym_name = parts[1], parts[2]
            if sym_type.isupper():
                dfn_global[addr].add(sym_name)
            else:
                dfn_local[addr].add(sym_name)
    return und, dfn_global, dfn_local


def parse_objdump_relocs(text):
    """Parse `objdump -r` output -> {addr: set(relocated_symbol_names)}."""
    relocs = collections.defaultdict(set)
    cur_addr = None
    for line in text.splitlines():
        if ":     file format" in line:
            stem = pathlib.Path(line.split(":", 1)[0]).stem
            try:
                cur_addr = int(stem, 16)
            except ValueError:
                cur_addr = None
        elif cur_addr is not None and line and line[0] in "0123456789abcdefABCDEF":
            parts = line.split()
            if len(parts) >= 3:
                sym = parts[2].split("+")[0].split("-")[0]
                relocs[cur_addr].add(sym)
    return relocs


def parse_objdump_disasm(text):
    """Scan `objdump -d` output for hardware ops and indirect calls (jalr)."""
    hw_reasons = collections.defaultdict(set)
    obj_jalr = set()
    cur_addr = None
    for line in text.splitlines():
        if ":     file format" in line:
            stem = pathlib.Path(line.split(":", 1)[0]).stem
            try:
                cur_addr = int(stem, 16)
            except ValueError:
                cur_addr = None
        elif cur_addr is not None and "\t" in line:
            parts = line.split("\t")
            if len(parts) >= 3:
                mnem = parts[2].strip()
                insn = " ".join(parts[2:])
                if COP2_RE.search(mnem):
                    hw_reasons[cur_addr].add("obj_gte")
                if COP0_RE.search(mnem):
                    hw_reasons[cur_addr].add("obj_cop0")
                if SYSCALL_RE.search(mnem):
                    hw_reasons[cur_addr].add("obj_syscall")
                if MMIO_LUI_RE.search(insn):
                    hw_reasons[cur_addr].add("obj_mmio_or_scratchpad")
                if JALR_RE.search(mnem):
                    obj_jalr.add(cur_addr)
    return hw_reasons, obj_jalr


def inspect_objects(objs, env, batch_size=500):
    """Extract nm symbols, relocations, and hardware/jalr instructions from .o files."""
    obj_list = [str(objs[a]) for a in sorted(objs)]
    und = collections.defaultdict(set)
    dfn_global = collections.defaultdict(set)
    dfn_local = collections.defaultdict(set)
    relocs = collections.defaultdict(set)
    obj_hw = collections.defaultdict(set)
    obj_jalr = set()

    for i in range(0, len(obj_list), batch_size):
        chunk = obj_list[i:i + batch_size]
        nm_r = subprocess.run(
            ["mipsel-linux-gnu-nm", "-A"] + chunk,
            capture_output=True, text=True, env=env, check=True,
        )
        u, dg, dl = parse_nm_output(nm_r.stdout)
        for a, s in u.items():
            und[a] |= s
        for a, s in dg.items():
            dfn_global[a] |= s
        for a, s in dl.items():
            dfn_local[a] |= s

        rel_r = subprocess.run(
            ["mipsel-linux-gnu-objdump", "-r"] + chunk,
            capture_output=True, text=True, env=env, check=True,
        )
        for a, s in parse_objdump_relocs(rel_r.stdout).items():
            relocs[a] |= s

        dis_r = subprocess.run(
            ["mipsel-linux-gnu-objdump", "-d"] + chunk,
            capture_output=True, text=True, env=env, check=True,
        )
        hw_chunk, jalr_chunk = parse_objdump_disasm(dis_r.stdout)
        for a, s in hw_chunk.items():
            obj_hw[a] |= s
        obj_jalr |= jalr_chunk

    return {
        "und": dict(und),
        "dfn_global": dict(dfn_global),
        "dfn_local": dict(dfn_local),
        "relocs": dict(relocs),
        "obj_hw": dict(obj_hw),
        "obj_jalr": obj_jalr,
    }


def classify_conservative(byaddr, edges_raw, indirect_raw, src_units, obj_info, compile_errors=None):
    """Pure, testable conservative classifier over merged static + object evidence.

    Returns a rich dict with per-unit classes (`candidate`, `hw`, `unknown`),
    direct hardware reasons, incomplete reasons, missing-edge diagnostics, and
    export-mismatch diagnostics.
    """
    compile_errors = compile_errors or {}
    und = obj_info.get("und", {})
    dfn_global = obj_info.get("dfn_global", {})
    relocs = obj_info.get("relocs", {})
    obj_hw = obj_info.get("obj_hw", {})
    obj_jalr = set(obj_info.get("obj_jalr", ()))

    nodes = set(byaddr) | set(src_units)
    static_direct = collections.defaultdict(set)
    static_indirect = collections.defaultdict(set)
    direct_row_counts = collections.Counter()
    indirect_row_counts = collections.Counter()

    for e in edges_raw:
        kind = e.get("kind")
        fr, to = e.get("from"), e.get("to")
        if fr not in nodes:
            continue
        if kind == "direct":
            direct_row_counts[fr] += 1
            if to is not None:
                static_direct[fr].add(to)
        elif kind == "indirect":
            indirect_row_counts[fr] += 1
            if to is not None:
                static_indirect[fr].add(to)

    hw_reasons = collections.defaultdict(set)
    incomplete_reasons = collections.defaultdict(set)
    merged_edges = collections.defaultdict(set)
    obj_func_edges = collections.defaultdict(set)
    nonstandard_syms = collections.defaultdict(set)
    export_mismatches = {}

    # 1. Static disassembly hardware signatures & indirect/out_degree gaps
    for addr, fn in byaddr.items():
        sig = fn.get("sig") or {}
        if sig.get("gte"):
            hw_reasons[addr].add("analysis_gte")
        if sig.get("mmio"):
            hw_reasons[addr].add("analysis_mmio")
        if sig.get("syscall"):
            hw_reasons[addr].add("analysis_syscall")
        if fn.get("bios_call"):
            hw_reasons[addr].add("analysis_bios")
        if fn.get("unresolved_indirect", 0) > 0:
            incomplete_reasons[addr].add("unresolved_indirect_analysis")

    # 2. Indirect site records (bios_dispatch -> hw; unresolved -> incomplete;
    #    resolved jalr targets -> edges)
    for site in indirect_raw:
        fn_addr = site.get("func")
        if fn_addr not in nodes:
            continue
        cls = str(site.get("classification", ""))
        targets = site.get("targets") or []
        if cls.startswith("bios_dispatch:"):
            hw_reasons[fn_addr].add("indirect_bios_dispatch")
        elif not targets:
            incomplete_reasons[fn_addr].add("unresolved_indirect_site")
        elif site.get("kind") == "jalr":
            for t in targets:
                merged_edges[fn_addr].add(t)
            incomplete_reasons[fn_addr].add("indirect_call_ptr")

    # 3. Seed merged edges with static direct + indirect edges
    for fr, tos in static_direct.items():
        merged_edges[fr] |= tos
    for fr, tos in static_indirect.items():
        merged_edges[fr] |= tos

    # 4. Compiled object evidence (symbols, relocations, hardware instructions)
    for addr in sorted(src_units):
        if addr in compile_errors:
            incomplete_reasons[addr].add("compile_error")
            continue
        expected = f"func_{addr:08X}"
        globals_here = set(dfn_global.get(addr, ()))
        if expected not in globals_here:
            export_mismatches[addr] = {
                "expected": expected,
                "exported_globals": sorted(globals_here),
            }
        for reason in obj_hw.get(addr, ()):
            hw_reasons[addr].add(reason)
        if addr in obj_jalr:
            incomplete_reasons[addr].add("obj_indirect_call")

        unit_relocs = set(relocs.get(addr, ()))
        for sym in und.get(addr, ()):
            m = FUNC_RE.match(sym)
            if m:
                target = int(m.group(1), 16)
                obj_func_edges[addr].add(target)
                merged_edges[addr].add(target)
                continue
            ms = FUNC_SUFFIX_RE.match(sym)
            if ms:
                target = int(ms.group(1), 16)
                obj_func_edges[addr].add(target)
                merged_edges[addr].add(target)
                if sym in unit_relocs:
                    nonstandard_syms[addr].add(sym)
                    incomplete_reasons[addr].add("nonstandard_external_symbol")
                continue
            md = DATA_RE.match(sym)
            if md:
                d_addr = int(md.group(1), 16)
                if is_ps1_mmio_or_scratchpad_addr(d_addr):
                    hw_reasons[addr].add("obj_mmio_sym")
                continue
            if sym in unit_relocs:
                nonstandard_syms[addr].add(sym)
                incomplete_reasons[addr].add("nonstandard_external_symbol")

    # 5. Missing-source nodes and out_degree deficits
    for addr in nodes:
        if addr not in src_units:
            incomplete_reasons[addr].add("no_source")
        fn = byaddr.get(addr)
        if fn:
            out_deg = fn.get("out_degree", 0)
            known_edge_sites = direct_row_counts[addr] + indirect_row_counts[addr]
            if out_deg > known_edge_sites and out_deg > len(merged_edges.get(addr, ())):
                incomplete_reasons[addr].add("out_degree_deficit")

    # 6. Conservative graph closure (hw > unknown > candidate)
    evidence_state = classify_graph(
        nodes=nodes,
        edges=merged_edges,
        hardware=set(hw_reasons),
        incomplete=set(incomplete_reasons),
    )
    class_map = dict(evidence_state)

    # 7. Diagnostics: missing static edges & export-mismatch callers
    missing_static_edges = {}
    for addr in sorted(src_units):
        extra = obj_func_edges.get(addr, set()) - static_direct.get(addr, set())
        if extra:
            missing_static_edges[addr] = sorted(extra)

    export_blocked = collections.defaultdict(set)
    for addr in sorted(src_units):
        # Walk merged edges to find any unit transitively depending on an export-mismatched unit
        seen = set()
        stack = list(merged_edges.get(addr, ()))
        while stack:
            cur = stack.pop()
            if cur in seen:
                continue
            seen.add(cur)
            if cur in export_mismatches:
                export_blocked[addr].add(cur)
            stack.extend(merged_edges.get(cur, ()))

    return {
        "state": class_map,
        "evidence_state": evidence_state,
        "merged_edges": {a: sorted(v) for a, v in merged_edges.items()},
        "static_direct": {a: sorted(v) for a, v in static_direct.items()},
        "obj_func_edges": {a: sorted(v) for a, v in obj_func_edges.items()},
        "hw_reasons": {a: sorted(v) for a, v in hw_reasons.items()},
        "incomplete_reasons": {a: sorted(v) for a, v in incomplete_reasons.items()},
        "nonstandard_syms": {a: sorted(v) for a, v in nonstandard_syms.items()},
        "missing_static_edges": missing_static_edges,
        "export_mismatches": export_mismatches,
        "export_blocked": {a: sorted(v) for a, v in export_blocked.items()},
        "compile_errors": dict(compile_errors),
    }


def run_classification(repo=REPO, obj_dir=DEFAULT_OBJ_DIR, jobs=8, force_compile=False):
    """End-to-end classification on the live repository."""
    analysis, byaddr, edges_raw, indirect_raw, src_units = load_static_inputs(repo)
    env = hc.toolchain_env()
    objs, compile_errors = compile_all_units(
        src_units, obj_dir, env, jobs=jobs, force=force_compile
    )
    obj_info = inspect_objects(objs, env)
    result = classify_conservative(
        byaddr, edges_raw, indirect_raw, src_units, obj_info, compile_errors
    )
    return analysis, byaddr, src_units, result


def compare_against_static_and_buildability(byaddr, src_units, result, buildability_path=None):
    """Compute delta vs legacy static-only classifier and optional buildability JSON."""
    _, _, static_callees = cand.load()
    static_hw = cand.hardware_units(byaddr)
    static_state = cand.classify(byaddr, static_callees, static_hw)
    new_state = result["state"]

    transitions = collections.Counter()
    static_clean_reclassified = {"hw": [], "unknown": []}
    for addr in sorted(src_units):
        old_cls = static_state.get(addr, "unknown")
        new_cls = new_state.get(addr, "unknown")
        transitions[(old_cls, new_cls)] += 1
        if old_cls == "clean" and new_cls != "candidate":
            static_clean_reclassified[new_cls].append(f"{addr:08X}")

    summary = {
        "static_counts": dict(collections.Counter(static_state.get(a, "unknown") for a in src_units)),
        "new_counts": dict(collections.Counter(new_state.get(a, "unknown") for a in src_units)),
        "transitions": {f"{o}->{n}": c for (o, n), c in sorted(transitions.items())},
        "static_clean_to_hw": static_clean_reclassified["hw"],
        "static_clean_to_unknown": static_clean_reclassified["unknown"],
    }

    if buildability_path and pathlib.Path(buildability_path).exists():
        bf = json.loads(pathlib.Path(buildability_path).read_text())
        cause_check = collections.defaultdict(collections.Counter)
        for unit_hex, entry in bf.items():
            addr = int(unit_hex, 16)
            ncls = new_state.get(addr, "unknown")
            if entry.get("state") == "ok":
                cause_check["bf_ok"][ncls] += 1
            else:
                for cause in entry.get("causes", ["other"]):
                    cause_check[f"bf_fail:{cause}"][ncls] += 1
        summary["buildability_comparison"] = {k: dict(v) for k, v in sorted(cause_check.items())}

    return summary


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--obj-dir", default=str(DEFAULT_OBJ_DIR),
                    help="Cache directory for compiled .o files")
    ap.add_argument("--jobs", type=int, default=min(12, os.cpu_count() or 4),
                    help="Parallel compile jobs")
    ap.add_argument("--force-compile", action="store_true",
                    help="Recompile all .o files regardless of mtime")
    ap.add_argument("--list", choices=["candidate", "hw", "unknown"],
                    help="List every src/game unit in the given class")
    ap.add_argument("--compare-buildability", metavar="JSON",
                    help="Compare against a historical buildability JSON")
    ap.add_argument("--json", metavar="OUT",
                    help="Write full machine-readable report JSON here")
    args = ap.parse_args()

    _, byaddr, src_units, result = run_classification(
        repo=REPO,
        obj_dir=args.obj_dir,
        jobs=args.jobs,
        force_compile=args.force_compile,
    )
    delta = compare_against_static_and_buildability(
        byaddr, src_units, result, args.compare_buildability
    )

    state = result["state"]
    total = len(src_units)
    if args.list:
        for addr in sorted(a for a in src_units if state.get(a) == args.list):
            name = byaddr.get(addr, {}).get("name", f"func_{addr:08X}")
            print(f"{addr:08X} {name}")
        return 0

    print(f"Conservative hardware/unknown classification ({total} src/game units):")
    for cls in ("candidate", "hw", "unknown"):
        n = delta["new_counts"].get(cls, 0)
        print(f"  {cls:15s} {n:5d}  ({100.0 * n / total:.1f}%)")
    print(f"  direct hw units: {sum(1 for a in src_units if a in result['hw_reasons'])} ")
    print(f"  compile errors:  {len(result['compile_errors'])}")
    print(f"  missing edges (obj func_ not in edges.json direct): ")
    print(f"    all units:     {len(result['missing_static_edges'])} units, "
          f"{sum(len(v) for v in result['missing_static_edges'].values())} edges")
    print(f"  export mismatches: {len(result['export_mismatches'])} units")
    for a, info in sorted(result["export_mismatches"].items()):
        callers = sorted(u for u, blocked in result["export_blocked"].items() if a in blocked)
        print(f"    {a:08X} ({state[a]}): expected {info['expected']}, exports {info['exported_globals']}, "
              f"transitive callers={len(callers)}")

    print("\nTransitions from legacy static-only classifier (host_test_candidates.py):")
    for k, v in delta["transitions"].items():
        print(f"  {k:24s} {v:5d}")

    if "buildability_comparison" in delta:
        print("\nComparison against historical buildability JSON:")
        for k, v in delta["buildability_comparison"].items():
            print(f"  {k:28s} {v}")

    if args.json:
        out = {
            "total_src_units": total,
            "counts": delta["new_counts"],
            "static_comparison": delta,
            "compile_errors": {f"{a:08X}": e for a, e in sorted(result["compile_errors"].items())},
            "export_mismatches": {
                f"{a:08X}": {
                    "class": state[a],
                    **info,
                    "transitive_callers": [
                        f"{u:08X}" for u in sorted(result["export_blocked"])
                        if a in result["export_blocked"][u]
                    ],
                }
                for a, info in sorted(result["export_mismatches"].items())
            },
            "missing_static_edges_summary": {
                "units": len(result["missing_static_edges"]),
                "edges": sum(len(v) for v in result["missing_static_edges"].values()),
            },
            "units": {
                f"{a:08X}": {
                    "name": byaddr.get(a, {}).get("name", f"func_{a:08X}"),
                    "class": state[a],
                    "direct_hw_reasons": result["hw_reasons"].get(a, []),
                    "incomplete_reasons": result["incomplete_reasons"].get(a, []),
                    "missing_static_edges": [f"{t:08X}" for t in result["missing_static_edges"].get(a, [])],
                    "nonstandard_syms": result["nonstandard_syms"].get(a, []),
                    "export_blocked_by": [f"{t:08X}" for t in result["export_blocked"].get(a, [])],
                }
                for a in sorted(src_units)
            },
        }
        pathlib.Path(args.json).write_text(json.dumps(out, indent=1))
        print(f"\nJSON written to {args.json}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
