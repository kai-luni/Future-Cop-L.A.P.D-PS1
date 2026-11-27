"""Tests for tools/fc_batch.py, the batch renamer used on named rounds.

The renamer edits source that must stay byte-exact, so its failure modes are
expensive: a rewritten ``__asm__`` literal drops the retail symbol, a
multi-declarator ``extern`` line loses a binding, or the inserted prototype
displaces the unit's struct block. Each case below reproduces one of those.
"""
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import fc_batch  # noqa: E402


class MaskTests(unittest.TestCase):
    def test_comments_and_literals_are_blanked_but_offsets_survive(self):
        text = 'int a; // func_80000001\nchar *s = "D_80000002"; /* x */ int b;\n'
        masked = fc_batch.mask(text)
        self.assertEqual(len(masked), len(text))
        self.assertEqual(masked.count("\n"), text.count("\n"))
        for symbol in ("func_80000001", "D_80000002"):
            self.assertNotIn(symbol, masked)


class RenameTests(unittest.TestCase):
    def test_identifiers_are_renamed_in_code_but_not_in_comments(self):
        text = (
            'extern void gNew(void) __asm__("func_80000001");\n'
            "/* func_80000001 runs twice */\n"
            "void gNew(void) { func_80000001(); }\n"
        )
        renamed = fc_batch.rename_identifiers(text, {"func_80000001": "gNew"})
        self.assertIn('__asm__("func_80000001")', renamed)
        # The doc blocks cite the retail names so a reader can find them in main.s.
        self.assertIn("/* func_80000001 runs twice */", renamed)
        self.assertIn("void gNew(void) { gNew(); }", renamed)

    def test_unmapped_identifiers_and_prefixes_are_untouched(self):
        text = "int func_800000010; int xfunc_80000001; int func_80000001;\n"
        renamed = fc_batch.rename_identifiers(text, {"func_80000001": "gNamed"})
        self.assertIn("int func_800000010;", renamed)
        self.assertIn("int xfunc_80000001;", renamed)
        self.assertIn("int gNamed;", renamed)


class DeclaratorTests(unittest.TestCase):
    def test_multi_declarator_statement_is_split_with_one_label_each(self):
        text = (
            'extern int D_80000001, D_80000002, D_80000003;\n'
        )
        masked = fc_batch.mask(text)
        (start, end), = fc_batch.statement_spans(masked)
        rewritten = fc_batch.rewrite_declaration(
            masked, text, start, end, {"D_80000002": "gNamed"})
        self.assertEqual(
            rewritten,
            'extern int D_80000001;\n'
            'extern int gNamed __asm__("D_80000002");\n'
            'extern int D_80000003;',
        )

    def test_existing_asm_label_is_kept_when_the_name_changes(self):
        text = 'extern unsigned char gOld[] __asm__("D_80000004");\n'
        masked = fc_batch.mask(text)
        (start, end), = fc_batch.statement_spans(masked)
        rewritten = fc_batch.rewrite_declaration(
            masked, text, start, end, {"D_80000004": "gNew"})
        self.assertEqual(rewritten, 'extern unsigned char gNew[] __asm__("D_80000004");')

    def test_untouched_statement_returns_none(self):
        text = "extern int D_80000005;\n"
        masked = fc_batch.mask(text)
        (start, end), = fc_batch.statement_spans(masked)
        self.assertIsNone(
            fc_batch.rewrite_declaration(masked, text, start, end, {"D_80000006": "gX"}))


class BindFileTests(unittest.TestCase):
    SOURCE = """\
// Categories: Platform > Test
typedef struct Node {
    int value;
} Node;

extern int D_8000000A, D_8000000B;
extern Node *gNodeList __asm__("D_8000000C");

/**
 *  @brief Copies one node.
 *  @par Callers func_8000000D.
 */
int func_8000000D(int value)
{
    return value + D_8000000A;
}
"""

    def bind(self, functions, globals_):
        directory = tempfile.mkdtemp()
        path = Path(directory) / "8000000D.c"
        path.write_text(self.SOURCE)
        changed = fc_batch.bind_file(path, functions, globals_)
        return changed, path.read_text()

    def test_prototype_lands_above_the_doc_block_and_body_stays(self):
        changed, text = self.bind(
            {"func_8000000D": "gCopyNode"}, {"D_8000000A": "gNodeCount"})
        self.assertTrue(changed)
        self.assertTrue(text.startswith("// Categories: Platform > Test\n"))
        self.assertIn(
            'int gCopyNode(int value) __asm__("func_8000000D");\n\n/**', text)
        self.assertIn("int gCopyNode(int value)\n{\n    return value + gNodeCount;", text)
        # The struct block must stay where it was: a prototype at the top of the
        # file would put it above the typedef and break the unit.
        self.assertLess(text.index("typedef struct Node"), text.index("int gCopyNode"))

    def test_every_renamed_global_keeps_its_retail_label(self):
        _, text = self.bind({}, {"D_8000000A": "gNodeCount", "D_8000000B": "gNodeLimit"})
        self.assertIn('extern int gNodeCount __asm__("D_8000000A");', text)
        self.assertIn('extern int gNodeLimit __asm__("D_8000000B");', text)
        self.assertNotIn("__asm__()", text)

    def test_file_without_a_reference_is_left_alone(self):
        changed, text = self.bind({"func_8000000E": "gOther"}, {})
        self.assertFalse(changed)
        self.assertEqual(text, self.SOURCE)


class BatchListTests(unittest.TestCase):
    """The two parallel rounds must not touch the same unit.

    A unit in both lists means two agents rewrote the same file at the same
    time; that is why the split is pinned here.
    """

    @classmethod
    def setUpClass(cls):
        cls.lists = {}
        for name in ("units_a", "units_b"):
            path = ROOT / "tools" / "batches" / f"{name}.txt"
            cls.lists[name] = path.read_text().split()

    def test_lists_are_disjoint_and_complete(self):
        units_a, units_b = self.lists["units_a"], self.lists["units_b"]
        self.assertEqual(len(units_a), 100)
        self.assertEqual(len(units_b), 100)
        self.assertEqual(set(units_a) & set(units_b), set())
        self.assertEqual(len(set(units_a) | set(units_b)), 200)

    def test_every_listed_unit_exists_in_the_manifest(self):
        import json
        manifest = json.loads((ROOT / "config/us/matching_units.json").read_text())
        sources = {unit["source"] for unit in manifest["units"]}
        for name in self.lists:
            resolved = fc_batch.load_units(ROOT / "tools" / "batches" / f"{name}.txt")
            self.assertEqual(len(resolved), len(self.lists[name]))
            for source in resolved:
                self.assertIn(source, sources)


if __name__ == "__main__":
    unittest.main()
