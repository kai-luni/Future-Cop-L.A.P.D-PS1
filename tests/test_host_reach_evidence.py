"""Regression tests for reach evidence and trace freshness."""
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from tools import host_test_reach as reach


class ReachEvidenceTests(unittest.TestCase):
    def test_mock_only_symbol_is_not_reported_as_original(self):
        with tempfile.TemporaryDirectory() as tmp:
            build = Path(tmp)
            binary = build / 'test'
            obj = build / '8001006C.o'
            obj.touch()
            def symbols(path, env):
                return {'func_8001006C'} if Path(path) == binary else {'different_symbol'}
            with patch.object(reach, 'defined_symbols', side_effect=symbols):
                self.assertFalse(reach.original_definition(binary, 'func_8001006C', {}))
            with patch.object(reach, 'defined_symbols', return_value={'func_8001006C'}):
                self.assertTrue(reach.original_definition(binary, 'func_8001006C', {}))

    def test_missing_trace_is_unknown_not_never_executed(self):
        with tempfile.TemporaryDirectory() as tmp:
            binary = Path(tmp) / 'bin'
            with patch.object(reach.hc, 'compile_unit', return_value=(Path(tmp) / 'obj', None)), \
                 patch.object(reach.hc, 'line_map', return_value={'fn': {1: {0}}}), \
                 patch.object(reach, 'symbol_address', return_value=0x1000), \
                 patch.object(reach, 'trace_addresses', return_value=(None, 'missing trace')):
                result, err = reach.measure_unit('8001006C', [binary], {}, Path(tmp))
                self.assertIsNone(result)
                self.assertIn('missing trace', err)

    def test_untraced_binary_prevents_negative_verdict(self):
        with tempfile.TemporaryDirectory() as tmp:
            binaries = [Path(tmp) / 'traced', Path(tmp) / 'missing']
            with patch.object(reach.hc, 'compile_unit', return_value=(Path(tmp) / 'obj', None)), \
                 patch.object(reach.hc, 'line_map', return_value={'fn': {1: {0}}}), \
                 patch.object(reach, 'symbol_address', return_value=0x1000), \
                 patch.object(reach, 'trace_addresses', side_effect=[({0x2000}, None), (None, 'missing trace')]):
                result, err = reach.measure_unit('8001006C', binaries, {}, Path(tmp))
                self.assertIsNone(result)
                self.assertIn('incomplete traces', err)

    def test_handwritten_extent_does_not_include_next_function(self):
        with tempfile.TemporaryDirectory() as tmp:
            binary = Path(tmp) / 'bin'
            with patch.object(reach.hc, 'compile_unit', return_value=(Path(tmp) / 'obj', None)), \
                 patch.object(reach.hc, 'line_map', return_value={}), \
                 patch.object(reach, 'symbol_extent', return_value=(0x1000, 0x10)), \
                 patch.object(reach, 'symbol_address', return_value=0x1000), \
                 patch.object(reach, 'trace_addresses', return_value=({0x1020}, None)):
                result, err = reach.measure_unit('8001006C', [binary], {}, Path(tmp))
                self.assertIsNone(err)
                self.assertEqual(result, (0, 0, False))

    def test_same_basename_in_different_builds_has_distinct_traces(self):
        import os
        import subprocess
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            fake = root / 'qemu'
            fake.write_text('#!/bin/sh\nprintf "0x1000: nop\\n" > "$4"\n')
            fake.chmod(0o755)
            first = root / 'one' / 'harness'
            second = root / 'two' / 'harness'
            for binary in (first, second):
                binary.parent.mkdir()
                binary.write_text('binary')
            env = dict(os.environ, QEMU_REAL=str(fake), QEMU_TRACE_DIR=str(root / 'traces'))
            subprocess.run(['bash', 'tests/host/qemu_trace.sh', str(first)], env=env, check=True)
            reach._trace_cache.clear()
            with patch.object(reach, 'TRACE_DIR', root / 'traces'):
                self.assertIsNotNone(reach.trace_addresses(first, {})[0])
                self.assertIsNone(reach.trace_addresses(second, {})[0])

    def test_trace_wrapper_keeps_guest_stderr(self):
        import os
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            fake = tmp / 'qemu'
            fake.write_text('#!/bin/sh\nprintf "guest-error\\n" >&2\nexit 2\n')
            fake.chmod(0o755)
            env = dict(os.environ, QEMU_REAL=str(fake), QEMU_TRACE_DIR=str(tmp))
            import subprocess
            result = subprocess.run(['bash', 'tests/host/qemu_trace.sh', 'binary'],
                                    capture_output=True, text=True, env=env)
            self.assertEqual(result.returncode, 2)
            self.assertIn('guest-error', result.stderr)

    def test_trace_wrapper_preserves_multiple_runs(self):
        import os
        import subprocess
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            fake = tmp / 'qemu'
            fake.write_text('#!/bin/sh\nprintf "trace-%s\\n" "$5" > "$4"\n')
            fake.chmod(0o755)
            env = dict(os.environ, QEMU_REAL=str(fake), QEMU_TRACE_DIR=str(tmp))
            for arg in ('first', 'second'):
                subprocess.run(['bash', 'tests/host/qemu_trace.sh', 'binary', arg],
                               env=env, check=True)
            trace = reach.trace_path('binary', tmp).read_text()
            self.assertEqual(trace.count('trace-binary'), 2)

    def test_trace_older_than_binary_is_not_used(self):
        import os
        with tempfile.TemporaryDirectory() as tmp:
            tmp = Path(tmp)
            binary = tmp / 'test'
            binary.write_text('binary')
            trace = reach.trace_path(binary, tmp)
            trace.write_text('0x1234: nop\n')
            os.utime(trace, (1, 1))
            reach._trace_cache.clear()
            with patch.object(reach, 'TRACE_DIR', tmp), patch.object(reach.hc, 'executed_addresses', side_effect=AssertionError('must not invent a test run')):
                addresses, error = reach.trace_addresses(binary, {})
                self.assertIsNone(addresses)
                self.assertIn('stale trace', error)
