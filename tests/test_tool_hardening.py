"""Regression tests for the tool-hardening work order (points 1, 2 and 5).

Point 1: one strict comparison everywhere, no self-padding of short output, and a
run state that cannot present a previous success as the current result.
Point 2: promotion only after re-verification, bound to source, headers, profile
and reference range, with the verified argument list kept verbatim.
Point 5 (the two small fixes): an emptied ``maspsx_flags_extra`` is written back,
and each matrix attempt keeps its own evidence directory.

Byte-level cases need the pinned toolchain and ``baseroms/us/SLUS_007.39`` (both
gitignored); without them the promotion tests skip instead of failing.
"""

from __future__ import annotations

import argparse
import contextlib
import io
import json
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "tools"))

import offline_decomp as worker  # noqa: E402
import verify_matching_units as matching  # noqa: E402

EXECUTABLE = ROOT / "baseroms/us/SLUS_007.39"
ASSEMBLY = ROOT / "asm/us/main.s"
TOOLCHAIN = ROOT / ".toolchain/gcc-2.8.1-psx/cc1"
HAVE_INPUTS = EXECUTABLE.is_file() and ASSEMBLY.is_file() and TOOLCHAIN.is_file()

# An accepted unit whose source and oracle range the promotion tests reuse.
UNIT = "func_8002A58C"
ADDRESS = 0x8002A58C
SIZE = 0xC
SETTINGS = {
    "compiler": "gcc-2.8.1-psx",
    "compiler_flags": ["-quiet", "-msoft-float", "-mgas", "-fgnu-linker", "-O2", "-G4"],
    "small_data_symbols": {"D_8009F6DC": 4},
    "link": True,
    "linker_symbols": {"D_8009F6DC": "0x8009F6DC", "_gp": "0x8009F214"},
}


class ComparisonTests(unittest.TestCase):
    """The single comparison of point 1, without any toolchain."""

    def test_short_output_is_never_padded(self) -> None:
        result = matching.compare_bytes(bytes.fromhex("0800E003"),
                                        bytes.fromhex("0800E00300000000"))
        self.assertFalse(result["exact"])
        self.assertEqual(result["missing_bytes"], 4)
        self.assertEqual(result["emitted_bytes"], 4)
        self.assertEqual(result["padding_bytes"], 0)

    def test_exact_output_is_accepted(self) -> None:
        result = matching.compare_bytes(bytes.fromhex("0800E00300000000"),
                                        bytes.fromhex("0800E00300000000"))
        self.assertTrue(result["exact"])
        self.assertEqual(result["missing_bytes"], 0)
        self.assertEqual(result["padding_bytes"], 0)

    def test_zero_padding_behind_the_range_is_counted(self) -> None:
        result = matching.compare_bytes(
            bytes.fromhex("0800E00300000000") + bytes(8),
            bytes.fromhex("0800E00300000000"))
        self.assertTrue(result["exact"])
        self.assertEqual(result["padding_bytes"], 8)
        self.assertFalse(result["nonzero_bytes_after_extent"])

    def test_nonzero_bytes_behind_the_range_are_rejected(self) -> None:
        result = matching.compare_bytes(
            bytes.fromhex("0800E00300000000") + bytes(3) + b"\x01",
            bytes.fromhex("0800E00300000000"))
        self.assertFalse(result["exact"])
        self.assertTrue(result["nonzero_bytes_after_extent"])

    def test_worker_scores_through_the_same_comparison(self) -> None:
        actual = bytes.fromhex("0800E003")
        expected = bytes.fromhex("0800E00300000000")
        self.assertEqual(worker.score_bytes(actual, expected)["exact"],
                         matching.compare_bytes(actual, expected)["exact"])
        self.assertFalse(worker.score_bytes(actual, expected)["exact"])

    def test_target_range_rejects_invalid_spans(self) -> None:
        image = bytes(matching.HEADER_SIZE + 0x100)
        with self.assertRaises(ValueError):
            matching.target_range(image, matching.LOAD_ADDRESS, 0)
        with self.assertRaises(ValueError):
            matching.target_range(image, matching.LOAD_ADDRESS, 6)
        with self.assertRaises(ValueError):
            matching.target_range(image, matching.LOAD_ADDRESS + 2, 8)
        with self.assertRaises(ValueError):
            matching.target_range(image, matching.LOAD_ADDRESS + 0x100, 8)
        offset, expected = matching.target_range(image, matching.LOAD_ADDRESS, 8)
        self.assertEqual(offset, matching.HEADER_SIZE)
        self.assertEqual(len(expected), 8)


class VerifierStatusTests(unittest.TestCase):
    """Point 1: strict verdicts plus a run state that is published atomically."""

    def setUp(self) -> None:
        self.directory = Path(tempfile.mkdtemp(prefix="hardening-verify-"))
        self.addCleanup(shutil.rmtree, self.directory, True)
        image = bytearray(matching.HEADER_SIZE + 0x200)
        image[matching.HEADER_SIZE:matching.HEADER_SIZE + 8] = bytes.fromhex(
            "0800E00300000000")
        self.executable = self.directory / "image.bin"
        self.executable.write_bytes(bytes(image))
        self.config = self.directory / "units.json"
        self.config.write_text(json.dumps({
            "compiler": "gcc-2.7.2-cdk",
            "compiler_flags": [],
            "units": [{"name": "probe", "address": hex(matching.LOAD_ADDRESS),
                       "size": 8, "source": "src/game/80010704.c", "symbols": {}}],
        }))
        self.report = self.directory / "matching_report.json"
        self.original_compiler = matching.compile_unit
        self.addCleanup(setattr, matching, "compile_unit", self.original_compiler)

    def test_short_output_fails_and_marks_the_run_failed(self) -> None:
        matching.compile_unit = lambda *arguments, **keywords: bytes.fromhex("0800E003")
        with self.assertRaises(SystemExit) as raised:
            matching.verify(self.config, self.executable, self.report,
                            "mipsel-linux-gnu-")
        self.assertIn("emitted 4 of 8 bytes", str(raised.exception))
        status = json.loads(matching.status_path_for(self.report).read_text())
        self.assertEqual(status["status"], "failed")
        self.assertFalse(self.report.is_file())

    def test_exact_output_is_published_with_padding(self) -> None:
        matching.compile_unit = (
            lambda *arguments, **keywords: bytes.fromhex("0800E00300000000") + bytes(4))
        with contextlib.redirect_stdout(io.StringIO()) as printed:
            report = matching.verify(self.config, self.executable, self.report,
                                     "mipsel-linux-gnu-")
        self.assertEqual(report["matched_functions"], 1)
        self.assertEqual(report["units"][0]["padding_bytes"], 4)
        self.assertIn("4 bytes zero padding", printed.getvalue())
        self.assertEqual(json.loads(matching.status_path_for(self.report).read_text())
                         ["status"], "ok")

    def test_failed_run_keeps_the_old_success_only_as_history(self) -> None:
        old = {"schema": 1, "matched_functions": 123, "matched_bytes": 456,
               "units": [], "failures": []}
        self.report.write_text(json.dumps(old))
        matching.compile_unit = lambda *arguments, **keywords: bytes.fromhex("0800E003")
        with self.assertRaises(SystemExit):
            matching.verify(self.config, self.executable, self.report,
                            "mipsel-linux-gnu-")
        self.assertFalse(self.report.is_file())
        archived = json.loads(
            self.report.with_name(self.report.stem + ".previous.json").read_text())
        self.assertEqual(archived["matched_functions"], 123)
        self.assertEqual(json.loads(matching.status_path_for(self.report).read_text())
                         ["status"], "failed")


class StageThresholdTests(unittest.TestCase):
    """The -G separation between compiler, maspsx and GNU as.

    A real profile pins the compiler to -G8 and the assembler stage to -G4; the
    compiler value must not be appended behind that and GNU as has to receive the
    pinned value instead of falling back to its default of 8.
    """

    def test_explicit_maspsx_threshold_wins(self) -> None:
        unit = {"maspsx_flags_extra": ["-G4", "--dont-force-G0"]}
        config = {"maspsx_flags": ["--dont-expand-li"]}
        flags = matching.maspsx_stage_flags(unit, config,
                                            ["-quiet", "-O2", "-G8"])
        self.assertEqual(flags, ["--dont-expand-li", "-G4", "--dont-force-G0"])

    def test_compiler_threshold_carries_over_when_no_stage_threshold_is_given(self) -> None:
        unit = {"maspsx_flags_extra": ["--expand-div"]}
        config = {"maspsx_flags": ["--dont-expand-li"]}
        flags = matching.maspsx_stage_flags(unit, config, ["-O2", "-G4"])
        self.assertEqual(flags, ["--dont-expand-li", "--expand-div", "-G4"])

    def test_assembler_gets_the_pinned_threshold(self) -> None:
        self.assertIn("-G4", matching.assembler_stage_flags(["-G4", "--aspsx-version=2.56"]))
        self.assertIn("-G0", matching.assembler_stage_flags(["-G0"]))
        # Without a pinned threshold the assembler keeps its own default.
        self.assertFalse([flag for flag in matching.assembler_stage_flags(["--expand-div"])
                          if flag.startswith("-G")])


class HeaderFingerprintTests(unittest.TestCase):
    """Point 2: a changed header has to invalidate an existing acceptance."""

    def test_headers_of_an_accepted_source_are_recorded(self) -> None:
        headers = worker.header_fingerprint(ROOT / "src/game/8002A58C.c")
        self.assertIn("include/game/types.h", headers)
        self.assertRegex(headers["include/game/types.h"], r"^[0-9a-f]{64}$")

    def test_changing_an_included_header_changes_the_fingerprint(self) -> None:
        directory = Path(tempfile.mkdtemp(prefix="hardening-headers-"))
        self.addCleanup(shutil.rmtree, directory, True)
        header = directory / "local.h"
        header.write_text("#define VALUE 1\n")
        source = directory / "unit.c"
        source.write_text('#include "local.h"\nint f(void) { return VALUE; }\n')
        before = worker.header_fingerprint(source)
        self.assertEqual(len(before), 1)
        self.assertTrue(next(iter(before)).endswith("local.h"))
        header.write_text("#define VALUE 2\n")
        self.assertNotEqual(worker.header_fingerprint(source), before)


@unittest.skipUnless(HAVE_INPUTS, "needs the pinned toolchain and the retail executable")
class PromotionTests(unittest.TestCase):
    """Point 2: preview records the review, promotion re-verifies and publishes."""

    def setUp(self) -> None:
        self.directory = Path(tempfile.mkdtemp(prefix="hardening-promote-"))
        self.addCleanup(shutil.rmtree, self.directory, True)
        self.source_text = (ROOT / "src/game/8002A58C.c").read_text()
        (self.directory / "config/us").mkdir(parents=True)
        self.manifest = self.directory / "config/us/matching_units.json"
        config = json.loads(worker.MATCHING_MANIFEST.read_text())
        config["units"] = [unit for unit in config["units"] if unit["name"] != UNIT]
        self.manifest.write_text(json.dumps(config))
        for name in ("ROOT", "MATCHING_MANIFEST"):
            self.addCleanup(setattr, worker, name, getattr(worker, name))
        worker.ROOT = self.directory
        worker.MATCHING_MANIFEST = self.manifest
        self.output = self.directory / "build/us/out"
        self.arguments = argparse.Namespace(
            output=self.output, executable=EXECUTABLE, assembly=ASSEMBLY,
            binutils_prefix="mipsel-linux-gnu-")

    def candidate(self, *, matched: bool = True, source_text: str | None = None,
                  source_sha256: str | None = None,
                  review_required: bool = True,
                  settings: dict | None = None, name: str = UNIT) -> Path:
        folder = self.output / "inbox" / name
        folder.mkdir(parents=True, exist_ok=True)
        text = self.source_text if source_text is None else source_text
        (folder / "candidate.c").write_text(text)
        import hashlib

        digest = hashlib.sha256(text.encode()).hexdigest()
        entry = {
            "name": name,
            "address": f"0x{ADDRESS:08X}",
            "size": f"0x{SIZE:X}",
            "source": "candidate.c",
            "symbols": {"D_8009F6DC": "0x8009F6DC"},
            "source_sha256": digest if source_sha256 is None else source_sha256,
            "matched": matched,
            "compile_settings": SETTINGS if settings is None else settings,
        }
        if review_required:
            entry["semantic_review_required"] = True
        (folder / "unit.json").write_text(json.dumps(entry))
        return folder

    def review(self) -> None:
        with contextlib.redirect_stdout(io.StringIO()) as printed:
            code = worker.preview_command(self.arguments)
        self.assertEqual(code, 0, printed.getvalue())

    def promote(self) -> tuple[int, str]:
        with contextlib.redirect_stdout(io.StringIO()) as printed:
            code = worker.promote_command(self.arguments)
        return code, printed.getvalue()

    def added_units(self) -> list[dict]:
        return [unit for unit in json.loads(self.manifest.read_text())["units"]
                if unit["name"] == UNIT]

    def test_unmatched_candidate_is_refused(self) -> None:
        self.candidate(matched=False)
        before = self.manifest.read_text()
        code, output = self.promote()
        self.assertEqual(code, 1, output)
        self.assertIn("not marked matched", output)
        self.assertEqual(self.manifest.read_text(), before)
        self.assertFalse((self.directory / "src/game/8002A58C.c").is_file())

    def test_wrong_source_hash_is_refused(self) -> None:
        self.candidate(source_sha256="deliberately-wrong-hash")
        before = self.manifest.read_text()
        code, output = self.promote()
        self.assertEqual(code, 1, output)
        self.assertIn("source_sha256 does not match", output)
        self.assertEqual(self.manifest.read_text(), before)

    def test_outstanding_review_is_refused(self) -> None:
        # The worker marks every candidate as needing a review; promotion must not
        # treat that as done while no review record exists.
        self.candidate()
        code, output = self.promote()
        self.assertEqual(code, 1, output)
        self.assertIn("review still outstanding", output)
        self.assertEqual(self.added_units(), [])

    def test_changed_source_invalidates_the_review(self) -> None:
        folder = self.candidate()
        self.review()
        (folder / "candidate.c").write_text(self.source_text + "\n/* changed */\n")
        entry = json.loads((folder / "unit.json").read_text())
        import hashlib

        entry["source_sha256"] = hashlib.sha256(
            (folder / "candidate.c").read_bytes()).hexdigest()
        (folder / "unit.json").write_text(json.dumps(entry))
        code, output = self.promote()
        self.assertEqual(code, 1, output)
        self.assertIn("different source", output)
        self.assertEqual(self.added_units(), [])

    def test_failed_preview_cannot_be_bypassed(self) -> None:
        folder = self.candidate()
        self.review()
        broken = self.source_text.replace("!= 0", "== 0")
        (folder / "candidate.c").write_text(broken)
        entry = json.loads((folder / "unit.json").read_text())
        import hashlib

        entry["source_sha256"] = hashlib.sha256(broken.encode()).hexdigest()
        (folder / "unit.json").write_text(json.dumps(entry))
        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(worker.preview_command(self.arguments), 1)
        self.assertFalse((folder / "review.json").is_file())
        code, output = self.promote()
        self.assertEqual(code, 1, output)
        self.assertEqual(self.added_units(), [])

    def test_promotion_keeps_the_verified_argument_order(self) -> None:
        # A repeated optimisation flag must survive verbatim; filtering duplicates
        # by membership would turn ``-O2 ... -O2`` into whichever came first.
        settings = dict(SETTINGS)
        settings["compiler_flags_extra"] = ["-O2"]
        self.candidate(settings=settings)
        self.review()
        code, output = self.promote()
        self.assertEqual(code, 0, output)
        units = self.added_units()
        self.assertEqual(len(units), 1, output)
        self.assertEqual(units[0]["compiler_flags"],
                         SETTINGS["compiler_flags"] + ["-O2"])
        self.assertNotIn("compiler_flags_extra", units[0])
        evidence = json.loads(
            (self.directory / f"build/us/matching-evidence/{UNIT}.json").read_text())
        self.assertTrue(evidence["result"]["exact"])
        self.assertEqual(evidence["profile"]["compiler_flags"],
                         SETTINGS["compiler_flags"] + ["-O2"])
        self.assertEqual(evidence["fingerprint"]["source_sha256"],
                         json.loads((self.output / "inbox" / UNIT / "unit.json")
                                    .read_text())["source_sha256"])
        self.assertTrue((self.directory / "src/game/8002A58C.c").is_file())

    def test_evidence_is_reused_only_for_the_same_fingerprint(self) -> None:
        self.candidate()
        self.review()
        self.assertEqual(self.promote()[0], 0)
        evidence_file = self.directory / f"build/us/matching-evidence/{UNIT}.json"
        first = json.loads(evidence_file.read_text())
        self.assertFalse(first["reused_evidence"])
        # Second promotion of the same evidence cannot happen (the unit is known);
        # the reuse gate itself is exercised through the verdict helper.
        prepared, reason = worker.promotion_verdict(
            json.loads((self.output / "inbox" / UNIT / "unit.json").read_text()),
            self.output / "inbox" / UNIT,
            json.loads(self.manifest.read_text()), EXECUTABLE.read_bytes(),
            "mipsel-linux-gnu-", self.output, None)
        self.assertIsNone(reason)
        self.assertTrue(prepared["evidence"]["reused_evidence"])


@unittest.skipUnless(HAVE_INPUTS, "needs the pinned toolchain and the retail executable")
class MatrixIdentityTests(unittest.TestCase):
    """Point 5 (small fixes) for ``tools/compiler_matrix.py``."""

    def setUp(self) -> None:
        matrix = __import__("compiler_matrix")
        self.matrix = matrix
        target = matrix.TARGETS[UNIT]
        self.candidate = {"name": UNIT, "ratio": 1.0, "words": "3/3",
                          "source": ROOT / "src/game/8002A58C.c",
                          "symbols": {"D_8009F6DC": "0x8009F6DC"},
                          "extras": {"compiler_flags_extra": ["-O2"],
                                     "maspsx_flags_extra": ["--dont-expand-li"]}}
        self.assertIsNotNone(target)

    def test_empty_maspsx_list_is_written_back(self) -> None:
        result = self.matrix.compile_once(self.candidate, "gcc-2.8.1-psx", "-O2",
                                          li_mode=False)
        self.assertEqual(result["maspsx_flags_extra"], [])
        self.assertIs(result["li_mode"], False)

    def test_link_and_li_runs_keep_separate_evidence(self) -> None:
        plain = self.matrix.compile_once(self.candidate, "gcc-2.8.1-psx", "-O2",
                                         link=False, li_mode=False)
        linked = self.matrix.compile_once(self.candidate, "gcc-2.8.1-psx", "-O2",
                                          link=True, li_mode=True)
        self.assertNotEqual(plain["work"], linked["work"])
        self.assertTrue((Path(plain["work"]) / "result.json").is_file())
        self.assertTrue((Path(linked["work"]) / "result.json").is_file())


if __name__ == "__main__":
    unittest.main()
