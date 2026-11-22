#!/usr/bin/env python3
"""Which untested units are worth a host test, and what blocks them?

The reach measurement (`tools/host_test_reach.py`) answers "how many units do
the tests execute today". This tool answers the follow-up question: for every
unit that is *not* reached yet, what would it take to run it, and how many other
units would come along for free?

Model
-----
A unit runs in a host test when every function it can reach is either

  * real decompiled C that itself runs (a "clean" unit), or
  * a stand-in the test provides at the retail symbol name.

So a unit is classified by the closure of its callees, not by its own text:

  clean    no hardware unit anywhere below it. Link the real code, test it.
  hw       some callee below it touches GTE/MMIO/BIOS. Needs a mock boundary.
  unknown  a callee with an unresolved indirect call, or an out-degree that the
           call graph could not resolve. Treated as blocked on purpose: a missing
           edge must never be read as hardware freedom.

The hardware flag comes from the disassembly signature in
`analysis/analysis.json` (`sig.gte`, `sig.mmio`, `sig.syscall`) plus
`bios_call`. Scanning the C text is *not* used to decide: every unit carries a
`// Categories: ...` header that names GTE, so a text grep reports hardware for
units that never touch cop2.

Frontier
--------
For every unit that is not clean, the blockers are its direct callees that are
not clean themselves. Grouping units by that blocker set shows which single
stand-in buys the most newly reachable units - the mock frontier. A blocker
that is itself a hardware unit is a candidate for a stand-in; a blocker that is
merely "unknown" is a call-graph gap and has to be resolved before mocking.

Usage
-----
    python3 tools/host_test_candidates.py                # ranked summary
    python3 tools/host_test_candidates.py --frontier     # mock targets only
    python3 tools/host_test_candidates.py --list clean   # every clean unit
    python3 tools/host_test_candidates.py --json c.json
"""

import argparse
import collections
import json
import pathlib
import re
import sys

REPO = pathlib.Path(__file__).resolve().parent.parent
SRC = REPO / "src" / "game"


def load():
    analysis = json.loads((REPO / "analysis" / "analysis.json").read_text())
    edges = json.loads((REPO / "analysis" / "edges.json").read_text())["edges"]
    byaddr = {f["addr"]: f for f in analysis["functions"]}
    callees = collections.defaultdict(set)
    for e in edges:
        if e.get("kind") != "direct":
            continue
        fr, to = e.get("from"), e.get("to")
        if fr in byaddr and to in byaddr:
            callees[fr].add(to)
    return analysis, byaddr, callees


def hardware_units(byaddr):
    """Units that touch hardware directly, by disassembly signature."""
    out = set()
    for addr, fn in byaddr.items():
        sig = fn.get("sig") or {}
        if sig.get("gte") or sig.get("mmio") or sig.get("syscall") or fn.get("bios_call"):
            out.add(addr)
    return out


def tags(byaddr, addr):
    fn = byaddr[addr]
    sig = fn.get("sig") or {}
    parts = []
    if sig.get("gte"):
        parts.append("gte")
    if sig.get("mmio"):
        parts.append("mmio")
    if fn.get("bios_call") or sig.get("syscall"):
        parts.append("bios")
    return "+".join(parts)


def classify(byaddr, callees, hw):
    """Dependency class per unit: clean, hw or unknown.

    `clean` means: the unit and every function below it are real decompiled C
    with no hardware access anywhere, so linking it into a test binary runs
    everything it needs. `hw` means some unit below it touches GTE/MMIO/BIOS and
    the test has to provide a stand-in. `unknown` means the call graph cannot
    prove either - an unresolved indirect call, or an out-degree that no edge
    explains. Unknown is deliberately not folded into clean: a missing edge must
    never be read as hardware freedom.

    The closure is computed as a least fixed point. A unit is clean only once
    every callee is already known clean, and it is hw as soon as any callee is
    hw, so each pass can only settle a unit, never flip one back. Cycles (the
    call graph has two self/mutual recursions) stay unknown instead of looping.
    """
    state = {a: "hw" for a in hw}
    unknown_seed = set()
    for addr, fn in byaddr.items():
        if addr in state:
            continue
        unresolved = fn.get("unresolved_indirect", 0) > 0
        # out_degree counts call sites, distinct callees can be fewer, so only
        # a deficit is a signal: sites with no edge at all.
        unexplained = fn.get("out_degree", 0) > len(callees.get(addr, ()))
        if unresolved or unexplained:
            unknown_seed.add(addr)

    changed = True
    while changed:
        changed = False
        for addr in byaddr:
            if addr in state:
                continue
            kids = callees.get(addr, ())
            verdicts = [state.get(k) for k in kids]
            if any(v == "hw" for v in verdicts):
                state[addr] = "hw"
            elif kids and any(v is None for v in verdicts):
                continue  # a callee is still open, decide later
            elif addr in unknown_seed:
                state[addr] = "unknown"
            elif all(v == "clean" for v in verdicts):
                state[addr] = "clean"
            else:
                continue
            changed = True

    for addr in byaddr:
        state.setdefault(addr, "unknown")
    return state


def src_units():
    return {int(p.stem, 16): p for p in SRC.glob("*.c")}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json", help="write the full per-unit result here")
    ap.add_argument("--frontier", action="store_true",
                    help="only the mock frontier (which stand-in unblocks what)")
    ap.add_argument("--list", metavar="CLASS",
                    choices=["clean", "hw", "unknown"],
                    help="list every unit of one class")
    ap.add_argument("--top", type=int, default=15,
                    help="how many candidates to print (default 15)")
    args = ap.parse_args()

    analysis, byaddr, callees = load()
    hw = hardware_units(byaddr)
    state = classify(byaddr, callees, hw)
    units = src_units()

    counted = collections.Counter()
    for addr in units:
        counted[state.get(addr, "unknown")] += 1

    total = len(units)
    print(f"C units with source:      {total}")
    for name in ("clean", "hw", "unknown"):
        n = counted[name]
        print(f"  {name:8s}               {n:5d}  ({100.0 * n / total:.1f}%)")
    print(f"hardware units total:     {len(hw)}  (direct GTE/MMIO/BIOS)")

    # Front: for each non-clean unit, its direct blockers.
    front = collections.defaultdict(list)
    for addr in units:
        if state.get(addr) == "clean":
            continue
        blockers = tuple(sorted(c for c in callees.get(addr, ())
                                if state.get(c) != "clean"))
        if blockers:
            front[blockers].append(addr)

    gain = collections.Counter()
    kind = {}
    for blockers, members in front.items():
        for b in blockers:
            gain[b] += len(members)
            kind[b] = state.get(b, "unknown")

    if args.list:
        for addr in sorted(a for a in units if state.get(a) == args.list):
            fn = byaddr.get(addr, {})
            print(f"  {fn.get('name', hex(addr)):16s} {tags(byaddr, addr)}")
        return 0

    if not args.frontier:
        print()
        print("native logic per class (lines in the unit's own body):")
        for name in ("clean", "hw", "unknown"):
            sizes = []
            for addr, path in units.items():
                if state.get(addr) != name:
                    continue
                text = path.read_text(errors="ignore")
                sizes.append(sum(1 for line in text.splitlines()
                                 if re.match(r"^\s{4}\S", line)
                                 and not line.strip().startswith("//")))
            if sizes:
                sizes.sort()
                print(f"  {name:8s} units={len(sizes):5d} median_body={sizes[len(sizes)//2]:3d}"
                      f" max={sizes[-1]:3d}")

    print()
    print("mock frontier - one stand-in, how many non-clean units it unblocks:")
    print(f"  {'symbol':18s} {'class':8s} {'unblocks':>8s}  {'hardware':9s} prototype")
    for sym, n in gain.most_common(args.top):
        fn = byaddr[sym]
        proto = (fn.get("sig") or {}).get("prototype", "")[:58]
        print(f"  {fn.get('name', hex(sym)):18s} {kind[sym]:8s} {n:8d}  "
              f"{tags(byaddr, sym):9s} {proto}")

    if args.json:
        out = {
            "total": total,
            "classes": dict(counted),
            "hardware_units": len(hw),
            "units": {
                byaddr[a].get("name", hex(a)): {
                    "class": state.get(a, "unknown"),
                    "hardware": tags(byaddr, a),
                    "blockers": sorted(byaddr[c].get("name", hex(c))
                                       for c in callees.get(a, ())
                                       if state.get(c) != "clean"),
                }
                for a in sorted(units)
            },
            "frontier": [
                {"symbol": byaddr[s].get("name", hex(s)),
                 "class": kind[s],
                 "hardware": tags(byaddr, s),
                 "unblocks": n}
                for s, n in gain.most_common()
            ],
        }
        pathlib.Path(args.json).write_text(json.dumps(out, indent=1))
        print(f"\nJSON written to {args.json}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
