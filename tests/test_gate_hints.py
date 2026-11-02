"""Guard against the entry mistake that cost four model rounds.

The link symbol table holds the small-data objects plus ``_gp``. Whenever a candidate
was written with the called functions in it, the same source measured short or wrong
and the C looked broken. ``preview`` prints a hint for that case; these tests pin the
detection so the hint cannot silently disappear.
"""
from __future__ import annotations

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import offline_decomp as worker  # noqa: E402


class LinkerTableHintTests(unittest.TestCase):
    def test_call_targets_in_the_link_table_are_reported(self) -> None:
        unit = {"linker_symbols": {"_gp": "0x8009F214", "D_8009F504": "0x8009F504",
                                   "func_8001D5E4": "0x8001D5E4",
                                   "func_80092BF4": "0x80092BF4"}}
        self.assertEqual(worker.linker_table_call_targets(unit),
                         ["func_8001D5E4", "func_80092BF4"])

    def test_conforming_table_reports_nothing(self) -> None:
        unit = {"linker_symbols": {"_gp": "0x8009F214", "D_8009F504": "0x8009F504"}}
        self.assertEqual(worker.linker_table_call_targets(unit), [])

    def test_missing_table_is_not_an_error(self) -> None:
        self.assertEqual(worker.linker_table_call_targets({}), [])
        self.assertEqual(worker.linker_table_call_targets({"linker_symbols": None}), [])


if __name__ == "__main__":
    unittest.main(verbosity=2)
