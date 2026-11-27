import pathlib
import re
import sys
import unittest

ROOT = pathlib.Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import build_function_map


class FunctionMapTests(unittest.TestCase):
    def test_readable_name_parses_standard_and_fnptr_prototypes(self):
        sample = """
        extern void gOther(void) __asm__("func_80000001");
        s32 gExampleHandler(
            void (*callback)(s32 code, void *ctx),
            u32 flags
        ) __asm__("func_80000002");
        """
        self.assertEqual(build_function_map.readable_name(sample, "func_80000001"), "gOther")
        self.assertEqual(
            build_function_map.readable_name(sample, "func_80000002"),
            "gExampleHandler",
        )
        self.assertIsNone(build_function_map.readable_name(sample, "func_80000003"))
        self.assertIsNone(
            build_function_map.readable_name('gNotAProto __asm__("func_80000004");', "func_80000004")
        )

    def test_group_of_classification(self):
        cases = {
            "Game Engine > Menu": "Boot & Front-End",
            "Input > Controller": "Boot & Front-End",
            "Utilities > Math": "Utilities",
            "Platform > Sound": "Platform",
            "BIOS > Memory": "Platform",
            "Resources > Movie Playback": "Video, MDEC & Movies",
            "Resources > Save Data": "Memory & Resources",
            "Gameplay > Progression": "Gameplay, AI & Physics",
            "Graphics > GTE > Matrix": "GTE Pipeline",
            "Graphics > Model Renderer": "Renderer & Scene",
            "Graphics > HUD": "GPU, 2D & HUD",
            "Unclassified > Thing": "Other",
        }
        for category, expected_group in cases.items():
            with self.subTest(category=category):
                self.assertEqual(build_function_map.group_of(category), expected_group)

    def test_renamed_units_have_unique_c_names_and_retail_asm_labels(self):
        man, cats, cnames, edges = build_function_map.load()
        self.assertGreaterEqual(len(cnames), 50)
        self.assertTrue(len(man) >= len(cats) > 0)
        self.assertTrue(len(edges) > 0)

        ident_re = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
        seen = {}
        for retail, cname in cnames.items():
            self.assertRegex(cname, ident_re)
            self.assertFalse(cname.startswith("func_"), f"{retail} kept func_ prefix: {cname}")
            self.assertNotIn(
                cname,
                seen,
                f"duplicate readable name {cname} for {seen.get(cname)} and {retail}",
            )
            seen[cname] = retail

            unit_path = ROOT / man[retail]["source"]
            text = unit_path.read_text(errors="ignore")
            self.assertIn(f'__asm__("{retail}")', text)

    def test_function_map_matches_rendered_output(self):
        self.assertTrue(build_function_map.OUT.exists())
        self.assertEqual(build_function_map.OUT.read_text(), build_function_map.render())


if __name__ == "__main__":
    unittest.main()
