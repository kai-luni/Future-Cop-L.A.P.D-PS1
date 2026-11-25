#!/usr/bin/env python3
"""Check explicit src/game/file.c:line anchors in the Precinct Assault docs."""

from functools import cache
from pathlib import Path
import re
import sys

ROOT = Path(__file__).resolve().parents[1]
DOCS = (
    "docs/precinct-assault-movers.md",
    "docs/precinct-assault-map-objects.md",
)
ANCHOR = re.compile(r"(src/game/\w+\.c):([0-9]+(?:-[0-9]+)?(?:,\s*[0-9]+(?:-[0-9]+)?)*)")
C_TOKEN = re.compile(r'/\*.*?\*/|//[^\n]*|"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'', re.S)


@cache
def code_lines(source):
    # Strip comments while preserving line numbers (and strings containing //).
    def strip_comment(match):
        token = match[0]
        return "\n" * token.count("\n") if token.startswith("/") else token

    return C_TOKEN.sub(strip_comment, (ROOT / source).read_text()).split("\n")


def main():
    checked = failures = 0
    for doc in DOCS:
        for doc_line, text in enumerate((ROOT / doc).read_text().splitlines(), 1):
            for source, locations in ANCHOR.findall(text):
                try:
                    lines = code_lines(source)
                except OSError as exc:
                    print(f"{doc}:{doc_line}: {exc}", file=sys.stderr)
                    failures += 1
                    continue
                for location in locations.split(","):
                    # Check both endpoints; a range may contain comments or spacing.
                    for number in location.split("-"):
                        checked += 1
                        line = int(number)
                        if not 1 <= line <= len(lines) or not lines[line - 1].strip():
                            print(f"{doc}:{doc_line}: {source}:{line} does not point to code",
                                  file=sys.stderr)
                            failures += 1
    print(f"PA source anchors: {checked} line references checked, {failures} failures")
    return 1 if failures or not checked else 0


if __name__ == "__main__":
    sys.exit(main())
