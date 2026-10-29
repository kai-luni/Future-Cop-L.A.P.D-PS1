#!/usr/bin/env python3
"""Build a per-target guidance file: the project rules plus the solved neighbourhood.

The worker passes `guidance` to the model, so handing in the verified sources of the
functions this target calls (and of its callers) gives argument types, struct layouts
and style that the oracle alone cannot show. Everything included here is byte-verified
in the manifest, so it is evidence, not a guess.
"""
from __future__ import annotations

import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import offline_decomp as worker  # noqa: E402

OUT = ROOT / "build/us/context"
MAX_SOURCE_CHARS = 3000


def solved_sources(names: list[str], manifest: dict) -> list[str]:
    chunks = []
    for name in names:
        unit = manifest.get(name)
        if not unit:
            continue
        path = ROOT / unit["source"]
        if not path.is_file():
            continue
        text = path.read_text()
        if len(text) > MAX_SOURCE_CHARS:
            continue
        chunks.append(f"### `{name}` (`{unit['address']}`, `{unit['size']}` Bytes, "
                      f"Compiler `{unit.get('compiler')}`)\n\n```c\n{text.rstrip()}\n```\n")
    return chunks


def build(name: str) -> str:
    config = json.loads(worker.MATCHING_MANIFEST.read_text())
    manifest = {unit["name"]: unit for unit in config["units"]}
    targets = {t.name: t for t in worker.parse_targets(worker.DEFAULT_ASSEMBLY.read_text())}
    target = targets[name]
    callees = sorted(set(re.findall(r"jal\s+(func_[0-9A-Fa-f]{8})\b", target.assembly)))
    callers = sorted(t.name for t in targets.values()
                     if t.name != name and re.search(rf"jal\s+{name}\b", t.assembly))
    solved_callees = [c for c in callees if c in manifest]
    solved_callers = [c for c in callers if c in manifest]
    gp_objects = sorted(worker.gp_objects(target.assembly))

    parts = [(ROOT / "config/us/reconstruction_rules.md").read_text().rstrip(), "",
             "## This target in detail", "",
             f"- `{name}` at `0x{target.address:08X}`, size `0x{target.size:X}` "
             f"({target.size} bytes).",
             f"- Objects reached through `$gp`: {', '.join(f'`{n}`' for n in gp_objects) or 'none'}."]
    if solved_callers:
        parts.append("- Verified callers: " + ", ".join(f"`{c}`" for c in solved_callers)
                     + " — their sources below fix the argument list and return type.")
    parts.append("- Verified callees: " + (", ".join(f"`{c}`" for c in solved_callees) or "none")
                 + " — call them with exactly these signatures.")
    parts.append("")

    if solved_callers:
        parts += ["## Verified callers (argument proof)", ""] + solved_sources(solved_callers, manifest)
    if solved_callees:
        parts += ["## Verified callees", ""] + solved_sources(solved_callees, manifest)
    parts += ["## Remaining callees without a source", "",
              "Their argument lists must be derived from the oracle: "
              + (", ".join(f"`{c}`" for c in callees if c not in manifest) or "none") + ".", ""]
    return "\n".join(parts)


def main(argv: list[str]) -> int:
    if not argv:
        raise SystemExit("usage: target_context.py func_XXXXXXXX")
    OUT.mkdir(parents=True, exist_ok=True)
    for name in argv:
        path = OUT / f"{name}.rules.md"
        path.write_text(build(name))
        print(f"{path} ({len(path.read_text())} Zeichen)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
