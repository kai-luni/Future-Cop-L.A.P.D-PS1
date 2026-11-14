#!/usr/bin/env python3
"""Run Splat on a private input snapshot and publish validated outputs."""

import csv
import fcntl
import hashlib
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = Path(__file__).resolve().parents[1]
INPUTS = ("config", "src", "baseroms", "include")


def hashes(root):
    return {
        p.relative_to(root): hashlib.sha256(p.read_bytes()).digest()
        for name in INPUTS
        for p in (root / name).rglob("*")
        if p.is_file()
    }


def generate(stage):
    from window_split import split

    listing = stage / "asm/us/main.s"
    rows_path = stage / "config/us/splat_windows.tsv"
    with rows_path.open(newline="") as f:
        rows = list(csv.DictReader(f, delimiter="\t"))
    include = "\n".join(p.read_text() for p in (stage / "include").rglob("*.inc"))
    link = "\n".join(
        (stage / "build/us" / name).read_text()
        for name in ("undefined_syms_auto.txt", "undefined_funcs_auto.txt")
    )
    result, report = split(
        listing.read_text(),
        rows,
        (stage / "baseroms/us/SLUS_007.39").read_bytes(),
        include,
        link,
    )
    if (
        report["missing"]
        or report["wrong_size_or_address"]
        or report["extra"]
        or report["blocked_groups"]
        or report["inventory"]["error"]
    ):
        raise ValueError(
            "window split incomplete: "
            + str(
                {
                    k: report[k]
                    for k in (
                        "missing",
                        "wrong_size_or_address",
                        "extra",
                        "blocked_groups",
                        "inventory",
                    )
                }
            )
        )
    definitions = {}
    for name, value in re.findall(
        r"(?m)^\s*([\w.$]+)\s*=\s*(0x[0-9a-fA-F]+)\s*;", link
    ):
        definitions.setdefault(name, []).append(int(value, 0))
    addresses = {r["name"]: int(r["address"], 0) for r in rows}
    for alias in report["preserved_cut_aliases"]:
        if any(
            value != addresses[alias["new_owner"]]
            for value in definitions.get(alias["alias"], [])
        ):
            raise ValueError("linker changes alias address: " + alias["alias"])
    listing.write_text(result)
    print(
        f"Validated {report['exact_count']}/{report['wanted_count']} windows; "
        f"preserved {report['unchanged_annotated_lines']} instruction lines and "
        f"{report['preserved_original_label_lines']} label lines",
        flush=True,
    )
    for warning in report["alias_metadata_warnings"]:
        print("Warning: .aent ownership changes for", warning["alias"], flush=True)


def main():
    python = os.path.abspath(sys.argv[1]) if len(sys.argv) == 2 else sys.executable
    (ROOT / "build").mkdir(exist_ok=True)
    with (ROOT / "build/.split-staged.lock").open("a") as lock:
        fcntl.flock(lock, fcntl.LOCK_EX)
        with tempfile.TemporaryDirectory(prefix=".split-stage-", dir=ROOT) as tmp:
            stage = Path(tmp)
            for name in INPUTS:
                shutil.copytree(ROOT / name, stage / name, symlinks=False)
            before = hashes(stage)
            subprocess.run(
                [python, "-m", "splat", "split", "config/us/splat.yaml"],
                cwd=stage,
                check=True,
            )
            generate(stage)
            if hashes(ROOT) != before:
                raise ValueError("split inputs changed during generation")
            outputs = [
                p
                for p in stage.rglob("*")
                if p.is_file()
                and (
                    p.relative_to(stage) not in before
                    or hashlib.sha256(p.read_bytes()).digest()
                    != before[p.relative_to(stage)]
                )
            ]
            listing = stage / "asm/us/main.s"
            if listing not in outputs:
                raise ValueError("Splat did not create a listing")
            outputs.sort(key=lambda p: (p == listing, str(p)))
            prepared = []
            try:
                for source in outputs:
                    dest = ROOT / source.relative_to(stage)
                    dest.parent.mkdir(parents=True, exist_ok=True)
                    fd, name = tempfile.mkstemp(prefix=".split-", dir=dest.parent)
                    os.close(fd)
                    prepared.append((Path(name), dest))
                    shutil.copyfile(source, name)
                    shutil.copymode(source, name)
                for source, dest in prepared:
                    os.replace(source, dest)
            finally:
                for source, _ in prepared:
                    source.unlink(missing_ok=True)
            print("Published Splat outputs; main.s published last", flush=True)


if __name__ == "__main__":
    try:
        main()
    except (OSError, ValueError, subprocess.CalledProcessError) as exc:
        print(f"split failed: {exc}", file=sys.stderr)
        sys.exit(1)
