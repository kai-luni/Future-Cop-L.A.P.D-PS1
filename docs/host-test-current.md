# Host-Tests: aktueller Stand und Grenzen

> **Nachtrag 22.11.2026:** Der aktuelle geprüfte Stand einschließlich der neuen Leaf-Suite und portablen A2-Vektoren steht in [`host-test-continuation.md`](host-test-continuation.md). Die folgenden Messwerte beschreiben den früheren Ausgangsstand.

## Ausgeführte Originalfunktionen

Die beiden neuen Leaf-Suites kompilieren unveränderte `src/game`-Units mit der
MIPS-Toolchain und führen sie unter QEMU aus:

| Suite | Funktionen | Assertions |
|---|---:|---:|
| A1 (ohne globale Daten) | 76 | 423 |
| A2 (explizite synthetische Daten) | 49 | 267 |
| Gesamt | 125 | 690 |

Die Suites drucken eine Zuordnung und Assertionzahl je Funktion. Die Erwartung
von 89 Funktionen ist als `spec`, `math`, `libc` oder `semantic` gekennzeichnet;
36 weitere sichern lediglich den aus dem Quelltext abgeleiteten Ist-Zustand
(`source`), nicht die ursprüngliche Absicht. `semantic` bezeichnet von Hand aus
dem vermuteten Zweck abgeleitete Erwartungen und ist nicht automatisch eine
unabhängige Spezifikation. Siehe `tests/host/LEAF_SUITES.md` und die Marker in
den Harnesses für die konkrete Zuordnung und ihre Begründung. Bei A2 sind die
Daten Testfixtures, kein Nachweis des Retail-Anfangszustands.

Der historische Vergleich gegen `/tmp/reach_final.json` ergab 125 zuvor dort
nicht erfasste Funktionen. Die alte Gesamtzahl 116 ist **nicht** als Baseline
für Originalfunktionen geeignet: Sie enthielt auch gelinkte Mocks.

## Geschätzte Reichweite und Messgrenzen

Der nachfolgende Ablauf zeichnet die wirklichen Testaufrufe mit einem frischen
Trace-Verzeichnis auf. `env_test.sh` ist bereits Teil von `make check`.
`TMPDIR` muss auf ein vorhandenes Verzeichnis zeigen.

```sh
TRACE_DIR="$(mktemp -d "${TMPDIR:-/tmp}/fc-traces.XXXXXX")"
QEMU_TRACE_DIR="$TRACE_DIR" QEMU="$PWD/tests/host/qemu_trace.sh" make check
QEMU_TRACE_DIR="$TRACE_DIR" make reach JSON="${TMPDIR:-/tmp}/fc-reach.json"
make coverage
```

Die Messung erfasste 200 `func_`-Symbole mit jeweils einem gleichnamigen
adressbenannten Objekt in 21 Binaries; davon waren 195 in den verwendeten
Traces erreichbar, fünf nicht nachgewiesen. Ob das jeweilige Originalobjekt
tatsächlich gelinkt wurde, ist nicht abschließend geprüft (siehe unten).
Sieben der 195 sind Assembler ohne Quellzeileninformation. Für die erfassten
Funktionen wurden 1527 von 1534 instruktionszugeordneten Quellzeilen als
ausgeführt gemeldet (99,5 %); für die fünf Funktionen ohne verwendbaren Trace
sind keine Zeilen enthalten.
Dies ist **keine Gesamtprojekt-Coverage** und keine Branch-Coverage. Zeilen
können in einzelnen Läufen auch nur teilweise ausgeführt sein.

Die Messung berücksichtigt ein Original nur, wenn ein adressbenanntes Objekt
im Build-Verzeichnis dessen `func_`-Symbol exportiert; anders benannte Originale
werden ausgelassen. Fehlt ein solches Objekt, wird ein gleichnamiger Mock nicht
als Original gezählt. Das vorhandene Objekt allein beweist aber noch nicht, dass
es tatsächlich in das Binary gelinkt wurde: Ein liegengebliebenes Objekt und
ein gleichnamiger Mock können einen falsch positiven Befund erzeugen. Ein
Linker-Map-Nachweis oder ein Gleichheitsvergleich fehlt bisher. Die Trace-Dateien
werden anhand des vollständigen Binary-Pfads getrennt; für jede Messung ist ein
frisches Verzeichnis nötig, damit ältere Läufe nicht mit neuen vermischt werden.
Ein fehlender/veralteter Trace ist
kein positiver Ausführungsnachweis. Manche Harnesse überschreiben die `QEMU`-
Variable, sodass ihre Ausführung in dieser Messung nicht aufgezeichnet wird;
fehlender Nachweis bedeutet deshalb nicht, dass die Funktion nie ausgeführt
wurde. Die Assembler-Reichweite nutzt die tatsächliche Symbolgröße statt eines
pauschalen Adressfensters. Die Zuordnung der Debug-Offsets setzt identischen
Maschinencode der Debug- und Testobjekte voraus; die Messung prüft diese
Gleichheit bislang nicht automatisch. Die Reichweite ist daher eine konservativ
gefilterte Beobachtung, kein vollständiger formaler Äquivalenzbeweis.

`make check` bestand mit 220 Python-Tests; zusätzlich bestanden
die Host-Harnesses einschließlich der beiden Leaf-Suites. Die getrennte
`make coverage`-Messung folgt ihren eigenen Unit-Listen und ist ebenfalls keine
Gesamtprojekt-Coverage.

## Mutationsstichprobe

Der A1-Lauf veränderte 152-mal jeweils eine Unit in einer temporären Kopie:
134 Mutationen führten zu fehlschlagenden Assertions, 13 zu Abstürzen, eine
kompilierte nicht und vier überlebten. Für diese vier wurde Äquivalenz als
Arbeitshypothese beurteilt, nicht formal bewiesen. Der Lauf führte zu zwei
zusätzlichen Tests für `8008D5D0`; das ursprüngliche Ergebnis wurde danach
nicht vollständig neu erhoben. A2 wurde nur stichprobenartig untersucht. Ein
Timeout wird jetzt auf die eigene Prozessgruppe begrenzt, sodass fremde
QEMU-Prozesse nicht beendet werden.

## Mock- und Datengrenzen

`python3 tests/host/mock_inventory.py` zählt 65 erkannte Definitionen, 50
Retail-Labels und 12 Labels mit Definitionen in mehreren Dateien. Das ist eine
regexbasierte Inventur, keine vollständige C-Analyse. Nicht alle alten Mocks
wurden zentralisiert oder semantisch validiert. Insbesondere:

- `func_8001006C`: drei genutzte Argumentregister; `func_80010820`: eines.
  Die No-op-Mocks in `tests/host/stubs.c` wurden entsprechend korrigiert.
- `func_8006B23C`: `void`; der Cadence-Mock gibt keinen Wert mehr vor.
- `func_8006ACB0`: Der lokale Cadence-Stub liefert bewusst einen kontrollierten
  Erfolg und zeichnet den Aufruf auf; er simuliert die Funktion nicht allgemein.
- `func_80019668`: Der konstante Form-Harness und der LCG im Naming-Harness
  sind unterschiedliche Test-Fixtures, kein validierter Ersatz der Retail-PRNG.
- `func_80046C84`: Die überprüfte C-Definition erwartet zwei 32-Bit-Wörter und
  liefert einen `s32`; lokale Recorder und der gemeinsame Sound-Mock haben
  unterschiedliche Aufzeichnungs- und Rückgabeverträge. Ihre Semantik ist
  nicht automatisch austauschbar.

Der GPU-Mock zählt nun auch Aufrufe jenseits des begrenzten Logs; ein neuer
MIPS-Selbsttest prüft Zahl und Overflow-Flag. Hardware selbst wird dadurch nicht
emuliert.

Ein `D_`-Name beweist weder Größe, Typ, Initialwert noch gültiges Zeigerziel.
Die `host_test_buildability.py`-Nullslots sind nur Hilfen für Linkproben, keine
Laufzeitfixtures. `tools/host_test_evidence.py` prüft PS-X-EXE-Bereiche anhand
Header und Dateigrenzen; `image-zero` bedeutet ausschließlich, dass die konkret
gelesenen Imagebytes null sind, nicht dass ein beliebiges Symbol BSS ist.

## Hardwareklassifikation und Exportlücken

`python3 tools/host_hw_classification.py --json <datei>` kombiniert bestehende
Disassemblierungsdaten und Graphkanten mit Symbolen, Relocations und
Instruktionsscans kompilierter Objekte. Der Lauf für 1772 C-Units ergab 903
`candidate`, 762 `hw` und 107 `unknown`; 111 Units haben direkte erkannte
Hardwaremerkmale. `candidate` heißt ausschließlich: im vorhandenen Modell ist
kein Hardwarepfad und keine erkannte Aufruflücke bekannt. Verdeckte
Abhängigkeiten und indirekte Hardwarezugriffe können weiterhin unentdeckt
bleiben. Die Kategorie ist ausdrücklich kein Hardwarefreiheitsbeweis.

133 Objekte haben insgesamt 455 unterschiedliche `func_`-Objektabhängigkeiten,
die nicht als direkte Graphkante vorliegen; diese Zahl ist nicht identisch mit
der ursprünglichen Untersuchung von 89 Units mit Out-Degree-Lücken. 66 der
historisch als `clean` gezählten Units fallen jetzt unter `hw`, vier unter
`unknown`. Von den 908 historischen Nullslot-Linkerfolgen fallen zehn unter
`hw` und zwei unter `unknown`: Linkerfolg ist kein Ausführungsnachweis.

Vier Units exportieren nicht das erwartete `func_`-Label. Bei `80096748` wird
stattdessen `gDivModUnsigned64` exportiert; 17 Units sind im Modell transitiv
davon abhängig. `8007D138` exportiert ein Datenlabel `D_8007D138`, kein
Funktionslabel; der Graph weist dafür keinen transitiven Aufrufer aus. Auch
`80095860` und `80096520` exportieren andere Namen. Diese Diagnosen ändern den
Matching-Code nicht und beweisen nicht, dass ein Alias für alle Aufrufkontexte
korrekt wäre. Die Analyse verwendet einen mtime-basierten Objektcache: Nach
Änderungen an Headern oder Toolchain mit `--force-compile` neu bauen.

## Offen

Der historische Aufrufgraph enthält nachweislich Lücken, etwa
`func_80013AB4` → `func_800181D4`. Die ältere `clean=969`-Klassifikation ist
kein Beweis für Hardwarefreiheit. Linkerfolg mit künstlichen Nullslots ist
kein Ausführungsnachweis. Indirekte Aufrufe, Datentypen und Laufzeitinitialisierung
müssen pro Kandidat weiter belegt werden. Eine vollständige Emulatorumgebung,
große Hardware-Nachbildungen und größere Funktionsketten bleiben zurückgestellt.
