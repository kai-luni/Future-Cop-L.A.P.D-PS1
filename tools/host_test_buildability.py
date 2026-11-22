#!/usr/bin/env python3
"""Do the units the call graph calls "clean" actually build and link?

`tools/host_test_candidates.py` classifies a unit as clean when no hardware
access exists anywhere in its callee closure. That is a statement about the call
graph, not about the build: a unit can still fail to compile under the harness
flags (gcc 2.7.2, -O1 -G0, maspsx), or fail to link because a data global it
reads is missing.

This tool measures the real ceiling. For every clean unit it compiles the
unchanged decomp source the way the host harnesses do, links it together with
every clean unit it actually calls, supplies the remaining data globals as
zeroed slots, and reports:

  ok        compiled and linked with nothing but its D_ data globals bound
  compile   gcc, maspsx, fix_align or as rejected the source
  link      a symbol outside that set is still needed (usually a func_ that is
            not clean C, i.e. the unit needs a mock after all)
  missing   no source file for the unit

Anything reported `ok` can be exercised by a host test today with no hardware
stand-in at all - that is the honest "no mock needed" number.

Two rules keep the number honest
--------------------------------
1. The chain is resolved from the *object files*, not from the requested root
   alone and not from `analysis/edges.json`. A chain is closed when every
   `func_` symbol its objects leave undefined is itself an object of a clean
   unit that gets linked in. Compiling only the root made ordinary clean
   callees look like missing symbols - that was the bug behind
   `80067684: link chain=2 ['func_80067480']`, where func_80067480 is clean and
   compiles fine on its own. Trusting the edge list alone misses 89 units whose
   direct calls the graph never recorded.
2. Only `D_` symbols (retail data globals - plain RAM that is zero before any
   code writes it) are satisfied with a zeroed slot, and `make_bindings` raises
   rather than emit anything else. A `func_` symbol bound as an integer slot
   links happily and then jumps to address 0 at run time, which is how an
   earlier version of this tool reported a false 969/969.

Usage
-----
    python3 tools/host_test_buildability.py                 # summary
    python3 tools/host_test_buildability.py --list ok       # every unit that builds
    python3 tools/host_test_buildability.py --unit 80067684 # one unit, verbose
    python3 tools/host_test_buildability.py --json out.json --jobs 8

Note on --jobs
--------------
Parallel probing runs several gcc 2.7.2 processes against one shared toolchain
binary and can hit "gcc: installation problem, cannot exec 'cc1': Text file
busy" (ETXTBSY) on startup. That is a race in the toolchain, not a property of
the source: run_retry() retries such startup failures, and re-running a unit
singly always settles it. Treat any residual `binding`/`compile` result for a
unit as suspect and confirm it with `--unit`.
"""

import argparse
import concurrent.futures as futures
import json
import os
import re
import subprocess
import sys
import tempfile
import threading
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import host_coverage as hc  # noqa: E402
import host_test_candidates as cand  # noqa: E402

REPO = hc.REPO
GCC = hc.GCC
TOOLCHAIN = hc.TOOLCHAIN
MASPSX = next(TOOLCHAIN.glob("maspsx-*/maspsx.py"))
FIXALIGN = REPO / "tests" / "host" / "fix_align.py"
CRT0 = REPO / "tests" / "host" / "crt0.s"
SRC = REPO / "src" / "game"

# The only symbol class that may be satisfied with a zeroed RAM slot.
DATA_PREFIX = "D_"
FUNC_RE = re.compile(r"^func_([0-9A-Fa-f]{8})$")


# ---------------------------------------------------------------- compiling
# `gcc` execs cc1 from the toolchain directory; when several probe workers start
# at once the kernel can return ETXTBSY for that binary. It is a startup race,
# not a property of the source, so a short retry is the honest handling.
TRANSIENT = ("Text file busy", "Resource temporarily unavailable",
             "cannot exec", "Temporary failure")
MAX_ATTEMPTS = 4


def run_retry(cmd, env, **kw):
    """subprocess.run with a bounded retry for toolchain startup races."""
    last = subprocess.run(cmd, env=env, **kw)
    for attempt in range(1, MAX_ATTEMPTS):
        if last.returncode == 0:
            return last
        text = (last.stderr or "") + (last.stdout or "")
        if not any(t in text for t in TRANSIENT):
            return last
        time.sleep(0.25 * attempt)
        last = subprocess.run(cmd, env=env, **kw)
    return last


def compile_unit_raw(srcpath, outbase, env, label=""):
    """Compile an arbitrary .c the harness way (returns obj, error)."""
    raw = Path(str(outbase) + ".s")
    mas = Path(str(outbase) + ".mas.s")
    obj = Path(str(outbase) + ".o")
    r = run_retry(
        [str(GCC), f"-B{TOOLCHAIN / 'gcc-2.7.2-cdk'}/", f"-I{REPO / 'include'}",
         "-O1", "-G0", "-S", str(srcpath), "-o", str(raw)],
        env, capture_output=True, text=True)
    if r.returncode != 0:
        return None, label + "compile: " + " ".join(r.stderr.split())[:160]
    m = subprocess.run([sys.executable, str(MASPSX), str(raw)],
                       capture_output=True, text=True, env=env)
    if m.returncode != 0:
        return None, label + "maspsx: " + " ".join(m.stderr.split())[:160]
    f = subprocess.run([sys.executable, str(FIXALIGN)], input=m.stdout,
                       capture_output=True, text=True, env=env)
    if f.returncode != 0:
        return None, label + "fix_align: " + " ".join(f.stderr.split())[:160]
    mas.write_text(f.stdout)
    a = subprocess.run(["mipsel-linux-gnu-as", "-o", str(obj), str(mas)],
                       capture_output=True, text=True, env=env)
    if a.returncode != 0:
        return None, label + "as: " + " ".join(a.stderr.split())[:160]
    return obj, None


def compile_unit(unit, outdir, env):
    """Compile one unit of src/game with the harness flags. (obj, error)."""
    src = SRC / f"{unit}.c"
    if not src.exists():
        return None, f"missing source src/game/{unit}.c"
    return compile_unit_raw(src, outdir / unit, env)


# ---------------------------------------------------------------- symbols
def symbols(obj, env):
    """(undefined, defined) symbol names of one object, from nm."""
    r = subprocess.run(["mipsel-linux-gnu-nm", str(obj)],
                       capture_output=True, text=True, env=env)
    und, dfn = set(), set()
    for line in r.stdout.splitlines():
        parts = line.split()
        if len(parts) == 2 and parts[0] == "U":
            und.add(parts[1])
        elif len(parts) >= 3:
            dfn.add(parts[-1])
    return und, dfn


def undefined(obj, env):
    return sorted(symbols(obj, env)[0])


def defined(obj, env):
    return symbols(obj, env)[1]


# ---------------------------------------------------------------- binding
def make_bindings(symbols_, path):
    """Emit a C file that defines every symbol as a zeroed 32-bit slot.

    Data globals in the retail image are plain RAM, so a zeroed slot is the
    honest host stand-in: it is what the original reads before any code has
    written it. Anything else - above all a `func_` - is refused here, because a
    function bound as an integer slot links fine and then jumps to address 0.
    """
    bad = [s for s in symbols_ if not s.startswith(DATA_PREFIX)]
    if bad:
        raise ValueError("refusing to bind non-data symbols: "
                         + ", ".join(sorted(bad)[:8]))
    lines = [f'unsigned int {re.sub(r"\\W", "_", s)} __asm__("{s}");'
             for s in symbols_]
    path.write_text("\n".join(lines) + "\n")


STUB_C = "void __main(void) {}\nint main(void) { return 0; }\n"


# ---------------------------------------------------------------- linker
def classify_failure(builder, syms):
    """Why the linker still wanted these symbols, in terms the caller can act on.

    The point is to separate a unit that needs a hardware stand-in from one that
    is merely missing a file - those are different work items.
    """
    causes = set()
    for sym in syms:
        m = FUNC_RE.match(sym)
        if not m:
            if sym.startswith(DATA_PREFIX):
                causes.add("data")           # D_ but ld still wanted it: odd
            else:
                causes.add("non-data-symbol")  # e.g. a plain-named global
            continue
        addr = int(m.group(1), 16)
        if addr not in builder.units:
            causes.add("no-source")
        elif builder.state.get(addr) == "hw":
            causes.add("hardware")
        elif builder.state.get(addr) == "unknown":
            causes.add("unknown")
        elif addr in builder.syms and sym not in builder.syms[addr][1]:
            causes.add("symbol-mismatch")  # clean, but exports another name
        else:
            causes.add("clean-unlinked")
    return sorted(causes)


def link_chain(builder, chain, label):
    """Link a whole clean chain against zeroed D_ slots.

    Only `D_` symbols (retail data globals, which hold zero before any code
    writes them) are bound; every other symbol is left for the linker to
    complain about. The link is always attempted, because nm reports some
    symbols as undefined that carry no relocation and that ld therefore never
    needs. A remaining `func_` symbol means the chain reaches a function that is
    not clean C, so the test would have to supply or mock it; that is a real
    finding, not a build problem.
    """
    env, outdir = builder.env, builder.outdir
    objs = [builder.objs[a] for a in chain if a in builder.objs]
    if not objs:
        return "link", ["no objects"], [], 0, ["no-objects"]

    # A symbol undefined in one member of the chain is satisfied by another, so
    # subtract everything the chain itself defines before judging.
    und, have = set(), set()
    for addr in chain:
        if addr not in builder.syms:
            continue
        u, d = builder.syms[addr]
        und |= u
        have |= d
    needed = sorted(und - have)
    data_syms = [s for s in needed if s.startswith(DATA_PREFIX)]

    extra = []
    if data_syms:
        bind = outdir / f"{label}.bind.c"
        make_bindings(data_syms, bind)
        bob, err = compile_unit_raw(bind, outdir / f"{label}.bind", env,
                                    "binding: ")
        if err:
            return "link", [err], data_syms, len(objs), ["binding"]
        extra.append(bob)

    # A test binary always provides `main`, and gcc calls `__main` for static
    # initialisers. Supplying both keeps the probe an honest "does this chain
    # link into a runnable program" question rather than a library-link one.
    stub = outdir / f"{label}.stub.c"
    stub.write_text(STUB_C)
    sobj, serr = compile_unit_raw(stub, outdir / f"{label}.stub", env, "stub: ")
    if serr:
        return "link", [serr], data_syms, len(objs), ["stub"]
    extra.append(sobj)

    crt = outdir / "crt0.o"
    if not crt.exists():
        subprocess.run(["mipsel-linux-gnu-as", "-o", str(crt), str(CRT0)],
                       capture_output=True, text=True, env=env)
    out = outdir / f"{label}.elf"
    r = subprocess.run(
        ["mipsel-linux-gnu-ld", "-static", "-e", "__start", "-o", str(out),
         str(crt)] + [str(o) for o in extra] + [str(o) for o in objs],
        capture_output=True, text=True, env=env)
    if r.returncode == 0:
        return "ok", [], data_syms, len(objs), []

    missing = sorted(set(re.findall(r"undefined reference to `([^']+)'", r.stderr)))
    missing = [m for m in missing if m not in ("main", "__main")]
    if not missing:
        # Multiple definition or another link error - report it verbatim, never
        # call it ok.
        return ("link", [" ".join(r.stderr.split())[:200]], data_syms,
                len(objs), ["link-error"])
    return ("link", missing, data_syms, len(objs),
            classify_failure(builder, missing))


# ---------------------------------------------------------------- closure
class Builder:
    """Object cache plus the symbol-resolved closure walk.

    Objects are compiled on demand, so `--unit` mode really links the whole
    chain instead of only the root: a clean callee that has no object yet is
    compiled when the walk reaches it.
    """

    def __init__(self, outdir, env, state, units, jobs=1):
        self.outdir = outdir
        self.env = env
        self.state = state
        self.units = units
        self.jobs = max(1, jobs)
        self.objs = {}
        self.errors = {}
        self.syms = {}
        self._lock = threading.Lock()
        self._inflight = set()

    def ensure(self, addr):
        """Compile one unit if needed. Returns the object path or None."""
        with self._lock:
            if addr in self.objs:
                return self.objs[addr]
            if addr in self.errors:
                return None
            claimed = addr in self._inflight
            if not claimed:
                self._inflight.add(addr)
        if claimed:
            # Another worker owns this unit; wait for it to land.
            for _ in range(2000):
                with self._lock:
                    if addr in self.objs:
                        return self.objs[addr]
                    if addr in self.errors:
                        return None
                time.sleep(0.01)
            return None
        unit = f"{addr:08X}"
        obj, err = compile_unit(unit, self.outdir, self.env)
        with self._lock:
            self._inflight.discard(addr)
            if err:
                self.errors[addr] = err
                return None
            self.objs[addr] = obj
            self.syms[addr] = symbols(obj, self.env)
        return obj

    def prefetch(self, addrs):
        """Compile a batch in parallel (subprocess-bound, so threads are fine)."""
        todo = [a for a in addrs if a not in self.objs and a not in self.errors]
        if not todo:
            return
        with futures.ThreadPoolExecutor(max_workers=self.jobs) as pool:
            list(pool.map(self.ensure, todo))

    def closure(self, root):
        """Units that must be linked to run `root`, and what is still missing.

        Returns (chain, hard_missing):
          chain   every unit that gets linked (root included)
          hard    func_ symbols the chain needs that are not clean C (hardware,
                  unknown, or no source) - the unit needs a mock after all.
        """
        chain, seen, hard = [], set(), set()
        stack = [root]
        while stack:
            cur = stack.pop()
            if cur in seen:
                continue
            seen.add(cur)
            chain.append(cur)
            if self.ensure(cur) is None:
                continue
            und, _ = self.syms[cur]
            for sym in sorted(und):
                m = FUNC_RE.match(sym)
                if not m:
                    continue
                child = int(m.group(1), 16)
                if child in seen:
                    continue
                if self.state.get(child) != "clean" or child not in self.units:
                    hard.add(sym)
                    continue
                stack.append(child)
        return sorted(chain), sorted(hard)


def closure_of(addr, callees, state, units):
    """Every clean unit reachable from `addr` by the analysis edge list.

    Kept for callers that only have the graph (and to compare with the
    symbol-resolved chain). The measurement itself uses Builder.closure, which
    also catches direct calls the edge list is missing.
    """
    seen = set()
    stack = [addr]
    while stack:
        cur = stack.pop()
        if cur in seen:
            continue
        seen.add(cur)
        if cur != addr and state.get(cur) != "clean":
            continue
        for kid in callees.get(cur, ()):
            if kid in units and kid not in seen and state.get(kid) == "clean":
                stack.append(kid)
    return sorted(a for a in seen if a in units)


# ---------------------------------------------------------------- driver
def probe(roots, builder):
    """Probe every root unit. Returns {unit: {...}}.

    The link is always attempted: `nm` marks some symbols undefined that carry
    no relocation, so only ld can say whether a chain is really closed.
    """
    builder.prefetch(roots)
    results = {}
    for addr in sorted(roots):
        unit = f"{addr:08X}"
        if addr not in builder.objs:
            results[unit] = {"state": "missing",
                             "detail": [builder.errors.get(addr, "no object")],
                             "data_globals": [], "chain": 0,
                             "class": builder.state.get(addr), "causes": []}
            continue
        chain, _hard = builder.closure(addr)
        st, missing, bound, n, causes = link_chain(builder, chain, unit)
        results[unit] = {"state": st, "detail": missing,
                         "data_globals": bound, "chain": n,
                         "class": builder.state.get(addr), "causes": causes}
    return results


def builder_err_class(state, units, addr, sym):
    """Short label for why one symbol is not linkable from clean C."""
    if addr not in units:
        return "no-source"
    if state.get(addr) == "hw":
        return "hardware"
    if state.get(addr) == "unknown":
        return "unknown"
    return "clean but exports another name"


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json", help="write the full per-unit result here")
    ap.add_argument("--list", metavar="STATE",
                    choices=["ok", "compile", "link", "missing"])
    ap.add_argument("--unit", help="probe a single unit (hex) and print details")
    ap.add_argument("--jobs", type=int, default=min(8, os.cpu_count() or 4),
                    help="parallel compile jobs (default: min(8, cpus))")
    ap.add_argument("--keep", metavar="DIR",
                    help="keep intermediates in DIR instead of a temp dir")
    args = ap.parse_args()

    env = hc.toolchain_env()
    analysis, byaddr, callees = cand.load()
    state = cand.classify(byaddr, callees, cand.hardware_units(byaddr))
    units = cand.src_units()

    if args.unit:
        want = {int(args.unit, 16)}
    else:
        want = {a for a in units if state.get(a) == "clean"}

    tmp_ctx = None
    if args.keep:
        tmpd = Path(args.keep)
        tmpd.mkdir(parents=True, exist_ok=True)
    else:
        tmp_ctx = tempfile.TemporaryDirectory()
        tmpd = Path(tmp_ctx.name)

    builder = Builder(tmpd, env, state, units, jobs=args.jobs)
    results = probe(want, builder)
    print(f"compiled {len(builder.objs)} objects for {len(want)} requested unit(s)",
          flush=True)

    if args.unit:
        for unit, v in sorted(results.items()):
            print(f"{unit}: {v['state']} class={v['class']} chain={v['chain']} "
                  f"bound_D={len(set(v.get('data_globals') or []))}")
            if v.get("causes"):
                print("  causes:", ", ".join(v["causes"]))
            for d in v["detail"]:
                print("  detail:", d)
        if args.json:
            Path(args.json).write_text(json.dumps(results, indent=1))
        return 0 if all(v["state"] == "ok" for v in results.values()) else 1

    counts = {}
    for v in results.values():
        counts[v["state"]] = counts.get(v["state"], 0) + 1
    total = len(results)
    print(f"clean units probed:  {total}")
    for name in ("ok", "link", "compile", "missing"):
        n = counts.get(name, 0)
        print(f"  {name:8s} {n:5d}  ({100.0 * n / max(1, total):.1f}%)")
    chains = sorted(v["chain"] for v in results.values() if v["chain"])
    if chains:
        print(f"  chain objects: min {chains[0]} median "
              f"{chains[len(chains) // 2]} max {chains[-1]}")
    bound = {s for v in results.values() for s in (v.get("data_globals") or [])}
    print(f"  distinct D_ slots bound: {len(bound)}")

    if args.list:
        for unit in sorted(results):
            if results[unit]["state"] == args.list:
                print(f"  {unit}")
        return 0

    # Group the failures by what they want - that is the mock frontier for this
    # class, measured instead of inferred - and break them down by cause, since
    # a hardware blocker and a missing source file are different work items.
    cause_units = {}
    for unit, v in results.items():
        if v["state"] == "ok":
            continue
        for c in (v.get("causes") or ["unknown"]):
            cause_units.setdefault(c, set()).add(unit)
    if cause_units:
        print("\nfailures by cause (a unit can have more than one):")
        for c, us in sorted(cause_units.items(), key=lambda kv: -len(kv[1])):
            print(f"  {c:18s} {len(us):4d} units")

    missing_all = {}
    for unit, v in results.items():
        if v["state"] != "ok":
            for sym in v["detail"]:
                missing_all.setdefault(sym, []).append(unit)
    if missing_all:
        nfail = total - counts.get("ok", 0)
        print(f"\nfailures: {nfail} units want {len(missing_all)} distinct "
              f"symbols/messages")
        print(f"  {'symbol':22s} {'units':>6s}  cause")
        for sym, users in sorted(missing_all.items(), key=lambda kv: -len(kv[1]))[:25]:
            m = FUNC_RE.match(sym)
            if m:
                a = int(m.group(1), 16)
                why = ("no-source" if a not in units
                       else builder_err_class(state, units, a, sym))
            else:
                why = "non-data symbol"
            print(f"  {sym:22s} {len(users):6d}  {why}")

    if args.json:
        Path(args.json).write_text(json.dumps(results, indent=1))
        print(f"\nJSON written to {args.json}")

    if tmp_ctx:
        tmp_ctx.cleanup()
    return 0


if __name__ == "__main__":
    sys.exit(main())
