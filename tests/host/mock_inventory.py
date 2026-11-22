#!/usr/bin/env python3
"""Inventory of every retail-label stand-in defined under tests/host.

Two kinds are reported, because the difference is the whole point of the
shared mock library:

  * ALIASED - the definition is bound to a retail label with __asm__("func_...").
    A wrong binding is a link error, which is what we want.
  * BY-NAME - a function whose C name is already `func_800...`. It links, but
    nothing ties it to the retail label: a typo silently becomes a different
    function, and a missing definition silently becomes an undefined symbol
    resolved by whatever else is in the link.

Usage: python3 tests/host/mock_inventory.py [--json out.json]
"""
import argparse
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parent

TYPE = r"(?:s8|u8|s16|u16|s32|u32|int|void|unsigned|char)"
NAME = r"[A-Za-z_][A-Za-z0-9_]*"

# `s32 gFoo(s32 a, s32 b) __asm__("func_80012345");` -> (ret, name, args, label)
ALIAS = re.compile(
    r"^\s*(?:static\s+)?" + TYPE + r"\s*\*?\s*(" + NAME + r")\s*\(([^;{]*)\)\s*"
    r"__asm__\s*\(\s*\"([A-Za-z0-9_]+)\"\s*\)\s*;"
)
# `void func_80092F38(void *a, u32 b) { ... }` -> (name, args)
BYNAME = re.compile(r"^\s*(?:static\s+)?" + TYPE + r"\s*\*?\s*(func_[0-9A-Fa-f]{8})\s*\(([^;{]*)\)\s*\{")


def norm_args(raw):
    return " ".join(raw.split())


def is_definition(lines, idx, name):
    """True when the declaration at idx is followed by its own body.

    The aliased form puts the alias on the prototype and the body below it:
        s32 gFoo(s32 a) __asm__("func_...");
        s32 gFoo(s32 a)
        {
    so the `{` can be one or two lines further down.
    """
    for j in range(idx + 1, min(idx + 3, len(lines))):
        stripped = lines[j].strip()
        if not stripped:
            continue
        head = re.match(r"^(?:" + TYPE + r")\s*\*?\s*(" + NAME + r")\s*\(", stripped)
        if head and head.group(1) == name:
            norm = re.sub(r"__asm__\s*\([^)]*\)", "", stripped)
            return norm.rstrip().endswith("{") or (
                j + 1 < len(lines) and lines[j + 1].strip() == "{")
        if stripped.startswith("{"):
            return True
        return False
    return False


def scan_file(path):
    """Stand-in definitions/declarations in one file, as a list of dicts."""
    path = Path(path)
    lines = path.read_text(errors="ignore").splitlines()
    rel = path.name
    recs = []
    for i, line in enumerate(lines):
        m = ALIAS.match(line)
        if m:
            name, args, label = m.groups()
            recs.append({
                "file": rel, "label": label, "symbol": name,
                "args": norm_args(args),
                "binding": "asm-alias",
                "kind": "definition" if is_definition(lines, i, name) else "declaration",
                "line": i + 1,
            })
            continue
        b = BYNAME.match(line)
        if b:
            name, args = b.groups()
            recs.append({
                "file": rel, "label": name, "symbol": name,
                "args": norm_args(args),
                "binding": "c-name",
                "kind": "definition",
                "line": i + 1,
            })
    return recs


def scan():
    recs = []
    for path in sorted(ROOT.rglob("*.[ch]")):
        if path.name == "mock_inventory.py":
            continue
        rel = str(path.relative_to(ROOT))
        for rec in scan_file(path):
            rec = dict(rec)
            rec["file"] = rel
            recs.append(rec)
    return recs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--json")
    args = ap.parse_args()

    recs = scan()
    defs = [r for r in recs if r["kind"] == "definition"]
    by_label = defaultdict(list)
    for r in defs:
        by_label[r["label"]].append(r)

    dup = {k: v for k, v in by_label.items()
           if len({r["file"] for r in v}) > 1}
    byname = [r for r in defs if r["binding"] == "c-name"]

    print(f"stand-in definitions:            {len(defs)}")
    print(f"  bound with an asm alias:       {len(defs) - len(byname)}")
    print(f"  bound by C name only:          {len(byname)}")
    print(f"distinct retail labels:          {len(by_label)}")
    print(f"labels defined in >1 file:       {len(dup)}")
    print()
    for label in sorted(by_label):
        rows = by_label[label]
        files = sorted({r["file"] for r in rows})
        mark = "  !! DUPLICATE" if len(files) > 1 else ""
        print(f"{label}{mark}")
        for r in sorted(rows, key=lambda x: x["file"]):
            print(f"   {r['file']:<34} {r['symbol']}({r['args']})  [{r['binding']}]")
    if dup:
        print("\n=== duplicates (the unification targets) ===")
        for label in sorted(dup):
            print(f"{label}: " + ", ".join(sorted({r['file'] for r in dup[label]})))
    if byname:
        print("\n=== bound by C name, not by retail label ===")
        for r in byname:
            print(f"{r['label']} in {r['file']}")

    if args.json:
        Path(args.json).write_text(json.dumps(recs, indent=1))
        print(f"\nJSON written to {args.json}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
