"""Structure test for the environment render path.

docs/environment-render-path.md describes the per-frame environment path
(gRunSessionFrame -> gRunRegionPass -> gDrawViewLayers -> ...). Each unit of
that area documents its role in an `@par Environment path` paragraph. This
test pins the curated unit list and asserts, per unit, that

  (a) the unit file still carries its `@par Environment path` paragraph,
  (b) the unit's function has a readable name bound to the retail symbol:
      C units declare `<name>(...) __asm__("func_<addr>");` in their own file;
      handwritten (GTE/asm) units export `.globl func_<addr>` and get their
      readable name from an `__asm__("func_<addr>")` declaration in a caller,
  (c) every `func_XXXXXXXX` cited in that paragraph names an existing unit.

It is a contract/structure test only: it proves the documentation and the
symbol bindings are consistent, not that any function behaves correctly.
Behaviour is covered (for a few units) by the qemu harnesses in tests/host/.
"""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GAME = ROOT / "src/game"
DOC = ROOT / "docs/environment-render-path.md"

# Curated from the `@par Environment path` paragraphs in src/game/*.c.
ENVIRONMENT_UNITS = (
    "80010030", "8001006C", "8001010C", "800101D4", "80010234", "8001047C", "80010704",
    "80010750", "80010820", "800108D8", "80010974", "800136C8", "80013AA0", "80013AF0",
    "80013C80", "80013CB8", "80013ECC", "80013F08", "80014164", "8001428C", "80014490",
    "8001484C", "80014C64", "80015830", "80015AF4", "80015B68", "80015F14", "80015F38",
    "800164F4", "80016A64", "80016C88", "80016E6C", "80017058", "800172A8", "80017508",
    "800175A0", "800175F8", "80017668", "80017764", "800177AC", "80017860", "800178DC",
    "80017994", "80017B28", "80017B60", "80017EBC", "800181D4", "8001846C", "80018534",
    "80018860", "800188D0", "80018DC0", "800192AC", "80019310", "8001937C", "800193F8",
    "80019470", "800195F0", "8001996C", "80019BA4", "80019D88", "80019D90", "80019E70",
    "80019F54", "8001A100", "8001A180", "8001A314", "8001A6F8", "8001A81C", "8001AB00",
    "8001AB78", "8001AC28", "8001C148", "8001C214", "8001F0FC", "800211FC", "80021B50",
    "8002333C", "80023478", "80023930", "80024EC8", "80025370", "8002549C", "80025690",
    "800260B8", "800267C8", "80026DDC", "80026E1C", "80026E9C", "800271A4", "80027510",
    "80028618", "8002A5AC", "8002A810", "8002AD58", "8002B588", "8002C584", "8002CC30",
    "8002DAA4", "8002DC94", "8002E630", "8002EED8", "8002F30C", "8003015C", "800302E8",
    "80030AD4", "80030C60", "80031238", "800313F0", "800317BC", "80031990", "800319D4",
    "80031B40", "800324E0", "80036E6C", "80037708", "800378BC", "80037EB4", "8003802C",
    "80038754", "80038A00", "80039C58", "8003A160", "8003A90C", "8003AA94", "8003B898",
    "8003BA50", "8003BEF0", "8003BF1C", "8003C404", "8003CF6C", "8003D240", "8003D680",
    "8003D9C4", "8003D9F8", "8003DB70", "8003E984", "8003F4A0", "8003F530", "8003F570",
    "8003F6E4", "8003F854", "8003FAD4", "8003FF7C", "800402DC", "80040874", "80040A00",
    "80040DCC", "80041070", "80041488", "80041728", "80041880", "80041C6C", "80042004",
    "80042128", "800441DC", "80044260", "800445B0", "80044770", "8004488C", "80044988",
    "80044F8C", "80047888", "80047BE8", "80047F30", "800485AC", "80048BF8", "80048F98",
    "80049100", "8004963C", "800496D0", "80049EA8", "8004A048", "8004A924", "8004ABE4",
    "8004AFC0", "8004B5F8", "8004C228", "8004C5C8", "8004C884", "8004CA98", "8004EDC8",
    "8005027C", "80050E0C", "800518A8", "80051C50", "80051CC4", "80052410", "80052960",
    "80052D7C", "80053A74", "80055EE8", "8005605C", "8005612C", "800564A8", "80056530",
    "80056634", "8005678C", "80056870", "80056994", "800569EC", "800573A4", "800576A8",
    "8005780C", "80057E40", "80058714", "800588D4", "800592D0", "800596C4", "800597C0",
    "80059ABC", "80059DB0", "8005A068", "8005A220", "8005A2BC", "8005A414", "8005A740",
    "8005A828", "8005A9C4", "8005BDE4", "8005CED4", "8005D060", "800619C0", "80061CAC",
    "80062EB8", "80063C5C", "80063DC8", "80064460", "800646BC", "800648E8", "80065134",
    "8006531C", "800654F8", "80065744", "80065834", "80065CA8", "800665E8", "80066684",
    "8006680C", "80066C9C", "80066E98", "80066F78", "8006943C", "800694C8", "80069580",
    "800695A8", "8006BF44", "8006C080", "8006C4C4", "8006C768", "8006C84C", "8006CB58",
    "8006D520", "8006D770", "8006DC24", "8006E988", "8006EAF0", "8006ED78", "8006F170",
    "8006F21C", "8006F310", "8006F844", "8006FF44", "80070048", "80070158", "800703A8",
    "800706C4", "80070730", "80071B40", "80071FA4", "80072F7C", "800734BC", "80073500",
    "8007364C", "80073700", "80073BBC", "80074104", "800753D0", "80075478", "800754B8",
    "80075A84", "800762B0", "800770A8", "80078B98", "8007F1F8", "8008125C", "80082358",
    "800824CC", "800825AC", "800826AC", "80082AB0", "80082B0C", "80082EF0", "80082FA4",
    "80083060", "80083280", "800834C4", "80083764", "80083A10", "80084398", "80085074",
    "800860CC", "800861E4", "80086AC0", "80091F90", "80092078", "800920DC", "80092140",
    "800921F4", "80092230", "80092514", "80092528", "8009253C", "80092550", "80092564",
    "80092634", "80092660", "800926C0", "80092E10", "80092F38", "80092F98", "80092FF8",
    "80093224", "800933B0", "800934BC", "80093954", "80093A4C", "80093A74", "80093A9C",
    "80093AF0", "80093CE8", "80093F58", "80093F78", "80094010", "800940A8", "800940C4",
    "8009446C", "80094CEC", "80095900", "80095B64", "80095C14", "80095C30", "80096070",
    "80096090", "800960CC", "80096484",
)

# The frame-order spine documented in the overview (address, readable name).
SPINE = (
    ("80027510", "gRunSessionFrame"),
    ("80026E1C", "gRunRegionPass"),
    ("80066E98", "gDrawViewLayers"),
    ("8006531C", "gDrawRegionTiles"),
    ("8004A924", "gDrawCellWindowObjects"),
    ("80084398", "gDrawCellWindowModels"),
    ("8003F6E4", "gRunViewScenePasses"),
    ("8003F4A0", "gRunSceneListPasses"),
    ("80075A84", "gRunPolygonPass"),
)

# Handwritten units that no other unit calls through a readable C name, so no
# readable binding exists anywhere (their paragraphs say "no caller").
NO_READABLE_BINDING = {"80095B64"}

# Empty reset stubs: the body is a bare `return`, so no role is provable and
# they are deliberately left unnamed (their paragraph says so).
UNNAMED_EMPTY_STUBS = {"80019D88", "8002A5AC", "80059DB0", "8005A068"}

PARAGRAPH = re.compile(r"@par Environment path\b(.*?)(?=\n\s*\*\s*@|\*/)", re.S)
FUNC_REF = re.compile(r"\bfunc_([0-9A-Fa-f]{8})\b")


def unit_path(addr):
    return GAME / f"{addr.upper()}.c"


def strip_comments(text):
    text = re.sub(r"/\*.*?\*/", "", text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def bound_names(text, addr):
    """Readable names declared with __asm__("func_<addr>") in *text*."""
    pattern = re.compile(
        r'\b([A-Za-z_]\w*)\s*\([^;{}\n]*\)\s*__asm__\(\s*"func_' + addr + r'"\s*\)\s*;')
    names = []
    for line in strip_comments(text).splitlines():
        if line.lstrip().startswith("#"):
            continue
        names.extend(pattern.findall(line))
    return [n for n in names if not n.startswith("func_")]


def environment_paragraph(text):
    match = PARAGRAPH.search(text)
    return match.group(1) if match else None


class EnvironmentPathTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.texts = {}
        for addr in ENVIRONMENT_UNITS:
            path = unit_path(addr)
            if path.is_file():
                cls.texts[addr] = path.read_text(errors="ignore")

    def test_curated_list_has_no_duplicates(self):
        self.assertEqual(len(ENVIRONMENT_UNITS), len(set(ENVIRONMENT_UNITS)))

    def test_every_unit_file_exists(self):
        missing = [a for a in ENVIRONMENT_UNITS if a not in self.texts]
        self.assertEqual(missing, [], "environment units without a source file")

    def test_curated_list_matches_annotated_units(self):
        annotated = sorted(
            p.stem.upper() for p in GAME.glob("*.c")
            if "@par Environment path" in p.read_text(errors="ignore"))
        self.assertEqual(
            sorted(set(annotated) - set(ENVIRONMENT_UNITS)), [],
            "units gained an @par Environment path paragraph; add them here")
        self.assertEqual(
            sorted(set(ENVIRONMENT_UNITS) - set(annotated)), [],
            "curated units lost their @par Environment path paragraph")

    def test_a_units_carry_environment_paragraph(self):
        failures = [a for a, t in self.texts.items()
                    if not (environment_paragraph(t) or "").strip()]
        self.assertEqual(failures, [], "missing/empty @par Environment path")

    def test_b_units_bind_a_readable_name(self):
        failures = []
        for addr, text in sorted(self.texts.items()):
            if bound_names(text, addr):
                continue
            if addr in NO_READABLE_BINDING or addr in UNNAMED_EMPTY_STUBS:
                continue
            if not re.search(r"\.globl(?:\\t|\s)+func_" + addr + r"\b", text):
                failures.append(f'{addr}.c: no readable name with __asm__("func_{addr}")')
                continue
            callers = [p.name for p in GAME.glob("*.c")
                       if bound_names(p.read_text(errors="ignore"), addr)]
            if not callers:
                failures.append(f"{addr}.c: handwritten unit without any readable binding")
        if failures:
            self.fail("\n".join(failures))

    def test_no_readable_binding_entries_are_still_unbound(self):
        for addr in sorted(NO_READABLE_BINDING):
            bound = [p.name for p in GAME.glob("*.c")
                     if bound_names(p.read_text(errors="ignore"), addr)]
            self.assertEqual(bound, [], f"{addr} now has a readable binding; "
                             "drop it from NO_READABLE_BINDING")

    def test_c_paragraph_references_resolve(self):
        failures = []
        for addr, text in sorted(self.texts.items()):
            for ref in FUNC_REF.findall(environment_paragraph(text) or ""):
                if not unit_path(ref).is_file():
                    failures.append(f"{addr}.c cites func_{ref} (no unit file)")
        if failures:
            self.fail("\n".join(failures))

    def test_spine_units_are_curated_and_named(self):
        for addr, name in SPINE:
            self.assertIn(addr, ENVIRONMENT_UNITS)
            self.assertIn(name, bound_names(self.texts[addr], addr), addr)

    def test_doc_lists_every_curated_unit(self):
        self.assertTrue(DOC.is_file(), "docs/environment-render-path.md missing")
        doc = DOC.read_text()
        missing = [a for a in ENVIRONMENT_UNITS if f"`0x{a}`" not in doc]
        self.assertEqual(missing, [], "units missing from the doc test table")

    def test_doc_citations_hold_their_code_fragment(self):
        """Every `src/game/X.c:N` in the doc's citation index still has its
        quoted fragment on line N, and every citation in the doc is indexed."""
        doc = DOC.read_text()
        index = re.findall(
            r"^\| `src/game/([0-9A-F]{8}\.c):(\d+)` \| `([^`]+)` \|$", doc, re.M)
        self.assertTrue(index, "citation index is empty")
        failures = []
        for name, line, fragment in index:
            path = GAME / name
            if not path.is_file():
                failures.append(f"{name}: no such unit")
                continue
            lines = path.read_text(errors="ignore").splitlines()
            n = int(line)
            if n > len(lines) or fragment not in lines[n - 1]:
                failures.append(f"{name}:{line} no longer holds `{fragment}`")
        indexed = {(name, line) for name, line, _ in index}
        for name, line in re.findall(r"src/game/([0-9A-F]{8}\.c):(\d+)", doc):
            if (name, line) not in indexed:
                failures.append(f"{name}:{line} cited but not in the index")
        if failures:
            self.fail("\n".join(failures))


if __name__ == "__main__":
    unittest.main()
