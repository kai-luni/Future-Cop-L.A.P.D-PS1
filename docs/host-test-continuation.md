# Host- und portable Tests: Fortsetzung

Stand: 22. November 2026. Der [frühere Bericht](portable-characterization-report.md) enthält den Ausgangsstand und weitere Grenzen. Originaler Matching-Code in `src/game` blieb unverändert.

## Ergebnis

Fünf Leaf-Suites testen **250 unterschiedliche Originalfunktionen** mit zusammen **1.660 zugeordneten Checks**: A1 76/423, A2 49/272, Extra 60/511, More 53/276 und Final 12/178. Für alle 250 belegt ein frischer Provenance-Lauf Linker-Map-Bindung an das jeweilige Originalobjekt und QEMU-Eintritts-Dispatch; Debug-Objekte stimmen bei `.text` und dessen Relocations überein. Die Zählung ist eine geprüfte Teilmenge, keine gesamte Projekt-Reach-Zahl. Das Ziel von mindestens 250 ist erreicht.

Die neue Suite `leaf_more` verwendet synthetische Daten, keine Retail-Fixtures. Ihre 53 Funktionen sind nicht in den anderen drei Leaf-Suites enthalten. Evidenz: 11 `math`, 22 `semantic`, 20 `source`. Letzteres bezeichnet quelltextbasierte Charakterisierung, keine unabhängige Retail-Messung; die Labels sind keine formale Vollständigkeitsgarantie. Die Suite ist Teil von `make check`, wird wie die bestehenden Leaf-Suites aber bei fehlendem QEMU oder Compiler übersprungen.

Die Suite `leaf_final` ergänzt zwölf weitere unterschiedliche Funktionen mit synthetischen Zuständen, ohne Funktionsmocks oder Retail-Fixtures. Evidenz: 1 `math`, 2 `semantic`, 9 `source`. Sie ist ebenfalls in `make check` integriert und wird bei fehlendem QEMU oder Compiler übersprungen. Bei `8003CDAC` liefert das ausgeführte MIPS-Objekt im Range-Modus für einen negativen Index 1; der Quellkommentar beschreibt dafür 0. Das ist eine Charakterisierung dieses Compilers und der Testeingaben, kein unabhängiger Retail-Nachweis.

Die portablen A1/A2-Vektoren decken nun **125 Funktionsadressen** in **86 Fällen** ab: 417 Schritte, 10.416 Aufrufe und 628 Rückgabe-/Speicherprüfungen. Viele Aufrufe entfallen auf eine wiederholte Zufallssequenz; `repeat` prüft nur deren letzten Zustand. Die ursprünglichen A1-Datensätze bleiben unverändert. Nicht alle früheren Assertion-Konstellationen sind portabel; Extra, More und Final sind noch nicht übertragen. Das Format erfasst nur ausdrücklich angegebene Beobachtungen, keine automatischen vollständigen Seiteneffekt- oder Mockprotokolle. Für die Tabellenfunktion `8008E120` verwenden die portablen Tests zulässige synthetische Regionszeiger statt der im C-Harness benutzten Zahlen. Die zusätzlichen Rückgabe-/Speicherprüfungen für `80022C68` und `80098F48` sind in `tests/portable/export_fixtures.py` nachvollziehbar.

## Qualität und Grenzen

- Zwei zuvor überlebende Mutationen in A2 werden nun mit neuen Grenzfällen erkannt: LCG-Seed 0 (`80019720`) und doppelte sortierte Tabellenwerte (`8001814C`). Der zweite Fall charakterisiert einen synthetischen Zustand, dessen Vorkommen im Spiel nicht belegt ist. Alle vier erneut untersuchten Mutationen dieser beiden Funktionen wurden durch Assertions erkannt.
- Die `leaf_more`-Stichprobe: zehn Mutanten in fünf Funktionen; sieben durch Assertions erkannt, einer durch Absturz, zwei überlebt (`800694C8`). Dort liefern die geänderten Grenzen 0 und 54 nach dem Clamping denselben Ausgabewert; die Stichprobe belegt keine vollständige funktionale Äquivalenz. Die ausgewählte Unit `80019FCC` lieferte mit den verwendeten Operatoren keine Mutation. Abstürze sind kein Assertionsnachweis.
- Die vollständige automatische Stichprobe für `leaf_final`: 30 Mutanten, 23 durch Assertions erkannt, fünf Abstürze und zwei überlebt (`80035184`, `8003B44C`); diese Überlebenden sind nicht als allgemein äquivalent bewiesen. Abstürze sind kein Assertionsnachweis.
- Der Export der portablen Vektoren wird nun byteweise gegen die eingecheckte JSON-Datei geprüft.
- Der Provenance-Runner meldet nun bei unterschiedlichen Debug-Objekten oder wiederholten Funktionen zwischen Suites einen Fehler statt eines erfolgreichen Exit-Codes; beide Fehlerfälle haben Regressionstests.
- Map und Dispatch belegen nicht die Ausführung jeder einzelnen Instruktion oder sämtliche möglichen Eingaben. Die Assertions sind durch Harness-Marker zugeordnet; `source` und synthetische Zustände sind keine unabhängig gemessene Retail-Spezifikation.

## Reproduktion

```sh
make check
make portable-check
mkdir -p build/portable-evidence
python3 tools/host_leaf_provenance.py leaf_a1 leaf_a2 leaf_extra leaf_more leaf_final --json build/portable-evidence/continued.json
python3 tools/host_leaf_suite.py mutate leaf_a2 --unit 80019720 --unit 8001814C --per-unit 2 --timeout 30 --budget 240 --json build/portable-evidence/a2-boundary-mutants.json
python3 tools/host_leaf_suite.py mutate leaf_more --unit 80019FCC --unit 8001C288 --unit 80017D5C --unit 800219EC --unit 800694C8 --unit 800821EC --per-unit 2 --timeout 30 --budget 300 --json build/portable-evidence/more-mutants.json
python3 tools/host_leaf_suite.py mutate leaf_final --per-unit 3 --timeout 30 --budget 600 --json build/portable-evidence/final-mutants.json
```

Letzter Lauf: `make check` erfolgreich, 228 Python-Tests und 1.771/1.771 Matching-Funktionen; alle fünf Leaf-Suites liefen tatsächlich. `make portable-check` erfolgreich einschließlich neun Format-Tests und echtem Replay aller 86 Fälle. Der Provenance-Befehl meldete 250/250. Ein unabhängiger Replay- und Vergleichslauf war ebenfalls erfolgreich. Alle Build- und Mutationsartefakte sind lokal und nicht Teil des Repositorys.
