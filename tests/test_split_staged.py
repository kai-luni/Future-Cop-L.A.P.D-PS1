"""Failure-path tests for the split command's publication boundary."""

import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


class StagedSplitTests(unittest.TestCase):
    def run_split(self, fail_splat):
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            shutil.copy(ROOT / "Makefile", root / "Makefile")
            (root / "asm/us").mkdir(parents=True)
            listing = root / "asm/us/main.s"
            listing.write_bytes(b"OLD LISTING\n")
            shutil.copytree(
                ROOT / "tools",
                root / "tools",
                ignore=shutil.ignore_patterns("__pycache__"),
            )
            for name in ("config/us", "src", "baseroms/us", "include"):
                (root / name).mkdir(parents=True)
            (root / "config/us/splat_windows.tsv").write_text(
                "name\taddress\tsize\nfunc_80010000\t0x80010000\t0\n"
            )
            (root / "baseroms/us/SLUS_007.39").write_bytes(bytes(0x810))
            marker = root / "splat-ran"
            fake = root / "fake_python"
            fake.write_text("""#!/bin/sh
mkdir -p asm/us build/us
printf 'nonmatching func_80010000, 0x4\\nglabel func_80010000\\n/* 000800 80010000 00000000 */ nop\\nendlabel func_80010000\\n' > asm/us/main.s
: > build/us/undefined_syms_auto.txt
: > build/us/undefined_funcs_auto.txt
touch "$MARKER"
exit """ + ("9" if fail_splat else "0") + "\n")
            fake.chmod(0o755)
            result = subprocess.run(
                ["make", "-o", "setup", "-o", "verify", "split", f"VENV_PYTHON={fake}"],
                cwd=root,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                env={**os.environ, "MARKER": str(marker)},
            )
            self.assertTrue(marker.exists(), result.stderr.decode())
            self.assertNotEqual(result.returncode, 0)
            self.assertEqual(listing.read_bytes(), b"OLD LISTING\n")
            if not fail_splat:
                self.assertIn(b"invalid size/alignment", result.stderr)

    def test_failed_splat_preserves_existing_listing(self):
        self.run_split(True)

    def test_failed_window_validation_preserves_existing_listing(self):
        self.run_split(False)
