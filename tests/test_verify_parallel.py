"""Tests for the parallel build stage of the verifier.

The verifier compiles every unit in its own temporary directory, so the builds
are independent. ``compile_units_in_order`` runs at most *jobs* of them at once
and still yields results in manifest order, which keeps the report and the
failure list deterministic.

These tests stub ``compile_unit``; no toolchain is needed.
"""
from __future__ import annotations

import sys
import tempfile
import threading
import time
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import verify_matching_units as matching  # noqa: E402


class CompileUnitsInOrderTests(unittest.TestCase):
    def setUp(self) -> None:
        self._original = matching.compile_unit

    def tearDown(self) -> None:
        matching.compile_unit = self._original

    def test_manifest_order_and_isolated_directories(self) -> None:
        starts: list[int] = []
        seen: set[str] = set()
        lock = threading.Lock()

        def compiler(unit, config, prefix, work):
            with lock:
                self.assertNotIn(str(work), seen)
                seen.add(str(work))
                starts.append(unit)
            time.sleep(0.01 * (10 - unit))
            return bytes([unit])

        matching.compile_unit = compiler
        with tempfile.TemporaryDirectory() as directory:
            rows = list(matching.compile_units_in_order(
                list(range(10)), {}, "", directory, 4))
        self.assertEqual([row[0] for row in rows], list(range(10)))
        self.assertEqual([row[3] for row in rows], [bytes([x]) for x in range(10)])
        self.assertEqual(len(starts), 10)
        self.assertEqual(len(seen), 10)

    def test_at_most_jobs_builds_start_before_the_first_result(self) -> None:
        started: list[int] = []
        block = threading.Event()
        lock = threading.Lock()

        def compiler(unit, config, prefix, work):
            with lock:
                started.append(unit)
            block.wait(timeout=5)
            return b"pass"

        matching.compile_unit = compiler
        errors: list[BaseException] = []
        with tempfile.TemporaryDirectory() as directory:
            iterator = matching.compile_units_in_order(
                list(range(200)), {}, "", directory, 3)

            def first() -> None:
                try:
                    next(iterator)
                except BaseException as error:  # noqa: BLE001 - reported below
                    errors.append(error)

            thread = threading.Thread(target=first)
            thread.start()
            time.sleep(0.05)
            with lock:
                self.assertLessEqual(len(started), 3)
            block.set()
            thread.join(timeout=10)
            self.assertFalse(thread.is_alive())
            iterator.close()
        self.assertEqual(errors, [])

    def test_compiler_error_reaches_the_caller(self) -> None:
        def compiler(unit, config, prefix, work):
            if unit == 2:
                raise RuntimeError("compiler failed")
            return bytes([unit])

        matching.compile_unit = compiler
        with tempfile.TemporaryDirectory() as directory:
            with self.assertRaisesRegex(RuntimeError, "compiler failed"):
                list(matching.compile_units_in_order(
                    [0, 1, 2, 3, 4], {}, "", directory, 3))

    def test_build_errors_come_back_as_rows(self) -> None:
        """A compiler that cannot build a unit must not end the whole run."""
        import subprocess

        def compiler(unit, config, prefix, work):
            if unit == 1:
                raise subprocess.CalledProcessError(1, ["as"])
            return bytes([unit])

        matching.compile_unit = compiler
        with tempfile.TemporaryDirectory() as directory:
            rows = list(matching.compile_units_in_order([0, 1, 2], {}, "", directory, 2))
        self.assertEqual([row[0] for row in rows], [0, 1, 2])
        self.assertIsInstance(rows[1][3], subprocess.CalledProcessError)
        self.assertEqual(rows[0][3], b"\x00")
        self.assertEqual(rows[2][3], b"\x02")

    def test_zero_jobs_is_rejected(self) -> None:
        matching.compile_unit = lambda *args: b""
        with tempfile.TemporaryDirectory() as directory:
            with self.assertRaises(ValueError):
                list(matching.compile_units_in_order([0], {}, "", directory, 0))

    def test_serial_matches_parallel_for_the_same_units(self) -> None:
        def compiler(unit, config, prefix, work):
            return bytes([unit, unit + 1])

        matching.compile_unit = compiler
        with tempfile.TemporaryDirectory() as directory:
            serial = list(matching.compile_units_in_order(
                list(range(6)), {}, "", directory, 1))
        with tempfile.TemporaryDirectory() as directory:
            parallel = list(matching.compile_units_in_order(
                list(range(6)), {}, "", directory, 4))
        self.assertEqual([row[0] for row in serial], [row[0] for row in parallel])
        self.assertEqual([row[3] for row in serial], [row[3] for row in parallel])


if __name__ == "__main__":
    unittest.main(verbosity=2)
