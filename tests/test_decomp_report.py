import unittest

from tools import generate_decomp_report
from tools.generate_decomp_report import build_report


def build_report_with_assets(measured: bool) -> dict:
    """Build the report as if ``config/project.json`` declared the given scope."""
    original = generate_decomp_report.assets_measured
    generate_decomp_report.assets_measured = lambda: measured
    try:
        return build_report()
    finally:
        generate_decomp_report.assets_measured = original


class DecompReportTests(unittest.TestCase):
    def test_report_tracks_only_manifested_exact_matches(self):
        report = build_report_with_assets(True)
        measures = report["measures"]
        self.assertEqual(report["version"], 2)
        self.assertEqual(measures["matched_code"], "51512")
        self.assertEqual(measures["total_code"], "555552")
        self.assertEqual(measures["matched_functions"], 522)
        self.assertEqual(measures["total_functions"], 1855)
        self.assertEqual(measures["total_data"], "305279768")
        self.assertEqual(measures["matched_data"], "27648404")
        self.assertEqual(
            sum(unit["metadata"]["complete"] for unit in report["units"]), 522
        )
        categories = {item["id"]: item["measures"] for item in report["categories"]}
        self.assertEqual(categories["executable_data"]["total_data"], "32224")
        self.assertEqual(categories["executable_data"]["matched_data"], "404")
        self.assertEqual(categories["disc_assets"]["total_data"], "305247544")
        self.assertEqual(categories["disc_assets"]["matched_data"], "27648000")

    def test_code_only_scope_drops_disc_assets_everywhere(self):
        report = build_report_with_assets(False)
        measures = report["measures"]
        self.assertEqual(measures["matched_code"], "51512")
        self.assertEqual(measures["total_code"], "555552")
        self.assertEqual(measures["matched_functions"], 522)
        self.assertEqual(measures["total_data"], "32224")
        self.assertEqual(measures["matched_data"], "404")
        categories = {item["id"]: item["measures"] for item in report["categories"]}
        self.assertNotIn("disc_assets", categories)
        self.assertEqual(categories["executable_data"]["total_data"], "32224")

    def test_default_scope_is_boolean(self):
        self.assertIsInstance(generate_decomp_report.assets_measured(), bool)


if __name__ == "__main__":
    unittest.main()
