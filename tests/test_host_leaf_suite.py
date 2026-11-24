"""Checks for the host leaf-suite runner."""
import subprocess
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch

from tools import host_leaf_suite


class LeafSuiteTests(unittest.TestCase):
    def test_parses_per_function_checks_and_failures(self):
        output = ('LEAF func_80010000 checks=2 failures=0\n'
                  'LEAF func_80010000 checks=1 failures=1\n')
        self.assertEqual(host_leaf_suite.parse(output)['func_80010000'],
                         {'checks': 3, 'failures': 1})

    def test_timeout_kills_only_its_own_process_group(self):
        with tempfile.TemporaryDirectory() as tmp:
            host = Path(tmp)
            (host / 'leaf_test.sh').write_text('#!/bin/bash\nsleep 30\n')
            other = subprocess.Popen(['sleep', '30'])
            try:
                with patch.object(host_leaf_suite.subprocess, 'run', side_effect=AssertionError('must not use pkill')):
                    rc, _, err = host_leaf_suite.run_suite('leaf', out=host / 'out', host=host, timeout=0.2)
                self.assertEqual((rc, err), (124, 'timeout'))
                self.assertIsNone(other.poll())
            finally:
                other.terminate()
                other.wait()

    def test_gen_script_links_opt_in_shared_mocks_only(self):
        """`/* @mock X_mock */` adds mocks/X_mock.o; a harness without it is unchanged."""
        from types import SimpleNamespace
        with tempfile.TemporaryDirectory() as tmp:
            host = Path(tmp)
            (host / 'mocks').mkdir()
            (host / 'mocks' / 'sound_mock.c').write_text('')
            (host / 'plain_harness.c').write_text('/* @leaf 80010000 source x */\n')
            (host / 'opted_harness.c').write_text(
                '/* @mock sound_mock */\n/* @leaf 80010000 source x */\n')
            with patch.object(host_leaf_suite, 'HOST', host), \
                    patch.object(host_leaf_suite, 'REPO', host):
                host_leaf_suite.gen_script(SimpleNamespace(suite='plain'))
                host_leaf_suite.gen_script(SimpleNamespace(suite='opted'))
            plain = (host / 'plain_test.sh').read_text()
            opted = (host / 'opted_test.sh').read_text()
        self.assertNotIn('sound_mock', plain)
        self.assertIn('"$OUT/bios_mock.o" "${objects[@]}"', plain)
        self.assertIn('compile "$HERE/mocks/sound_mock.c" "$OUT/sound_mock.o"', opted)
        self.assertIn('"$OUT/bios_mock.o" "$OUT/sound_mock.o" "${objects[@]}"', opted)
