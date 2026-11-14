"""Regression tests for configured Splat windows."""

import sys
from pathlib import Path
import unittest

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
import window_split


class WindowTests(unittest.TestCase):
    def fixture(self, alias="dlabel D_80010008", branch=False):
        words = [
            "00000000",
            "00000008" if branch else "00000000",
            "00000000",
            "00000000",
        ]
        lines = ["nonmatching func_80010000, 0x10", "glabel func_80010000"]
        for index, word in enumerate(words):
            if index == 2 and alias:
                lines.append(alias)
            lines.append(
                f"/* {0x800+index*4:06X} {0x80010000+index*4:08X} {word} */ nop"
            )
        lines.append("endlabel func_80010000")
        text = "\n".join(lines) + "\n"
        rom = bytearray(0x810)
        for i, word in enumerate(words):
            rom[0x800 + i * 4 : 0x804 + i * 4] = bytes.fromhex(word)
        rows = [
            {"name": "func_80010000", "address": "0x80010000", "size": "0x8"},
            {"name": "func_80010008", "address": "0x80010008", "size": "0x8"},
        ]
        return text, rows, bytes(rom)

    def test_unique_alias_is_preserved(self):
        text, rows, rom = self.fixture()
        output, report = window_split.split(text, rows, rom, "")
        self.assertFalse(report["blocked_groups"])
        self.assertEqual(output.count("dlabel D_80010008"), 1)
        self.assertEqual(report["exact_count"], 2)

    def test_duplicate_alias_blocks_split(self):
        text, rows, rom = self.fixture()
        _, report = window_split.split(text + "dlabel D_80010008\n", rows, rom, "")
        self.assertIn(
            "ambiguous or unsupported D_ alias", str(report["blocked_groups"])
        )

    def test_invalid_bounds_rejected(self):
        text, rows, rom = self.fixture()
        for size in ("0", "3", "0xc", "0x100"):
            bad = [dict(r) for r in rows]
            bad[0]["size"] = size
            with self.subTest(size=size), self.assertRaises(ValueError):
                window_split.split(text, bad, rom, "")

    def test_delay_slot_blocks_split(self):
        text, rows, rom = self.fixture(branch=True)
        _, report = window_split.split(text, rows, rom, "")
        self.assertIn("cut in branch delay slot", str(report["blocked_groups"]))

    def test_retail_mismatch_blocks_split(self):
        text, rows, rom = self.fixture()
        rom = bytearray(rom)
        rom[0x800] ^= 1
        _, report = window_split.split(text, rows, bytes(rom), "")
        self.assertIn("retail annotation mismatch", str(report["blocked_groups"]))

    def test_unchanged_owner_retail_mismatch_also_rejected(self):
        text, _, rom = self.fixture(alias="")
        rows = [{"name": "func_80010000", "address": "0x80010000", "size": "0x10"}]
        rom = bytearray(rom)
        rom[0x800] ^= 1
        with self.assertRaisesRegex(ValueError, "retail"):
            window_split.split(text, rows, bytes(rom), "")
