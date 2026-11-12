"""CLI smoke tests for tools/offline_decomp.py.

The worker itself is executed as a subprocess (``run``, ``status``, ``preview``) so
argument parsing, state handling and the review gate are exercised end to end. A
local fake backend supplies fixed proposals; no model call leaves the machine.
``promote`` is called as a function against a throwaway copy of the project,
because the command writes into ``src/game/`` and the matching manifest.

The byte-level cases need the pinned toolchain and ``baseroms/us/SLUS_007.39``
(both gitignored); without them the module skips instead of failing.
"""

from __future__ import annotations

import json
import shutil
import sys
import tempfile
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TOOLS = ROOT / "tools"
sys.path.insert(0, str(TOOLS))

TOOLCHAIN = ROOT / ".toolchain/gcc-2.8.1-psx/cc1"
EXECUTABLE = ROOT / "baseroms/us/SLUS_007.39"
ASSEMBLY = ROOT / "asm/us/main.s"
HAVE_BUILD_INPUTS = TOOLCHAIN.is_file() and EXECUTABLE.is_file() and ASSEMBLY.is_file()

# A small unit that is already accepted, so every case below can reuse its source
# and its oracle range instead of inventing one.
UNIT = "func_8002A58C"
UNIT_ADDRESS = 0x8002A58C
UNIT_SIZE = 0xC

# A fixed, self-contained target for the `run`/`status` cases: it brings its own listing
# block and its own executable image, so the tests never depend on which functions are
# still open in the real catalogue. Its name is not a catalogued function.
FIXTURE_TARGET = "func_80020000"
FIXTURE_ADDRESS = 0x80020000
FIXTURE_WORDS = (
    ("27BDFFE8", "addiu      $sp, $sp, -0x18"),
    ("AFBF0014", "sw         $ra, 0x14($sp)"),
    ("AFB00010", "sw         $s0, 0x10($sp)"),
    ("00808021", "addu       $s0, $a0, $zero"),
    ("0C008000", "jal        func_80020040"),
    ("00000000", "nop"),
    ("02001021", "addu       $v0, $s0, $zero"),
    ("8FBF0014", "lw         $ra, 0x14($sp)"),
    ("8FB00010", "lw         $s0, 0x10($sp)"),
    ("03E00008", "jr         $ra"),
    ("27BD0018", "addiu      $sp, $sp, 0x18"),
)
FIXTURE_SIZE = 4 * len(FIXTURE_WORDS)
BACKEND_RESPONSE = {
    "source": '#include "game/types.h"\n\n'
              'extern u32 D_8009F6DC;\n\n'
              'int func_8002A58C(void)\n{\n    return D_8009F6DC != 0;\n}\n',
    "symbols": {"D_8009F6DC": "0x8009F6DC"},
    "note": "smoke test proposal",
}


def run_cli(*arguments: str) -> tuple[int, str]:
    import subprocess

    completed = subprocess.run(
        [sys.executable, str(TOOLS / "offline_decomp.py"), *arguments],
        cwd=ROOT, text=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
        timeout=600,
    )
    return completed.returncode, completed.stdout + completed.stderr


def fixture_listing() -> str:
    """One open target, written by the test itself instead of taken from the catalogue.

    The `run`/`status` cases used to pick the smallest still-open function out of
    `asm/us/main.s`. That made them depend on campaign progress: once every 8–64-byte
    target was accepted, the helper found nothing and three tests failed — and with them
    `verify_repo()`, which parked finished promotions as `verify-failed`.
    """
    words = [
        ("27BDFFE8", "addiu      $sp, $sp, -0x18"),
        ("AFBF0014", "sw         $ra, 0x14($sp)"),
        ("AFB00010", "sw         $s0, 0x10($sp)"),
        ("00808021", "addu       $s0, $a0, $zero"),
        ("0C008000", "jal        func_80020040"),
        ("00000000", "nop"),
        ("02001021", "addu       $v0, $s0, $zero"),
        ("8FBF0014", "lw         $ra, 0x14($sp)"),
        ("8FB00010", "lw         $s0, 0x10($sp)"),
        ("03E00008", "jr         $ra"),
        ("27BD0018", "addiu      $sp, $sp, 0x18"),
    ]
    lines = [f"nonmatching {FIXTURE_TARGET}, 0x{FIXTURE_SIZE:X}", "",
             f"glabel {FIXTURE_TARGET}"]
    for index, (word, text) in enumerate(words):
        address = FIXTURE_ADDRESS + index * 4
        lines.append(f"    /* {0x800 + address - 0x80010000:X} {address:08X} {word} */  {text}")
    lines += [f"endlabel {FIXTURE_TARGET}", ""]
    return "\n".join(lines)


def fixture_executable() -> bytes:
    """A minimal image that holds the fixture range and nothing else."""
    offset = 0x800 + FIXTURE_ADDRESS - 0x80010000
    image = bytearray(offset + FIXTURE_SIZE)
    for index, (word, _text) in enumerate(
            (line.split()[3], line.split()[4]) for line in fixture_listing().splitlines()
            if line.strip().startswith("/*")):
        image[offset + index * 4:offset + index * 4 + 4] = bytes.fromhex(word)
    return bytes(image)


def run_args(output: Path, command: str, *extra: str):
    """Parse the worker's own `run` arguments, so the namespace stays in sync."""
    import offline_decomp

    return offline_decomp.parser().parse_args(
        ["run", "--output", str(output), "--backend-command", command,
         "--jobs", "1", "--attempts", "1", *extra])


def fake_backend(path: Path, response: dict) -> None:
    path.write_text(
        "import json, sys\n"
        f"print(json.dumps({json.dumps(response)}))\n"
    )


def inbox_unit(directory: Path, name: str = UNIT, size: int = UNIT_SIZE,
               source: Path | None = None, symbols: dict | None = None,
               compile_settings: dict | None = None) -> Path:
    """Write one review-inbox candidate the way the worker would."""
    folder = directory / "inbox" / name
    folder.mkdir(parents=True, exist_ok=True)
    candidate = folder / "candidate.c"
    if source is None:
        candidate.write_text(BACKEND_RESPONSE["source"])
    else:
        candidate.write_text(source.read_text())
    entry = {
        "name": name,
        "address": f"0x{UNIT_ADDRESS:08X}",
        "size": f"0x{size:X}",
        "source": "candidate.c",
        "symbols": {"D_8009F6DC": "0x8009F6DC"} if symbols is None else symbols,
        "matched": True,
        "semantic_review_required": True,
    }
    import hashlib

    entry["source_sha256"] = hashlib.sha256(candidate.read_bytes()).hexdigest()
    settings = compile_settings
    if settings is None:
        settings = {
            "compiler": "gcc-2.8.1-psx",
            "compiler_flags": ["-quiet", "-msoft-float", "-mgas", "-fgnu-linker", "-O2", "-G4"],
            "small_data_symbols": {"D_8009F6DC": 4},
            "link": True,
            "linker_symbols": {"D_8009F6DC": "0x8009F6DC", "_gp": "0x8009F214"},
        }
    if settings:
        entry["compile_settings"] = settings
    (folder / "unit.json").write_text(json.dumps(entry))
    return folder


@unittest.skipUnless(HAVE_BUILD_INPUTS, "needs the pinned toolchain and the retail executable")
class WorkerCliTests(unittest.TestCase):
    def setUp(self) -> None:
        # The worker refuses output outside build/: keep the scratch dirs there.
        self.directory = Path(tempfile.mkdtemp(prefix="worker-cli-", dir=ROOT / "build/us"))
        self.addCleanup(shutil.rmtree, self.directory, True)
        self.output = self.directory / "output"
        self.backend = self.directory / "backend.py"
        fake_backend(self.backend, BACKEND_RESPONSE)

    def command(self) -> str:
        return f"{sys.executable} {self.backend}"

    def fixture_inputs(self) -> tuple[Path, Path]:
        """Write the fixed listing block and the matching image for `run`/`status`.

        Both files live in the test's own scratch directory, so the cases no longer read
        campaign progress out of the real catalogue.
        """
        listing = self.directory / "fixture.s"
        listing.write_text(fixture_listing())
        executable = self.directory / "fixture.bin"
        executable.write_bytes(fixture_executable())
        return listing, executable

    def fixture_arguments(self) -> list[str]:
        listing, executable = self.fixture_inputs()
        return ["--assembly", str(listing), "--executable", str(executable)]

    def test_run_writes_state(self) -> None:
        fake_backend(self.backend, {"source": "int f(void)\n{\n    return 0;\n}\n",
                                    "symbols": {}, "note": "deliberately wrong"})
        code, output = run_cli("run", "--jobs", "1", "--attempts", "1",
                               "--function", FIXTURE_TARGET, "--output", str(self.output),
                               *self.fixture_arguments(),
                               "--backend-command", self.command())
        self.assertEqual(code, 0, output)
        state = json.loads((self.output / "work" / FIXTURE_TARGET / "state.json").read_text())
        self.assertEqual(state["attempts"], 1)
        self.assertEqual(state["status"], "pending")

    def test_run_reaches_exact(self) -> None:
        # The fake backend proposes the accepted source, so a target that is open in
        # the manifest becomes an inbox candidate without any model call.
        import offline_decomp

        fake_backend(self.backend, {**BACKEND_RESPONSE,
                                    "source": BACKEND_RESPONSE["source"]})
        with tempfile.TemporaryDirectory(prefix="worker-manifest-") as directory:
            copy = Path(directory) / "matching_units.json"
            config = json.loads(offline_decomp.MATCHING_MANIFEST.read_text())
            config["units"] = [u for u in config["units"] if u["name"] != UNIT]
            copy.write_text(json.dumps(config))
            original = offline_decomp.MATCHING_MANIFEST
            import offline_decomp as module
            try:
                module.MATCHING_MANIFEST = copy
                state = offline_decomp.Runner(run_args(self.output, self.command())
                                              ).run_target(
                    next(t for t in offline_decomp.parse_targets(ASSEMBLY.read_text())
                         if t.name == UNIT))
            finally:
                module.MATCHING_MANIFEST = original
        self.assertEqual(state["status"], "exact")
        self.assertTrue((self.output / "inbox" / UNIT / "unit.json").is_file())

    def test_run_uses_custom_rules_file(self) -> None:
        # `--rules` is how per-target context (neighbouring verified sources) reaches
        # the model: the request's guidance must be exactly that file's text.
        rules = self.directory / "rules.md"
        rules.write_text("# custom rules\nmarker-80024EC8\n")
        capture = self.directory / "capture.py"
        capture.write_text(
            "import json, pathlib, sys\n"
            "request = json.load(sys.stdin)\n"
            f"pathlib.Path({str(self.directory / 'request.json')!r}).write_text(json.dumps(request))\n"
            "print(json.dumps({'source': 'int f(void){\\n    return 0;\\n}\\n',"
            " 'symbols': {}, 'note': 'captured'}))\n")
        code, output = run_cli("run", "--jobs", "1", "--attempts", "1",
                               "--function", FIXTURE_TARGET, "--output", str(self.output),
                               *self.fixture_arguments(),
                               "--rules", str(rules),
                               "--backend-command", f"{sys.executable} {capture}")
        self.assertEqual(code, 0, output)
        request = json.loads((self.directory / "request.json").read_text())
        self.assertIn("marker-80024EC8", request["guidance"])
        self.assertNotIn("reconstruction_rules", request["guidance"])

    def test_status_reports_state(self) -> None:
        fake_backend(self.backend, {"source": "int f(void)\n{\n    return 0;\n}\n",
                                    "symbols": {}, "note": "deliberately wrong"})
        run_cli("run", "--jobs", "1", "--attempts", "1", "--function", FIXTURE_TARGET,
                "--output", str(self.output), *self.fixture_arguments(),
                "--backend-command", self.command())
        code, output = run_cli("status", "--output", str(self.output))
        self.assertEqual(code, 0, output)
        self.assertIn("targets=1 exact=0", output)

    def test_run_needs_no_open_catalogue_target(self) -> None:
        """Regression: the run/status cases must work with a fully solved catalogue.

        The three cases above used to derive their target from the smallest still-open
        function, so a complete catalogue made them fail — and `verify_repo()` then parked
        correct promotions as `verify-failed`. Here the manifest is emptied of the fixture
        name and the listing holds nothing but the fixture block.
        """
        import offline_decomp

        listing = self.directory / "only-fixture.s"
        listing.write_text(fixture_listing())
        executable = self.directory / "fixture.bin"
        executable.write_bytes(fixture_executable())
        fake_backend(self.backend, {"source": "int f(void)\n{\n    return 0;\n}\n",
                                    "symbols": {}, "note": "deliberately wrong"})
        code, output = run_cli("run", "--jobs", "1", "--attempts", "1",
                               "--function", FIXTURE_TARGET, "--output", str(self.output),
                               "--assembly", str(listing), "--executable", str(executable),
                               "--backend-command", self.command())
        self.assertEqual(code, 0, output)
        self.assertIn(f"Running 1 target(s)", output)
        self.assertTrue((self.output / "work" / FIXTURE_TARGET / "state.json").is_file())

    def test_preview_accepts_exact_candidate(self) -> None:
        inbox_unit(self.output)
        code, output = run_cli("preview", "--output", str(self.output))
        self.assertEqual(code, 0, output)
        self.assertIn(f"OK   {UNIT}: {UNIT_SIZE} bytes", output)
        self.assertIn("failed 0", output)

    def test_preview_accepts_zero_alignment(self) -> None:
        # The pinned compiler emits 16 bytes for this 12-byte unit; the verifier's
        # extent rule allows the surplus only while it is zero.
        inbox_unit(self.output)
        code, output = run_cli("preview", "--output", str(self.output))
        self.assertEqual(code, 0, output)
        self.assertIn("zero padding", output)

    def test_preview_rejects_too_short_output(self) -> None:
        # Claiming more bytes than the compiler produces must not pass.
        inbox_unit(self.output, size=UNIT_SIZE + 8)
        code, output = run_cli("preview", "--output", str(self.output))
        self.assertEqual(code, 1, output)
        self.assertIn("FAIL", output)

    def test_preview_rejects_real_mismatch(self) -> None:
        wrong = self.directory / "wrong.c"
        wrong.write_text('#include "game/types.h"\n\n'
                         'extern u32 D_8009F6DC;\n\n'
                         'int func_8002A58C(void)\n{\n    return D_8009F6DC == 0;\n}\n')
        inbox_unit(self.output, source=wrong)
        code, output = run_cli("preview", "--output", str(self.output))
        self.assertEqual(code, 1, output)
        self.assertIn("bytes differ", output)


@unittest.skipUnless(HAVE_BUILD_INPUTS, "needs the pinned toolchain and the retail executable")
class PromoteTests(unittest.TestCase):
    def setUp(self) -> None:
        self.directory = Path(tempfile.mkdtemp(prefix="worker-promote-"))
        self.addCleanup(shutil.rmtree, self.directory, True)
        import argparse

        import offline_decomp

        self.worker = offline_decomp
        (self.directory / "config/us").mkdir(parents=True)
        shutil.copy(self.worker.MATCHING_MANIFEST, self.directory / "config/us/matching_units.json")
        for name in ("ROOT", "MATCHING_MANIFEST"):
            original = getattr(self.worker, name)
            self.addCleanup(setattr, self.worker, name, original)
        self.worker.ROOT = self.directory
        self.worker.MATCHING_MANIFEST = self.directory / "config/us/matching_units.json"
        self.output = self.directory / "build/us/out"
        self.arguments = argparse.Namespace(output=self.output, executable=EXECUTABLE,
                                            assembly=ASSEMBLY,
                                            binutils_prefix="mipsel-linux-gnu-")

    def promote(self) -> tuple[int, str]:
        import contextlib
        import io

        with contextlib.redirect_stdout(io.StringIO()) as printed:
            code = self.worker.promote_command(self.arguments)
        return code, printed.getvalue()

    def test_promote_refuses_candidate_without_review(self) -> None:
        # The worker marks every candidate as review-required; without an
        # independent review record promotion must not touch the repository.
        config = json.loads(self.worker.MATCHING_MANIFEST.read_text())
        config["units"] = [unit for unit in config["units"] if unit["name"] != UNIT]
        self.worker.MATCHING_MANIFEST.write_text(json.dumps(config))
        inbox_unit(self.output)
        before = self.worker.MATCHING_MANIFEST.read_text()
        code, output = self.promote()
        self.assertEqual(code, 1, output)
        self.assertIn("review still outstanding", output)
        self.assertEqual(self.worker.MATCHING_MANIFEST.read_text(), before)
        self.assertFalse((self.directory / f"src/game/{UNIT_ADDRESS:08X}.c").is_file())

    def test_promote_records_link_settings(self) -> None:
        config = json.loads(self.worker.MATCHING_MANIFEST.read_text())
        config["units"] = [unit for unit in config["units"] if unit["name"] != UNIT]
        self.worker.MATCHING_MANIFEST.write_text(json.dumps(config))
        inbox_unit(self.output)
        import contextlib
        import io

        with contextlib.redirect_stdout(io.StringIO()):
            self.assertEqual(self.worker.preview_command(self.arguments), 0)
        before = json.loads(self.worker.MATCHING_MANIFEST.read_text())
        code, output = self.promote()
        self.assertEqual(code, 0, output)
        after = json.loads(self.worker.MATCHING_MANIFEST.read_text())
        added = [unit for unit in after["units"] if unit["name"] not in
                 {u["name"] for u in before["units"]}]
        self.assertEqual([unit["name"] for unit in added], [UNIT], output)
        unit = added[0]
        self.assertTrue(unit["link"])
        self.assertEqual(unit["linker_symbols"]["_gp"], "0x8009F214")
        self.assertEqual(unit["small_data_symbols"], {"D_8009F6DC": 4})
        self.assertTrue((self.directory / f"src/game/{UNIT_ADDRESS:08X}.c").is_file())
        self.assertTrue((self.directory / f"build/us/matching-evidence/{UNIT}.json").is_file())

    def test_promote_skips_known_unit(self) -> None:
        inbox_unit(self.output)  # already in the manifest
        before = json.loads(self.worker.MATCHING_MANIFEST.read_text())

        self.promote()
        after = json.loads(self.worker.MATCHING_MANIFEST.read_text())
        self.assertEqual(len(after["units"]), len(before["units"]))


if __name__ == "__main__":
    unittest.main()
