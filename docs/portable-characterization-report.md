# Abschlussbericht: Portable Characterization Tests

Stand: 22. November 2026. Ausgangsbasis: 125 A1-/A2-Funktionen mit 690 Assertions (76/423 + 49/267). Die bereits vorhandenen Tests und `source`-basierten Erwartungen wurden erhalten. `src/game` wurde nicht geändert.

## Ergebnis und Zählweise

| Kennzahl | Nachgewiesener Stand |
|---|---:|
| Ausgeführte Originalfunktionen | **185** unterschiedliche Funktionen in den drei Leaf-Suites, frisch kompiliert, Map-geprüft und mit QEMU-Eintritts-Dispatch |
| Funktionen mit zugeordneten Assertions | **185**, insgesamt 1.201 Checks: A1 423, A2 267, Extra 511 |
| Portable Testfälle | **48** unabhängig initialisierte Fälle für **76 A1-Funktionen**, mit 230 Aufrufschritten und 364 Rückgabe-/Speicherprüfungen |
| Mutationsstichprobe, durch Assertions abgesichert | **10 Funktionen**, bei denen mindestens ein Mutant die zugeordnete Assertion auslöste |
| Neue Funktionen gegenüber A1/A2 | **60**, ohne Überschneidung mit den 125 Markern der bestehenden Leaf-Suites; nicht notwendigerweise alle gegenüber sämtlichen älteren Host-Harnesses neu |

Die 185 sind eine reproduzierbar nachgewiesene **Teilmenge**, keine vollständige projektweite Zählung. Die Zielmarke 250 ist damit **nicht erreicht**. Frühere `reach`-Zahlen sind mit diesem Map-Nachweis nicht vergleichbar und werden nicht addiert. Die `LK_FUNC`-Zählung ordnet Assertions anhand der Harness-Marker zu; einzelne Prüfungen können mehrere Felder auf einmal abdecken. Die 48 portablen Fälle sind Aufrufsequenzen, nicht 230 einzeln initialisierte Fälle. Jeder Fall erhält einen eigenen Prozess und initiale Speicherbereiche.

## Implementierung und Nachweise

- Format und Ablauf: [`portable-characterization.md`](portable-characterization.md), ausführbarer Validator `tools/portable_tests.py`, QEMU-Runner `tools/portable_qemu.py`, Vektoren und deren Generator in `tests/portable/`. A1-Erwartungen wurden aus den bestehenden Tests übertragen, nicht blind aufgezeichnet. Beim Replay werden Quelle, Signatur, Hash, Linker-Map, QEMU-Eintritt, Rückgaben und ausgewählte Speicherzellen geprüft.
- `tools/host_leaf_provenance.py` baut jede Suite frisch in einem temporären Verzeichnis, prüft die Symbolbindung im Linker-Map und beobachtet den Funktions-Eintritt mit QEMU `exec,nochain`. Für 185 von 185 stimmen `.text`-Bytes **und** `.text`-Relocations des Debug-Objekts mit denen des normal gebauten Objekts überein. `in_asm` allein belegt nicht das Retire einzelner Instruktionen.
- Neue Suite `tests/host/leaf_extra_harness.c`: 60 verschiedene Ziel-Units, 511 Prüfungen. Erwartungen: 7 `spec`, 10 `math`, 2 `libc`, 38 `semantic`, 3 `source`. Abgeleitete Semantik und synthetische Speicherzustände sind keine unabhängigen Retail-Messungen.
- Vektoren: A1 vollständig **hinsichtlich der 76 Funktionsadressen**, aber nicht notwendigerweise sämtliche alten Assertion-Konstellationen. 36 `spec`, 1 `math`, 10 `libc`, 15 `semantic`, 14 `source`. Das ist eine Verhaltenserfassung für die definierten Eingaben, keine Vollständigkeitsgarantie.

## Mutationen

Reproduzierbare Stichprobe von 24 einzelnen Änderungen in temporären Quellkopien: **18** durch Assertions erkannt (10 Funktionen), **2** durch Absturz erkannt (eine weitere Funktion), **4** überlebt, keine ungültig oder wegen Timeout. Abstürze werden ausdrücklich nicht als Assertionsnachweis gezählt. Zwei überlebte Mutanten betreffen `8001E318`, je einer `80019720` und `8001814C`; ihre Äquivalenz ist nicht abschließend belegt. Eine erkannte Mutation pro Funktion ist kein Beleg für vollständige Abdeckung. Die hier benannten JSON-Dateien unter `build/portable-evidence/` sind generierte lokale Artefakte, nicht eingecheckt.

## Prüfungen und Reproduktion

```sh
make check
make portable-check
mkdir -p build/portable-evidence
python3 tools/host_leaf_provenance.py leaf_a1 leaf_a2 leaf_extra --json build/portable-evidence/leaf-provenance.json
python3 tools/host_leaf_suite.py mutate leaf_a1 --unit 80015AF4 --unit 8008D5D0 --unit 800607B8 --per-unit 2 --timeout 30 --budget 360 --json build/portable-evidence/mutations-a1.json
python3 tools/host_leaf_suite.py mutate leaf_a2 --unit 80019720 --unit 8001814C --per-unit 2 --timeout 30 --budget 240 --json build/portable-evidence/mutations-a2.json
python3 tools/host_leaf_suite.py mutate leaf_extra --unit 80018818 --unit 80018580 --unit 8001E318 --unit 80060DD8 --unit 8008E61C --unit 80092634 --unit 800427A8 --per-unit 2 --timeout 30 --budget 240 --json build/portable-evidence/mutations-extra.json
```

Letzter tatsächlicher Lauf: `make check` erfolgreich mit **226 Python-Tests**, Leaf-Suites 76/49/60 und Matching-Bericht **1.771/1.771 Funktionen, 555.576/555.576 Code-Bytes**. `make portable-check`: 8 Validator-Tests und 48/48 QEMU-Fälle erfolgreich. Provenance: 185/185 Map und Eintritts-Dispatch, 185/185 Debug-Code samt Relocations gleich. 1.772 C-Units und 1.771 Matching-Funktionen sind unterschiedliche Grundgesamtheiten. `make check` überspringt bestimmte Host-Suites bei fehlenden Voraussetzungen; der gemeldete Lauf führte die drei Leaf-Suites aus. `portable-check` überspringt seine QEMU-Ausführung nicht.

## Offen

- 49 A2-Funktionen und 60 Extra-Funktionen besitzen noch keine portablen Vektoren. Der Codex-Expertenlauf hinterließ nur den A1-Prototyp und keinen vollständigen Abschlussbericht; Runner und Speicherprüfung wurden anschließend integriert und real getestet. Weitere Expertenstarts wurden nicht unternommen.
- `retail`, Mock-Aufrufprotokolle und eigenständige Seiteneffekte unterstützt v1 nicht; `source` bleibt gültige Characterization. Keine separat am Retail-Binary gemessenen Erwartungswerte behaupten.
- Noch kein .NET-Runner oder xUnit-Test; `dotnet` war hier nicht im PATH. Nur das JSON- und Beobachtungsformat sind für eine spätere Umsetzung vorbereitet.
- Nicht beobachtete Bytes, Pointer-Aliasing außerhalb modellierter Regionen, übergebene dynamische Hardware-Adressen, undefiniertes C-Verhalten und unbekannte Initialzustände sind kein Beweis für kompatibles Gesamtverhalten. Global-Fixtures sind synthetisch. Nicht jede als `semantic` oder `math` benannte Herleitung wurde unabhängig fachlich verifiziert.
- Die generelle Reach-/Coverage-Infrastruktur wurde nicht vollständig auf Linker-Maps umgestellt; die stärkere Zuordnung gilt für die drei geprüften Leaf-Suites und den separaten Portable-Runner. Zeilen- und Branch-Coverage für das Gesamtprojekt wurden nicht neu belegt.
