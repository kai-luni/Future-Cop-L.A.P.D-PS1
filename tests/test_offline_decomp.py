import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))
SPEC = importlib.util.spec_from_file_location("offline_decomp", ROOT / "tools/offline_decomp.py")
worker = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = worker
SPEC.loader.exec_module(worker)


class OfflineDecompTests(unittest.TestCase):
    def test_parse_targets_preserves_oracle_and_boundaries(self):
        text = """nonmatching func_80010000, 0x8

glabel func_80010000
    nop
endlabel func_80010000

nonmatching func_80010008, 0xC
glabel func_80010008
    jal func_80020000
    lui $v0, %hi(D_800A0000)
endlabel func_80010008
"""
        targets = worker.parse_targets(text)
        self.assertEqual([(target.name, target.address, target.size) for target in targets], [
            ("func_80010000", 0x80010000, 8),
            ("func_80010008", 0x80010008, 12),
        ])
        self.assertNotIn("func_80010008", targets[0].assembly)
        self.assertTrue(targets[0].assembly.endswith("endlabel func_80010000"))
        self.assertEqual(worker.inferred_symbols(targets[1]), {
            "D_800A0000": "0x800A0000",
            "func_80020000": "0x80020000",
        })

    def test_score_requires_exact_extent(self):
        expected = bytes.fromhex("0102030405060708")
        self.assertTrue(worker.score_bytes(expected, expected)["exact"])
        trailing = worker.score_bytes(expected + b"\x01", expected)
        self.assertFalse(trailing["exact"])
        self.assertTrue(trailing["nonzero_bytes_after_extent"])
        partial = worker.score_bytes(bytes.fromhex("01020304"), expected)
        self.assertEqual(partial["equal_words"], 1)
        self.assertFalse(partial["exact"])

    def test_backend_response_validation(self):
        source, symbols, note, extras = worker.decode_backend_output(json.dumps({
            "source": "int func_80010000(void) { return 0; }",
            "symbols": {"D_800A0000": "0x800A0000"},
            "note": "candidate",
            "compiler": "gcc-2.7.2-cdk",
            "compiler_flags_extra": ["-G4"],
        }))
        self.assertEqual(extras["compiler"], "gcc-2.7.2-cdk")
        self.assertEqual(extras["compiler_flags_extra"], ["-G4"])
        self.assertTrue(source.endswith("\n"))
        self.assertEqual(symbols["D_800A0000"], "0x800A0000")
        self.assertEqual(note, "candidate")
        normalized, _symbols, _note, _extras = worker.decode_backend_output(json.dumps({
            "source": '#include "../../include/game/types.h"\nint f(void) { return 0; }',
        }))
        self.assertIn('#include "game/types.h"', normalized)
        with self.assertRaises(ValueError):
            worker.decode_backend_output(json.dumps({
                "source": "int f(void) { return 0; }",
                "symbols": {"bad;name": 1},
            }))
        # Inline assembly inside a C function is allowed for BIOS dispatch stubs.
        worker.decode_backend_output(json.dumps({
            "source": "void f(void) { __asm__ volatile(\"addiu $t2,$zero,0xB0\\n\\tjr $t2\\n\"); }",
        }))
        # A smuggled assembly listing is still rejected.
        with self.assertRaises(ValueError):
            worker.decode_backend_output(json.dumps({
                "source": "glabel f\n    addiu $t2,$zero,0xB0\n    jr $t2\n",
            }))
        with self.assertRaises(ValueError):
            worker.decode_backend_output(json.dumps({
                "source": "void f(void) {}\n.word 0x1234\n",
            }))
        with self.assertRaises(ValueError):
            worker.decode_backend_output(json.dumps({
                "source": "void f(void) {}",
                "compiler": "gcc-9.9-psx",
            }))

    def test_failed_compile_can_never_match_zero_bytes(self):
        score = worker.Runner.failed_score(b"\x00\x00\x00\x00")
        self.assertFalse(score["exact"])
        self.assertTrue(score["compile_failed"])

    def test_atomic_json_replaces_complete_document(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "state.json"
            worker.atomic_json(path, {"attempts": 1})
            worker.atomic_json(path, {"attempts": 2})
            self.assertEqual(json.loads(path.read_text()), {"attempts": 2})
            self.assertFalse(path.with_suffix(".json.tmp").exists())


if __name__ == "__main__":
    unittest.main()
