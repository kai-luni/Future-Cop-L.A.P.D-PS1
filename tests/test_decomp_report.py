import json
import tempfile
import unittest
from pathlib import Path

from tools import generate_decomp_report
from tools.generate_decomp_report import build_report


def expected_function_count() -> int:
    """Catalogue minus the merges, computed with the report's own canonicalisation."""
    return len(generate_decomp_report.canonical_functions())


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
        # A new verified unit changes the manifest, so these numbers move with it;
        # what this test pins is the shape of the aggregate, not a snapshot.
        self.assertEqual(measures["total_code"], "555552")
        # The catalogue minus the merges recorded in the boundary overrides: the
        # count follows the data instead of pinning a snapshot that every new
        # override would break.
        self.assertEqual(measures["total_functions"], expected_function_count())
        self.assertGreater(int(measures["matched_code"]), 51512)
        self.assertGreaterEqual(measures["matched_functions"], 522)
        self.assertEqual(
            sum(unit["metadata"]["complete"] for unit in report["units"]),
            measures["matched_functions"],
        )
        categories = {item["id"]: item["measures"] for item in report["categories"]}
        self.assertEqual(categories["executable_data"]["total_data"], "32224")
        self.assertEqual(categories["executable_data"]["matched_data"], "404")
        self.assertEqual(categories["disc_assets"]["total_data"], "305247544")
        self.assertEqual(categories["disc_assets"]["matched_data"], "27648000")

    def test_code_only_scope_drops_disc_assets_everywhere(self):
        report = build_report_with_assets(False)
        measures = report["measures"]
        self.assertEqual(measures["total_code"], "555552")
        self.assertGreater(int(measures["matched_code"]), 51512)
        self.assertEqual(measures["total_data"], "32224")
        self.assertEqual(measures["matched_data"], "404")
        categories = {item["id"]: item["measures"] for item in report["categories"]}
        self.assertNotIn("disc_assets", categories)
        self.assertEqual(categories["executable_data"]["total_data"], "32224")

    def test_units_the_verifier_did_not_confirm_are_not_counted(self):
        """A manifest entry is not a verified unit: the report cross-checks the gate."""
        records = generate_decomp_report.verification_records()
        if records is None:
            self.skipTest("no verification report in this working tree")
        units = generate_decomp_report.matching_units()
        dropped = next(iter(units))
        payload = {"units": [row for address, row in records.items() if address != dropped]}
        handle = tempfile.NamedTemporaryFile("w", suffix=".json", delete=False)
        handle.write(json.dumps(payload))
        handle.close()
        path = Path(handle.name)
        try:
            full = build_report_with_assets(False)
            reduced = generate_decomp_report.build_report(verification=path)
        finally:
            path.unlink()
        self.assertEqual(reduced["unverified_units"], [units[dropped]["name"]])
        self.assertEqual(
            int(reduced["measures"]["matched_functions"]),
            int(full["measures"]["matched_functions"]) - 1,
        )
        self.assertEqual(
            sum(unit["metadata"]["complete"] for unit in reduced["units"]),
            reduced["measures"]["matched_functions"],
        )

    def test_missing_verification_report_keeps_the_manifest_view(self):
        """A fresh checkout has no gate output; the report must still build."""
        empty = Path(tempfile.mkdtemp()) / "does-not-exist.json"
        report = generate_decomp_report.build_report(verification=empty)
        self.assertEqual(report["unverified_units"], [])
        self.assertEqual(
            int(report["measures"]["matched_functions"]),
            len(generate_decomp_report.matching_units()),
        )

    def test_default_scope_is_boolean(self):
        self.assertIsInstance(generate_decomp_report.assets_measured(), bool)


if __name__ == "__main__":
    unittest.main()
