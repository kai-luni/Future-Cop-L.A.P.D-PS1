#!/usr/bin/env python3
"""Sweep the pinned compiler matrix over every open near-miss.

A near-miss is cheap to finish when the remaining differences come from the compiler
family (epilogue order) or the optimisation level, so before touching C this script
compiles each known best candidate with every pinned GCC at -O1/-O2/-O3, keeping the
candidate's own maspsx flags and link setting. Candidates above 90 % get a second pass
that also flips the link path and toggles maspsx --dont-expand-li.

Writes build/us/matrix/results.json and prints one line per candidate.
"""

from __future__ import annotations

import hashlib
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import offline_decomp as worker  # noqa: E402
import verify_matching_units as matching  # noqa: E402

CONFIG = json.loads((ROOT / "config/us/matching_units.json").read_text())
EXECUTABLE = worker.DEFAULT_EXECUTABLE.read_bytes()
TARGETS = {t.name: t for t in worker.parse_targets(worker.DEFAULT_ASSEMBLY.read_text())}
OUT = ROOT / "build/us/matrix"
SEARCH = [ROOT / "build/us" / name for name in (
    "ab-B-32k", "offline-decomp-div2", "offline-decomp", "offline-decomp-gp2", "offline-decomp-div",
)]
OPT_LEVELS = ("-O1", "-O2", "-O3")


def candidates(min_ratio: float) -> list[dict]:
    solved = worker.matched_names()
    found: dict[str, dict] = {}
    for directory in SEARCH:
        for state_path in sorted((directory / "work").glob("*/state.json")):
            state = json.loads(state_path.read_text())
            name = state["name"]
            best = state.get("best")
            if not best or state.get("status") == "exact" or name in solved or name not in TARGETS:
                continue
            score = best.get("score") or {}
            total = score.get("total_words") or 0
            ratio = (score.get("equal_words") or 0) / total if total else 0.0
            if ratio < min_ratio:
                continue
            previous = found.get(name)
            if previous and previous["ratio"] >= ratio:
                continue
            found[name] = {"name": name, "ratio": ratio, "source": directory / best["source"],
                           "symbols": best.get("symbols") or {}, "extras": best.get("extras") or {},
                           "words": f"{score.get('equal_words')}/{total}"}
    return sorted(found.values(), key=lambda item: -item["ratio"])


def with_level(flags: list[str], level: str) -> list[str]:
    return [flag for flag in flags if not flag.startswith("-O")] + [level]


def compile_once(candidate: dict, compiler: str, level: str, *,
                 link: bool | None = None, li_mode: bool | None = None) -> dict:
    target = TARGETS[candidate["name"]]
    extras = dict(candidate["extras"])
    extras["compiler"] = compiler
    extras["compiler_flags_extra"] = with_level(
        list(extras.get("compiler_flags_extra") or []), level)
    if link is True:
        extras["link"] = True
    elif link is False:
        extras["link"] = False
    maspsx = list(extras.get("maspsx_flags_extra") or [])
    if li_mode is True and "--dont-expand-li" not in maspsx:
        maspsx.append("--dont-expand-li")
    if li_mode is False:
        maspsx = [flag for flag in maspsx if flag != "--dont-expand-li"]
    if li_mode is not None:
        # Write the list back even when it is empty: skipping it would leave the
        # old ``--dont-expand-li`` in ``extras`` and silently undo ``li_mode=False``.
        extras["maspsx_flags_extra"] = maspsx

    runner = worker.Runner.__new__(worker.Runner)
    runner.config = CONFIG
    spec = runner.unit_spec(target, candidate["source"].resolve(), candidate["symbols"], extras)
    for key, value in extras.items():
        spec[key] = value
    # One directory per distinct attempt: target, source content and the complete
    # effective profile. Link and li mode belong in the name, otherwise a later
    # run with another configuration overwrites the evidence of an earlier hit.
    identity = hashlib.sha256(json.dumps({
        "target": candidate["name"],
        "source": hashlib.sha256(candidate["source"].read_bytes()).hexdigest(),
        "profile": {
            key: value for key, value in extras.items()
            if key != "maspsx_flags_extra"
        } | {"maspsx_flags_extra": extras.get("maspsx_flags_extra", [])},
    }, sort_keys=True, default=str).encode()).hexdigest()[:12]
    tags = ("link" if extras.get("link") else "nolink",
            {True: "li-pinned", False: "li-expanded", None: "li-default"}[li_mode])
    slug = f"{candidate['name']}-{compiler}-{level}-{tags[0]}-{tags[1]}-{identity}"
    work = OUT / "work" / slug
    suffix = 1
    while work.exists():
        work = OUT / "work" / f"{slug}-{suffix}"
        suffix += 1
    work.mkdir(parents=True)
    result = {"profile": f"{compiler} {level} {tags[0]} {tags[1]}",
              "compiler": compiler, "level": level, "link": extras.get("link"),
              "li_mode": li_mode, "identity": identity, "work": str(work),
              "source": str(candidate["source"]),
              "maspsx_flags_extra": extras.get("maspsx_flags_extra", [])}
    try:
        code = matching.compile_unit(spec, CONFIG, "mipsel-linux-gnu-", work,
                                     environment=worker.compiler_environment())
    except Exception as error:  # noqa: BLE001 - one profile must not stop the sweep
        result["error"] = str(error)[:200]
        (work / "result.json").write_text(json.dumps(result, indent=2) + "\n")
        return result
    _, expected = matching.target_range(EXECUTABLE, target.address, target.size,
                                        target.name)
    score = matching.compare_bytes(code, expected)
    result.update({
        "words": f"{score['equal_words']}/{score['total_words']}",
        "equal_words": score["equal_words"], "total_words": score["total_words"],
        "exact": score["exact"], "compiled": score["compiled_size"],
        "padding_bytes": score["padding_bytes"],
    })
    (work / "result.json").write_text(json.dumps(result, indent=2) + "\n")
    return result


def main(argv: list[str]) -> int:
    min_ratio = float(argv[0]) if argv else 0.5
    OUT.mkdir(parents=True, exist_ok=True)
    results = []
    for candidate in candidates(min_ratio):
        runs = [compile_once(candidate, compiler, level)
                for compiler in sorted(worker.ALLOWED_COMPILERS)
                for level in OPT_LEVELS]
        hit = next((run for run in runs if run.get("exact")), None)
        if not hit and candidate["ratio"] >= 0.9:
            second = []
            for compiler in sorted(worker.ALLOWED_COMPILERS):
                for level in OPT_LEVELS:
                    for link in (True, False):
                        second.append(compile_once(candidate, compiler, level, link=link))
            runs += second
            hit = next((run for run in runs if run.get("exact")), None)
            if not hit:
                third = [compile_once(candidate, compiler, level, li_mode=True)
                         for compiler in ("gcc-2.7.2-cdk", "gcc-2.8.1-psx")
                         for level in OPT_LEVELS]
                runs += third
                hit = next((run for run in runs if run.get("exact")), None)
        best = max(runs, key=lambda run: (run.get("equal_words") or 0))
        entry = {"name": candidate["name"], "baseline": candidate["words"],
                 "source": str(candidate["source"]), "extras": candidate["extras"],
                 "symbols": candidate["symbols"], "profiles": len(runs),
                 "best_profile": best.get("profile"), "best_words": best.get("words"),
                 "exact_profile": hit.get("profile") if hit else None,
                 "exact": bool(hit)}
        results.append(entry)
        print(f"{candidate['name']}: war {candidate['words']} -> "
              f"{entry['exact_profile'] or entry['best_words']} "
              f"({entry['profiles']} profile)", flush=True)
        (OUT / "results.json").write_text(json.dumps(results, indent=2) + "\n")
    exact = [entry for entry in results if entry["exact"]]
    print(f"fertig: {len(results)} Kandidaten, {len(exact)} exakt"
          + (": " + ", ".join(entry['name'] for entry in exact) if exact else ""), flush=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
