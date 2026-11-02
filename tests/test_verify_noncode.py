"""Regression tests for tools/verify_noncode.py (no copyrighted ROM needed)."""

from __future__ import annotations

import hashlib
import importlib.util
import json
import tempfile
import unittest
from pathlib import Path


SCRIPT = Path(__file__).resolve().parents[1] / "tools" / "verify_noncode.py"
spec = importlib.util.spec_from_file_location("verify_noncode", SCRIPT)
verify = importlib.util.module_from_spec(spec)
assert spec.loader is not None
spec.loader.exec_module(verify)


class VerifyNoncodeTests(unittest.TestCase):
    def setUp(self) -> None:
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.files = self.root / "files"
        self.files.mkdir()
        self.exe = b"H" * verify.HEADER_SIZE + bytes(64)
        self.ranges_path = self.root / "padding.json"
        self.assets_path = self.root / "assets.json"

    def ranges(self, *items: dict) -> Path:
        self.ranges_path.write_text(json.dumps({"ranges": list(items)}))
        return self.ranges_path

    def assets(self, *items: dict) -> Path:
        self.assets_path.write_text(json.dumps({"units": list(items)}))
        return self.assets_path

    def unit(self, name="DUMMY.DAT", data=b"\x00" * 16, **overrides) -> dict:
        item = {
            "path": name,
            "size": len(data),
            "fill": 0,
            "sha256": hashlib.sha256(data).hexdigest(),
        }
        item.update(overrides)
        return item

    def test_valid_executable_ranges(self):
        self.assertEqual(
            verify.verify_executable(self.exe, self.ranges(
                {"address": "0x80010000", "size": "0x8", "fill": 0},
                {"address": verify.LOAD_ADDRESS + 16, "size": 4, "fill": "0x00"},
            )),
            (2, 12),
        )

    def test_rejects_negative_slice_that_would_match_last_bytes(self):
        address = verify.LOAD_ADDRESS - verify.HEADER_SIZE - 4
        with self.assertRaisesRegex(ValueError, "below load address"):
            verify.verify_executable(
                self.exe, self.ranges({"address": address, "size": 4, "fill": 0})
            )

    def test_rejects_header_addresses_even_when_header_bytes_match(self):
        with self.assertRaisesRegex(ValueError, "below load address"):
            verify.verify_executable(
                bytes(len(self.exe)),
                self.ranges({"address": verify.LOAD_ADDRESS - 4, "size": 4, "fill": 0}),
            )

    def test_rejects_executable_range_beyond_end(self):
        with self.assertRaisesRegex(ValueError, "outside file"):
            verify.verify_executable(
                self.exe,
                self.ranges({"address": verify.LOAD_ADDRESS + 60, "size": 8, "fill": 0}),
            )

    def test_rejects_overlapping_executable_ranges(self):
        with self.assertRaisesRegex(ValueError, "overlapping"):
            verify.verify_executable(self.exe, self.ranges(
                {"address": verify.LOAD_ADDRESS, "size": 8, "fill": 0},
                {"address": verify.LOAD_ADDRESS + 4, "size": 8, "fill": 0},
            ))

    def test_rejects_invalid_executable_fill_and_size(self):
        for value, size in [(-1, 1), (256, 1), (0, 0), (0, -1), (True, 1)]:
            with self.subTest(value=value, size=size):
                with self.assertRaises(ValueError):
                    verify.verify_executable(
                        self.exe,
                        self.ranges({"address": verify.LOAD_ADDRESS, "size": size, "fill": value}),
                    )

    def test_valid_asset_and_sha256(self):
        data = bytes(16)
        (self.files / "DUMMY.DAT").write_bytes(data)
        self.assertEqual(
            verify.verify_assets(self.files, self.assets(self.unit())),
            (1, len(data)),
        )

    def test_valid_nested_asset(self):
        (self.files / "sub").mkdir()
        (self.files / "sub" / "DUMMY.DAT").write_bytes(bytes(16))
        self.assertEqual(
            verify.verify_assets(self.files, self.assets(self.unit("sub/DUMMY.DAT"))),
            (1, 16),
        )

    def test_rejects_duplicate_asset_entries(self):
        (self.files / "DUMMY.DAT").write_bytes(bytes(16))
        with self.assertRaisesRegex(ValueError, "duplicate"):
            verify.verify_assets(self.files, self.assets(self.unit(), self.unit()))

    def test_rejects_alias_duplicate_via_internal_symlink(self):
        (self.files / "DUMMY.DAT").write_bytes(bytes(16))
        (self.files / "alias.dat").symlink_to(self.files / "DUMMY.DAT")
        with self.assertRaisesRegex(ValueError, "duplicate"):
            verify.verify_assets(self.files, self.assets(
                self.unit("DUMMY.DAT"), self.unit("alias.dat")
            ))

    def test_rejects_path_traversal_and_absolute_paths(self):
        for path in ("../outside", "sub/../../outside", "/etc/passwd",
                     "C:\\Windows\\system.ini", "sub/../DUMMY.DAT",
                     "sub//DUMMY.DAT", "./DUMMY.DAT", "sub\\DUMMY.DAT"):
            with self.subTest(path=path):
                with self.assertRaisesRegex(ValueError, "unsafe asset path"):
                    verify.verify_assets(self.files, self.assets(self.unit(path)))

    def test_rejects_symlink_escaping_root(self):
        outside = self.root / "outside.dat"
        outside.write_bytes(bytes(16))
        (self.files / "linked.dat").symlink_to(outside)
        with self.assertRaisesRegex(ValueError, "escapes file root"):
            verify.verify_assets(self.files, self.assets(self.unit("linked.dat")))

    def test_rejects_invalid_asset_fill_and_size(self):
        (self.files / "DUMMY.DAT").write_bytes(bytes(16))
        for kwargs in ({"fill": -1}, {"fill": 256}, {"fill": True},
                       {"size": 0}, {"size": -1}, {"size": 1.5}):
            with self.subTest(kwargs=kwargs):
                with self.assertRaisesRegex(ValueError, "invalid"):
                    verify.verify_assets(self.files, self.assets(self.unit(**kwargs)))

    def test_rejects_asset_content_mismatch(self):
        (self.files / "DUMMY.DAT").write_bytes(b"\x00" * 15 + b"\x01")
        with self.assertRaisesRegex(ValueError, "fill recipe does not match"):
            verify.verify_assets(self.files, self.assets(self.unit()))

    def test_rejects_asset_digest_mismatch(self):
        (self.files / "DUMMY.DAT").write_bytes(bytes(16))
        with self.assertRaisesRegex(ValueError, "digest mismatch"):
            verify.verify_assets(self.files, self.assets(self.unit(sha256="0" * 64)))

    def test_rejects_empty_asset_path(self):
        with self.assertRaisesRegex(ValueError, "invalid asset path"):
            verify.verify_assets(self.files, self.assets(self.unit("")))


if __name__ == "__main__":
    unittest.main()
