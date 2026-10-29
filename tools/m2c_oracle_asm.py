#!/usr/bin/env python3
"""Turn one Splat oracle block into the plain GNU-as input m2c expects.

The Splat listing carries address/byte comments and project labels; m2c wants bare
instructions. Nothing here is verified output — it is only the input side of the
m2c seed experiment.
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import offline_decomp as worker  # noqa: E402

OUT = ROOT / "build/us/m2c"
COMMENT = re.compile(r"/\*.*?\*/\s*")


def convert(assembly: str, name: str) -> str:
    # m2c needs an explicit function entry label; Splat's ``glabel`` is not assembly.
    lines = [".text", f".globl {name}", f"{name}:"]
    for line in assembly.splitlines():
        stripped = line.strip()
        if stripped.startswith("nonmatching") or stripped.startswith("glabel") \
                or stripped.startswith("endlabel"):
            continue
        if stripped.startswith(".L"):
            lines.append(stripped)
            continue
        body = COMMENT.sub("", line).strip()
        if not body:
            continue
        lines.append(body)
    return "\n".join(lines) + "\n"


def main(argv: list[str]) -> int:
    if not argv:
        raise SystemExit("usage: make_m2c_input.py func_XXXXXXXX [...]")
    OUT.mkdir(parents=True, exist_ok=True)
    targets = {t.name: t for t in worker.parse_targets(worker.DEFAULT_ASSEMBLY.read_text())}
    for name in argv:
        target = targets[name]
        path = OUT / f"{name}.for-m2c.s"
        path.write_text(convert(target.assembly, name))
        print(f"{path}  ({target.size} bytes, 0x{target.address:08X})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
