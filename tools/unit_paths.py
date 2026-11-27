#!/usr/bin/env python3
"""Centralized resolution and layout naming for decompiled units under src/game/.

Each unit lives at ``src/game/<group>/<name>.c`` where:
  * ``<group>`` is the directory slug derived from the first-line
    ``// Categories: ...`` tag (or ``other`` when absent), and
  * ``<name>`` is the C alias bound to ``func_<ADDR>`` via
    ``__asm__("func_<ADDR>")`` (or ``func_<ADDR>`` when unbound/asm-only).

``config/us/matching_units.json`` is the primary index mapping retail addresses
and ``func_<ADDR>`` symbols to source paths, with filesystem fallback for
unregistered units (e.g. candidates or handwritten scratch files).
"""

from __future__ import annotations

import argparse
import json
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parent.parent
CATEGORY_RE = re.compile(r"^// Categories: (.+)$")
ADDR_HEX_RE = re.compile(r"^(?:(?:0[xX])|(?:func_))?([0-9A-Fa-f]{8})$")
STEM_ADDR_RE = re.compile(r"^(?:func_)?([0-9A-Fa-f]{8})$")
ASM_ALIAS_ADDR_RE = re.compile(r'__asm__\s*\(\s*"func_([0-9A-Fa-f]{8})"\s*\)')
GLOBL_ADDR_RE = re.compile(r"\.globl(?:\\t|\s)+func_([0-9A-Fa-f]{8})\b")
FUNC_DEF_ADDR_RE = re.compile(r"\bfunc_([0-9A-Fa-f]{8})\s*\(")

GROUP_SLUGS = {
    "Boot & Front-End": "boot-frontend",
    "Memory & Resources": "memory-resources",
    "Video, MDEC & Movies": "video-mdec-movies",
    "GTE Pipeline": "gte",
    "Renderer & Scene": "renderer-scene",
    "GPU, 2D & HUD": "gpu-2d-hud",
    "Gameplay, AI & Physics": "gameplay-ai",
    "Platform": "platform",
    "Utilities": "utilities",
    "Other": "other",
}

_MANIFEST_CACHE: dict[tuple[str, str, int, int], dict[int, Path]] = {}


def group_of(primary: str) -> str:
    """Map a primary ``// Categories:`` path to its FUNCTION_MAP area name."""
    toks = [t.strip() for t in primary.split(">")]
    area, subs = toks[0], set(toks[1:])
    if area in {"Game Engine", "Input"}:
        return "Boot & Front-End"
    if area == "Utilities":
        return "Utilities"
    if area in {"Platform", "BIOS"}:
        return "Platform"
    if area == "Resources":
        return "Video, MDEC & Movies" if "Movie Playback" in subs else "Memory & Resources"
    if area in {"Gameplay", "AI", "Physics"}:
        return "Gameplay, AI & Physics"
    if area == "Graphics":
        if "GTE" in subs:
            return "GTE Pipeline"
        if subs & {"Model Renderer", "Renderer", "Scene Graph", "Lighting", "Effects"}:
            return "Renderer & Scene"
        return "GPU, 2D & HUD"
    return "Other"


def readable_name(text: str, retail: str) -> str | None:
    """Return the C name bound to *retail* by an asm-labelled prototype.

    Parameters may contain function pointers, so the argument list is
    balanced by hand instead of matched with a regular expression.
    """
    marker = f'__asm__("{retail}")'
    index = 0
    while True:
        index = text.find(marker, index)
        if index < 0:
            return None
        close = index
        while close > 0 and text[close - 1] in " \t\n":
            close -= 1
        if close > 0 and text[close - 1] == ")":
            depth = 0
            open_at = close - 1
            while open_at >= 0:
                if text[open_at] == ")":
                    depth += 1
                elif text[open_at] == "(":
                    depth -= 1
                    if depth == 0:
                        break
                open_at -= 1
            name = re.search(r"([A-Za-z_]\w*)\s*$", text[:open_at])
            if name:
                return name.group(1)
        index += len(marker)


def unit_group_slug(source_text: str) -> str:
    """Return the topic directory slug for *source_text*."""
    first = source_text.split("\n", 1)[0]
    match = CATEGORY_RE.match(first)
    if not match:
        return GROUP_SLUGS["Other"]
    cats = [c.strip() for c in match.group(1).split("|") if c.strip()]
    if not cats:
        return GROUP_SLUGS["Other"]
    return GROUP_SLUGS[group_of(cats[0])]


def normalize_address(unit: int | str) -> int:
    """Parse an int address, hex string, ``func_<ADDR>`` symbol, or legacy path."""
    if isinstance(unit, int):
        return unit
    text = str(unit).strip()
    if text.endswith(".c"):
        text = Path(text).stem
    match = ADDR_HEX_RE.fullmatch(text)
    if not match:
        raise ValueError(f"cannot parse unit address from {unit!r}")
    return int(match.group(1), 16)


def target_relative_path(address: int | str, source_text: str) -> str:
    """Return the repo-relative ``src/game/<group>/<name>.c`` path for a unit."""
    addr_hex = f"{normalize_address(address):08X}"
    retail = f"func_{addr_hex}"
    slug = unit_group_slug(source_text)
    alias = readable_name(source_text, retail)
    return f"src/game/{slug}/{alias or retail}.c"


def _load_manifest_entries(repo: Path, manifest_path: Path) -> dict[int, Path]:
    if not manifest_path.is_file():
        return {}
    stat = manifest_path.stat()
    key = (str(repo.resolve()), str(manifest_path.resolve()), stat.st_mtime_ns, stat.st_size)
    cached = _MANIFEST_CACHE.get(key)
    if cached is not None:
        return cached
    data = json.loads(manifest_path.read_text(encoding="utf-8"))
    mapping: dict[int, Path] = {}
    for unit in data.get("units", ()):
        src = unit.get("source")
        addr_raw = unit.get("address")
        if not src or not addr_raw:
            continue
        mapping[int(str(addr_raw), 0)] = repo / src
    _MANIFEST_CACHE[key] = mapping
    return mapping


def manifest_map(repo: Path = ROOT, manifest_path: Path | None = None) -> dict[int, Path]:
    """Return ``{addr_int: Path}`` from ``matching_units.json`` (plus candidates)."""
    repo = Path(repo)
    primary = Path(manifest_path) if manifest_path else (repo / "config" / "us" / "matching_units.json")
    mapping = dict(_load_manifest_entries(repo, primary))
    candidates = repo / "config" / "us" / "candidate_units.json"
    if manifest_path is None and candidates.is_file():
        for addr, path in _load_manifest_entries(repo, candidates).items():
            mapping.setdefault(addr, path)
    return mapping


def _infer_address_from_file(path: Path) -> int | None:
    match = STEM_ADDR_RE.fullmatch(path.stem)
    if match:
        return int(match.group(1), 16)
    text = path.read_text(errors="ignore")
    for pattern in (ASM_ALIAS_ADDR_RE, GLOBL_ADDR_RE, FUNC_DEF_ADDR_RE):
        found = pattern.search(text)
        if found:
            return int(found.group(1), 16)
    return None


def all_unit_paths(repo: Path = ROOT, manifest_path: Path | None = None) -> dict[int, Path]:
    """Return ``{addr_int: Path}`` for every ``.c`` unit under ``src/game/``."""
    repo = Path(repo)
    game_dir = repo / "src" / "game"
    mapping: dict[int, Path] = {}
    seen_paths: set[Path] = set()
    for addr, path in manifest_map(repo, manifest_path).items():
        if path.is_file():
            mapping[addr] = path
            seen_paths.add(path.resolve())
    if game_dir.is_dir():
        for path in sorted(game_dir.rglob("*.c")):
            resolved = path.resolve()
            if resolved in seen_paths:
                continue
            addr = _infer_address_from_file(path)
            if addr is not None and addr not in mapping:
                mapping[addr] = path
                seen_paths.add(resolved)
    return mapping


def resolve_unit_path(
    unit: int | str,
    repo: Path = ROOT,
    manifest_path: Path | None = None,
) -> Path | None:
    """Resolve an address or ``func_<ADDR>`` symbol to its ``src/game/`` path."""
    repo = Path(repo)
    addr = normalize_address(unit)
    from_manifest = manifest_map(repo, manifest_path).get(addr)
    if from_manifest is not None and from_manifest.is_file():
        return from_manifest
    flat = repo / "src" / "game" / f"{addr:08X}.c"
    if flat.is_file():
        return flat
    return all_unit_paths(repo, manifest_path).get(addr)


def source_address(
    path: Path | str,
    repo: Path = ROOT,
    manifest_path: Path | None = None,
) -> int | None:
    """Resolve a ``src/game/...`` file path back to its integer retail address."""
    repo = Path(repo)
    target = Path(path)
    if not target.is_absolute():
        target = repo / target
    resolved = target.resolve()
    for addr, candidate in manifest_map(repo, manifest_path).items():
        if candidate.resolve() == resolved:
            return addr
    return _infer_address_from_file(target) if target.is_file() else None


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", type=Path, default=ROOT)
    parser.add_argument("--dump-tsv", action="store_true",
                        help="Print ADDR_HEX<TAB>repo-relative-path for every unit")
    parser.add_argument("units", nargs="*")
    args = parser.parse_args(argv)

    repo = args.root.resolve()
    if args.dump_tsv:
        for addr, path in sorted(all_unit_paths(repo).items()):
            print(f"{addr:08X}\t{path.relative_to(repo).as_posix()}")
        return 0

    if not args.units:
        parser.error("provide unit addresses/symbols or --dump-tsv")
    rc = 0
    for item in args.units:
        found = resolve_unit_path(item, repo=repo)
        if found is None:
            print(f"unit_paths: cannot resolve {item}", file=sys.stderr)
            rc = 1
        else:
            print(found.as_posix())
    return rc


if __name__ == "__main__":
    sys.exit(main())
