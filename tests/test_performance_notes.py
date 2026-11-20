"""Structure test for docs/performance-notes.md.

The document lists what the retail binary does to stay fast on PS1 hardware,
with one section per finding. Each finding carries a Status line, a Mechanism
line, an Evidence list, a Units list and a Naming proposals list. This test
pins the contract:

  (a) every finding has a non-empty Status, Mechanism, Evidence, Units and
      Naming proposals block,
  (b) Status is one of the three documented values,
  (c) every unit in a Units list exists and its manifest entry matches the
      address in the file name,
  (d) every `src/game/<unit>.c:<line>` citation in the document carries its
      exact code fragment on the cited line, and every citation in the body
      is present in the citation index,
  (e) the index at the top lists exactly the findings, with the same status
      and the same unit count as the section itself.

It is a contract/structure test: it proves the document, the sources and the
manifest agree, not that any function behaves correctly.
"""
import json
import re
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
GAME = ROOT / "src/game"
DOC = ROOT / "docs/performance-notes.md"
MANIFEST = ROOT / "config/us/matching_units.json"

STATUSES = ("confirmed", "likely", "hypothesis")

FINDING = re.compile(r"^### (\d+)\. (.+)$", re.M)
CITATION = re.compile(r"`((?:src/game|asm/us)/[^`]+?):(\d+)`")
INDEX_ROW = r"^(\d+)\. (.+?) - (%s) - (\d+) units?$" % "|".join(STATUSES)


def body_of(doc):
    """The document without its citation index table."""
    marker = "\n## Citation index"
    return doc.split(marker, 1)[0]


def sections(doc):
    """Map finding number to its raw section text (heading excluded)."""
    body = body_of(doc)
    starts = [(m.group(1), m.start()) for m in FINDING.finditer(body)]
    result = {}
    for i, (number, start) in enumerate(starts):
        end = starts[i + 1][1] if i + 1 < len(starts) else len(body)
        result[number] = body[start:end]
    return result


def bullet_block(section, name):
    """The non-empty body of a `- <name>:` block, or None."""
    lines = section.split("\n")
    out = []
    collecting = False
    for line in lines:
        if line.startswith(f"- {name}:"):
            collecting = True
            rest = line[len(f"- {name}:"):].strip()
            if rest:
                out.append(rest)
            continue
        if collecting:
            if line.startswith("- ") or line.startswith("### ") or line.startswith("## "):
                break
            out.append(line)
    text = "\n".join(out).strip()
    return text or None


def unit_addresses(section):
    return re.findall(r"^  - `src/game/([0-9A-F]{8})\.c` - manifest 0x([0-9A-F]{8}),",
                      section, re.M)


class PerformanceNotesTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.doc = DOC.read_text(errors="ignore")
        cls.body = body_of(cls.doc)
        cls.sections = sections(cls.doc)
        cls.manifest = {}
        for unit in json.loads(MANIFEST.read_text())["units"]:
            cls.manifest[unit["address"].upper().replace("0X", "")] = unit

    def test_document_exists_and_has_findings(self):
        self.assertTrue(DOC.is_file(), "docs/performance-notes.md missing")
        self.assertTrue(self.sections, "no findings in the document")

    def test_findings_are_numbered_without_gaps(self):
        numbers = sorted(int(n) for n in self.sections)
        self.assertEqual(numbers, list(range(1, len(numbers) + 1)))

    def test_every_finding_has_all_blocks(self):
        failures = []
        for number, section in sorted(self.sections.items(), key=lambda i: int(i[0])):
            for block in ("Status", "Mechanism", "Evidence", "Units", "Naming proposals"):
                if not bullet_block(section, block):
                    failures.append(f"finding {number}: empty {block} block")
        self.assertEqual(failures, [], "\n".join(failures))

    def test_status_is_one_of_the_documented_values(self):
        failures = []
        for number, section in sorted(self.sections.items(), key=lambda i: int(i[0])):
            status = bullet_block(section, "Status")
            value = status.split("\n")[0].strip().lower() if status else ""
            if value not in STATUSES:
                failures.append(f"finding {number}: status '{value}'")
        self.assertEqual(failures, [], "\n".join(failures))

    def test_units_exist_and_match_the_manifest(self):
        failures = []
        for number, section in sorted(self.sections.items(), key=lambda i: int(i[0])):
            listed = unit_addresses(section)
            if not listed:
                failures.append(f"finding {number}: no unit entry with address and size")
            for name, address in listed:
                if address != name:
                    failures.append(f"finding {number}: {name}.c claims manifest 0x{address}")
                if not (GAME / f"{name}.c").is_file():
                    failures.append(f"finding {number}: {name}.c does not exist")
                if name not in self.manifest:
                    failures.append(f"finding {number}: {name} is not in the manifest")
        self.assertEqual(failures, [], "\n".join(failures))

    def test_cited_lines_still_hold_that_code(self):
        rows = re.findall(r"^\| `((?:src/game|asm/us)/[^`]+?):(\d+)` \| `((?:[^`]|\\`)+)` \|$",
                          self.doc, re.M)
        self.assertTrue(rows, "citation index is empty")
        failures = []
        for path, line, fragment in rows:
            source = ROOT / path
            if not source.is_file():
                failures.append(f"{path}: no such file")
                continue
            lines = source.read_text(errors="ignore").splitlines()
            number = int(line)
            if number > len(lines) or fragment not in lines[number - 1]:
                failures.append(f"{path}:{line} no longer holds `{fragment}`")
        self.assertEqual(failures, [], "\n".join(failures))

    def test_every_body_citation_is_indexed(self):
        indexed = {(p, n) for p, n, _ in re.findall(
            r"^\| `((?:src/game|asm/us)/[^`]+?):(\d+)` \| `((?:[^`]|\\`)+)` \|$",
            self.doc, re.M)}
        failures = []
        for path, line in CITATION.findall(self.body):
            if (path, line) not in indexed:
                failures.append(f"{path}:{line} cited but not in the index")
        self.assertEqual(failures, [], "\n".join(failures))

    def test_index_lists_every_finding_with_matching_status_and_count(self):
        body = self.body
        index_text = body.split("## Index", 1)[1].split("\n### ", 1)[0]
        rows = re.findall(INDEX_ROW, index_text, re.M)
        self.assertEqual(len(rows), len(self.sections), "index row count")
        for number, title, status, count in rows:
            section = self.sections.get(number)
            self.assertIsNotNone(section, f"index lists finding {number} which has no section")
            heading = section.split("\n", 1)[0]
            self.assertIn(title.strip(), heading, f"finding {number}: index title")
            status_text = bullet_block(section, "Status") or ""
            self.assertEqual(status_text.split("\n")[0].strip().lower(),
                             status, f"finding {number}: index status")
            self.assertEqual(len(set(unit_addresses(section))), int(count),
                             f"finding {number}: index unit count")


    def test_applied_renames_are_bound_in_their_unit(self):
        """Every row of the renames table names a symbol its unit really binds."""
        table = self.doc.split("## Renames applied with this document", 1)
        self.assertEqual(len(table), 2, "renames table missing")
        rows = re.findall(r"^\| `(func_[0-9A-F]{8})` \| `([A-Za-z_]\w*)` \| `(src/game/[0-9A-F]{8}\.c)` \|$",
                          table[1], re.M)
        self.assertTrue(rows, "renames table is empty")
        failures = []
        for retail, name, path in rows:
            source = (ROOT / path).read_text(errors="ignore")
            code = re.sub(r"/\*.*?\*/", "", source, flags=re.S)
            pattern = (r"\b" + re.escape(name) + r"\s*\([^;{}\n]*\)\s*__asm__\(\""
                       + re.escape(retail) + r"\"\)")
            if not re.search(pattern, code):
                failures.append(f"{path}: does not bind {retail} to {name}")
        self.assertEqual(failures, [], "\n".join(failures))


if __name__ == "__main__":
    unittest.main()
