"""The shared mock library stays the single definition of each stand-in.

tests/host/mocks/ exists because the same PS1 stand-in was being written out
again per harness, with quietly different return conventions. A harness that
goes back to carrying its own copy of a symbol the library already provides
reintroduces exactly that - and it does not fail: the link succeeds whenever the
private copy is the only one in that build, so the duplication is invisible.

This test makes it visible. It reuses the scanner in tests/host/mock_inventory.py
(so the report a human reads and the check CI enforces cannot disagree), and
fails when a retail label that tests/host/mocks/ defines is also defined outside
it.

The exemption list is deliberate and small: a copy that predates the library and
belongs to a harness this change did not convert. Every entry is a conversion
still to do, not an accepted state, and a stale entry fails its own test.
"""
import sys
import unittest
from pathlib import Path

HOST = Path(__file__).resolve().parent / "host"
MOCKS = HOST / "mocks"

sys.path.insert(0, str(HOST))
import mock_inventory  # noqa: E402


# Retail labels a harness still defines privately even though the library has a
# shared version. Converting one means deleting its line here.
#
# This change converts exactly one harness (naming round 7, to mocks/sound_mock.c)
# because the brief was explicit: prove reuse on one existing test, do not refactor
# working tests beyond that. The rest are listed so the duplication is visible and
# counted instead of quietly tolerated - each entry is a follow-up, and the list
# fails once an entry goes stale.
KNOWN_LOCAL = {
    # A private SE recorder with its own ordered log (sound_log/sound_count) and
    # a `struct WObj *` second argument; it asserts the order of the SE ids a
    # weapon animation emits. Moving it onto the library has to keep that log.
    ("weapon_anim_harness.c", "func_80046C84"),
    # Same shape: a per-harness SE counter with its own log, not the shared one.
    ("cadence_harness.c", "func_80046C84"),
    ("precinct_assault_harness.c", "func_80046C84"),
    # Two BIOS string thunks copied inline; same semantics as mocks/bios_mock.c,
    # but this harness asserts through its own call counters.
    ("precinct_assault_harness.c", "func_8008CCA0"),
    ("precinct_assault_harness.c", "func_8008CCB0"),
    # A stream starter whose call is recorded in the harness's shared `calls[]`
    # array together with three other stand-ins, so its position in that array
    # is part of what the harness asserts.
    ("naming_round_harness.c", "func_8002A16C"),
    # The legacy shared stub file for build.sh/harness.c: silent no-ops that
    # nothing asserts on. Replacing them with mocks/gpu_mock.c would make the
    # texture harness record transfers; that is a change to build.sh's link set,
    # not part of the unification.
    ("stubs.c", "func_80092F38"),
    ("stubs.c", "func_80092C8C"),
}


def definitions(path):
    """{retail label: [line numbers]} of the stand-in definitions in one file."""
    return {r["label"]: [0] for r in mock_inventory.scan_file(path)
            if r["kind"] == "definition"}


def shared_labels():
    """Every retail label the mock library defines."""
    labels = set()
    for path in sorted(MOCKS.glob("*.c")):
        labels.update(definitions(path))
    return labels


class SharedMockLibraryTests(unittest.TestCase):
    def test_library_provides_the_reusable_set(self):
        labels = shared_labels()
        self.assertGreater(
            len(labels), 10,
            "tests/host/mocks/ should hold the reusable stand-in set",
        )

    def test_no_harness_redefines_a_shared_mock(self):
        shared = shared_labels()
        self.assertTrue(shared, "no shared mock definitions found")

        offenders = []
        for path in sorted(HOST.rglob("*.[ch]")):
            if MOCKS in path.parents:
                continue
            if path.name == "mock_inventory.py":
                continue
            for label in definitions(path):
                if label not in shared:
                    continue
                if (path.name, label) in KNOWN_LOCAL:
                    continue
                offenders.append(f"{path.relative_to(HOST)}: {label}")

        self.assertEqual(
            offenders, [],
            "these files define a retail label the shared mock library already "
            "provides; link mocks/<name>_mock.o instead of copying the "
            "stand-in:\n  " + "\n  ".join(offenders),
        )

    def test_every_mock_binds_to_its_retail_label(self):
        """A stand-in found by C name alone would not prove the binding."""
        from_name_only = []
        for path in sorted(MOCKS.glob("*.c")):
            for rec in mock_inventory.scan_file(path):
                if rec["kind"] != "definition":
                    continue
                if rec["binding"] != "asm-alias":
                    from_name_only.append(f"{path.name}: {rec['label']}")
        self.assertEqual(
            from_name_only, [],
            "mock stand-ins must carry __asm__(\"<retail label>\") so a wrong "
            "binding is a link error:\n  " + "\n  ".join(from_name_only),
        )

    def test_each_mock_object_includes_its_own_header(self):
        """Definition and declaration must not drift apart.

        The stand-ins are declared in the header and defined in the object; a
        mock that did not include its own header could define a prototype that
        disagrees with the one harnesses compile against, and both would still
        link.
        """
        missing = []
        for c_path in sorted(MOCKS.glob("*.c")):
            h_path = c_path.with_suffix(".h")
            if not h_path.exists():
                continue
            text = c_path.read_text(errors="ignore")
            if f'"{h_path.name}"' not in text:
                missing.append(f"{c_path.name} does not include {h_path.name}")
        self.assertEqual(
            missing, [],
            "a mock object must include its own header:\n  " + "\n  ".join(missing),
        )

    def test_known_local_entries_still_exist(self):
        """A stale exemption would silently widen the hole it documents."""
        stale = []
        for filename, label in sorted(KNOWN_LOCAL):
            path = HOST / filename
            if not path.exists() or label not in definitions(path):
                stale.append(f"{filename}: {label}")
        self.assertEqual(
            stale, [],
            "KNOWN_LOCAL lists copies that no longer exist; remove them:\n  "
            + "\n  ".join(stale),
        )


if __name__ == "__main__":
    unittest.main()
