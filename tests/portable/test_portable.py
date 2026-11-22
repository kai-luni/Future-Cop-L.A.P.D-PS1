import copy
import json
from pathlib import Path
import tempfile
import unittest

from tools import portable_tests as portable


def example():
    return json.loads((portable.ROOT / "tests/portable/a1-a2.v1.json").read_text())


class FormatTests(unittest.TestCase):
    def test_checked_in_suite_validates_against_sources(self):
        portable.validate(example(), check_sources=True)

    def test_invalid_typed_values_are_rejected(self):
        for value in ("4294967296", "01", "-0", "1e1", 1, True):
            with self.subTest(value=value):
                with self.assertRaises(portable.Invalid):
                    portable.scalar({"type": "u32", "value": value}, {}, "u32")

    def test_out_of_bounds_pointer_is_rejected(self):
        with self.assertRaises(portable.Invalid):
            portable.scalar({"type": "ptr", "value": {"region": "a", "offset": 4}},
                            {"a": 4}, "ptr")

    def test_region_spans_cannot_overlap(self):
        patch = [{"region": "a", "offset": 0, "value": {"type": "u32", "value": "0"}},
                 {"region": "a", "offset": 3, "value": {"type": "u8", "value": "0"}}]
        with self.assertRaises(portable.Invalid):
            portable.cells(patch, {"a": 4})

    def test_duplicate_json_keys_are_rejected(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "duplicate.json"
            path.write_text('{"schema":1,"schema":2}')
            with self.assertRaises(portable.Invalid):
                portable.load(path)

    def test_region_symbol_without_elf_size_uses_object_extent(self):
        from tools import portable_qemu as qemu
        linked = {'portable_region_0': (0x1000, 0, 'D')}
        spans = {'.data': (0, 0x20)}
        object_symbols = {'portable_region_0': (0, 0, 'D')}
        qemu.verify_region_storage(linked, object_symbols, spans, 'portable_region_0', 16)
        with self.assertRaises(portable.Invalid):
            qemu.verify_region_storage(linked, object_symbols, spans, 'portable_region_0', 33)

    def test_region_bounds_do_not_cross_next_symbol(self):
        from tools import portable_qemu as qemu
        linked = {'portable_region_0': (0x1000, 0, 'D')}
        symbols = {'portable_region_0': (0, 0, 'D'), 'next_region': (12, 0, 'D')}
        with self.assertRaises(portable.Invalid):
            qemu.verify_region_storage(linked, symbols, {'.data': (0, 32)}, 'portable_region_0', 16)

    def test_comparison_rejects_changed_or_missing_step(self):
        suite = example()
        expected = portable.expected(suite)
        portable.compare(suite, expected)
        for mutate in (lambda rows: rows.pop(),
                       lambda rows: rows[0]["steps"][0].update(
                           {"return": {"type": "s32", "value": "2"}})):
            wrong = copy.deepcopy(expected)
            mutate(wrong["cases"])
            with self.assertRaises(portable.Invalid):
                portable.compare(suite, wrong)


if __name__ == "__main__":
    unittest.main()
