"""Link-map evidence must distinguish a real object from a same-named mock."""
import unittest
from pathlib import Path
from unittest.mock import patch

from tools import host_leaf_provenance as provenance
from tools.host_leaf_provenance import map_text_ranges, verify_binding


class LeafProvenanceTests(unittest.TestCase):
    def test_matching_symbol_binds_to_target_object(self):
        text = ''' .text          0x00401000       0x40 /tmp/build/80010000.o
                0x00401000                func_80010000
 .text          0x00401040       0x20 /tmp/build/harness.o
'''
        ranges = map_text_ranges(text)
        verify_binding(ranges, Path('/tmp/build/80010000.o'), 0x401000, 0x10)

    def test_mock_symbol_must_not_inherit_original_object_evidence(self):
        text = ''' .text          0x00401000       0x40 /tmp/build/80010000.o
 .text          0x00401040       0x20 /tmp/build/harness.o
                0x00401040                func_80010000
'''
        with self.assertRaises(ValueError):
            verify_binding(map_text_ranges(text), Path('/tmp/build/80010000.o'), 0x401040, 0x10)

    def test_missing_ambiguous_or_cross_boundary_ranges_fail(self):
        target = Path('/tmp/build/80010000.o')
        with self.assertRaises(ValueError):
            verify_binding([], target, 0x401000, 4)
        with self.assertRaises(ValueError):
            verify_binding([(0x401000, 0x20, target)], target, 0x40101c, 8)
        with self.assertRaises(ValueError):
            verify_binding([(0x401000, 0x20, target), (0x401000, 0x20, target)], target, 0x401000, 4)

    def test_only_exec_dispatches_count(self):
        data = ('0x00401000: instruction\n'
                'Trace 0: 0x7fff [00000000/00402000/00000000/00000000] linked\n')
        self.assertEqual(provenance.executed_entries(data), {0x402000})

    def test_multiline_map_section_is_supported(self):
        text = ''' .text
                0x00401000       0x20 /tmp/build/80010000.o
'''
        self.assertEqual(map_text_ranges(text), [(0x401000, 0x20, Path('/tmp/build/80010000.o'))])

    # A source script may override OUT or DECOMP; do not certify it from a
    # stale adjacent object or a foreign source tree.
    def test_suite_forces_repo_source_and_fresh_output(self):
        import os
        with patch.dict(os.environ, {'DECOMP': '/not/the/repo', 'OUT': '/stale/output'}):
            with patch.object(provenance.subprocess, 'run') as run:
                run.side_effect = RuntimeError('stop before external invocation')
                with self.assertRaisesRegex(RuntimeError, 'stop before external invocation'):
                    provenance.verify_suite('leaf_a1')
                env = run.call_args.kwargs['env']
                self.assertEqual(env['DECOMP'], str(provenance.leaf.REPO))
                self.assertNotEqual(env['OUT'], '/stale/output')


if __name__ == '__main__':
    unittest.main()
