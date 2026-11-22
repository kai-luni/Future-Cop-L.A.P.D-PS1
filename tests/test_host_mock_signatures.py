"""Regression checks for independently verified MIPS mock call shapes.

These check C declarations for the existing local test doubles. They do not
prove that a mock reproduces the hardware or the original implementation.
"""
import re
import unittest
from pathlib import Path

HOST = Path(__file__).resolve().parent / 'host'


class MockSignatureTests(unittest.TestCase):
    def test_gte_stubs_use_the_consumed_argument_registers(self):
        text = (HOST / 'stubs.c').read_text()
        self.assertRegex(text, r'void\s+func_80010820\s*\(\s*void\s*\*\s*\w+\s*\)')
        self.assertRegex(text, r'void\s+func_8001006C\s*\(\s*void\s*\*\s*\w+\s*,\s*void\s*\*\s*\w+\s*,\s*s32\s+\w+\s*\)')

    def test_cadence_beam_mock_has_no_return_value(self):
        text = (HOST / 'cadence_harness.c').read_text()
        self.assertRegex(text, r'void\s+func_8006B23C\s*\(')
