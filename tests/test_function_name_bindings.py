"""Every renamed function must keep its retail symbol.

A unit may carry any C identifier, but the object has to export the retail
`func_XXXXXXXX` name: the verifier only compares bytes, so a definition that
lost its `__asm__("func_XXXXXXXX")` label still passes while emitting the C
name instead of the retail one. This test closes that hole.

Definitions whose name is already a retail symbol (`func_XXXXXXXX`,
`D_XXXXXXXX`, `jtbl_XXXXXXXX`, `entrypoint`) need no label because the
manifest `.set` gives them their address under that same name.
"""
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GAME = ROOT / "src/game"

# Pre-existing definitions with a readable C name and no asm label; their
# retail names are reached through D_ symbols by the callers.
KNOWN_UNBOUND = {
    ("800754B8.c", "gAttachMessageName"),
    ("800889DC.c", "gCodeDataSource"),
}

DEFINITION = re.compile(
    r'^[A-Za-z_][A-Za-z0-9_ \t\*]*?\b([A-Za-z_]\w*)\s*\([^;{}]*\)\s*\{', re.M
)
RETAIL_NAME = re.compile(r'^(?:func|D|jtbl|RODATA)_[0-9A-Fa-f]{8}$|^entrypoint$')
KEYWORDS = {"if", "for", "while", "switch", "return", "sizeof", "do"}


def strip_comments(text):
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.S)
    return re.sub(r'//[^\n]*', '', text)


def definitions(text):
    """Yield (name, is_static) for every function definition in *text*."""
    for match in DEFINITION.finditer(strip_comments(text)):
        name = match.group(1)
        if name not in KEYWORDS:
            yield name, "static" in match.group(0)


class FunctionNameBindingTest(unittest.TestCase):
    def test_renamed_definitions_carry_their_retail_label(self):
        failures = []
        for path in sorted(GAME.glob("*.c")):
            retail = "func_" + path.stem.upper()
            text = path.read_text(errors="ignore")
            names = {n for n, is_static in definitions(text) if not is_static}
            if not names or f'__asm__("{retail}")' in text:
                continue
            for name in names:
                if RETAIL_NAME.match(name) or (path.name, name) in KNOWN_UNBOUND:
                    continue
                failures.append(f'{path.name}: {name} needs __asm__("{retail}")')
        if failures:
            self.fail("definitions without a retail symbol binding:\n" + "\n".join(failures))

    def test_known_unbound_entries_are_still_unbound(self):
        for filename, name in sorted(KNOWN_UNBOUND):
            path = GAME / filename
            if not path.is_file():
                continue
            text = path.read_text(errors="ignore")
            if re.search(r'__asm__\("func_[0-9A-F]{8}"\)', text):
                self.fail(
                    f"{filename} is now asm-labelled; drop {name} from KNOWN_UNBOUND")
            self.assertIn(
                name, {n for n, _ in definitions(text)},
                f"{name} no longer defined in {filename}")


if __name__ == "__main__":
    unittest.main()