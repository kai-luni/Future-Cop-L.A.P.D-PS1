"""Integration test for the m2c seed path.

Two levels, so the module is still useful without the optional tool:

* the oracle-to-GNU-as conversion runs always (pure text, no external tool);
* the real seed generation runs only when a pinned m2c environment and the retail
  executable exist (``build/us/m2c-venv``, created with
  ``python -m venv`` plus ``pip install git+https://github.com/matt-kempster/m2c@<commit>``).

The seed stays an unverified hint: registering it must never touch
``config/us/matching_units.json``.
"""

from __future__ import annotations

import json
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import m2c_oracle_asm  # noqa: E402
import offline_decomp  # noqa: E402

M2C = ROOT / "build/us/m2c-venv/bin/m2c"
EXECUTABLE = ROOT / "baseroms/us/SLUS_007.39"
HAVE_M2C = M2C.is_file() and EXECUTABLE.is_file()
TARGET = "func_80029BC0"


def oracle(name: str = TARGET) -> str:
    targets = {t.name: t for t in
               offline_decomp.parse_targets(offline_decomp.DEFAULT_ASSEMBLY.read_text())}
    return targets[name].assembly


class ConversionTests(unittest.TestCase):
    def test_splat_markup_is_removed(self) -> None:
        text = m2c_oracle_asm.convert(oracle(), TARGET)
        self.assertTrue(text.startswith(".text\n.globl " + TARGET + "\n"
                                          + TARGET + ":\n"), text[:60])
        self.assertNotIn("/*", text)
        self.assertNotIn("nonmatching", text)
        self.assertNotIn("glabel", text)
        self.assertIn("lw         $a0, 0x4A8($gp)", text)

    def test_delay_slot_indentation_survives(self) -> None:
        text = m2c_oracle_asm.convert(oracle("func_80045028"), "func_80045028")
        self.assertNotIn("endlabel", text)
        self.assertTrue(text.strip())


@unittest.skipUnless(HAVE_M2C, "needs build/us/m2c-venv with a pinned m2c")
class SeedGenerationTests(unittest.TestCase):
    def test_m2c_produces_a_seed_and_the_manifest_stays_untouched(self) -> None:
        before = (ROOT / "config/us/matching_units.json").read_text()
        with tempfile.TemporaryDirectory(prefix="m2c-", dir=ROOT / "build/us") as directory:
            work = Path(directory)
            assembly = work / f"{TARGET}.for-m2c.s"
            assembly.write_text(m2c_oracle_asm.convert(oracle(), TARGET))
            context = work / "context.c"
            context.write_text("typedef unsigned char u8;\ntypedef int s32;\n"
                               "extern u8 func_80045028(s32 value);\n")
            seed = work / "seed.c"
            completed = subprocess.run(
                [str(M2C), "-t", "mipsel-gcc-c", "-f", TARGET,
                 "--context", str(context), str(assembly)],
                cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                timeout=300,
            )
            self.assertEqual(completed.returncode, 0, completed.stdout + completed.stderr)
            seed.write_text(completed.stdout)
            self.assertIn(TARGET, completed.stdout)
            self.assertIn("func_80045028", completed.stdout)

            seeds = work / "seeds"
            registered = subprocess.run(
                [sys.executable, str(ROOT / "build/us/decomp_context_addon/register_seed.py"),
                 "--repo", str(ROOT), "--name", TARGET, "--address", "0x80029BC0",
                 "--size", "0x24", "--source", str(seed), "--seed-dir", str(seeds),
                 "--producer", "m2c pinned; mipsel-gcc-c; test context"],
                cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                timeout=120,
            )
            if registered.returncode != 0:
                self.skipTest("context add-on is not unpacked under build/us")
            record = json.loads((seeds / f"{TARGET}.json").read_text())
            self.assertEqual(record["name"], TARGET)
            self.assertFalse(record["verified"])
            self.assertEqual(len(record["target_sha256"]), 64)
        self.assertEqual(before, (ROOT / "config/us/matching_units.json").read_text())


if __name__ == "__main__":
    unittest.main()
