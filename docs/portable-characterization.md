# Portable Characterization Tests, Version 1

Stand: 22. November 2026. Die JSON-Datei `tests/portable/a1-a2.v1.json` enthält handübertragene Assertions für alle A1- und A2-Funktionen. `tests/portable/export_fixtures.py` erzeugt sie reproduzierbar; die Erwartungswerte werden nicht aus dem QEMU-Ergebnis übernommen. Die bestehenden C-Assertions bleiben bestehen. Extra, More und Final sind noch nicht portiert.

## Format

Die ausführbare, streng validierende Formatspezifikation ist `tools/portable_tests.py` (`validate`); unbekannte Felder, doppelte Schlüssel und ungültige Typen werden abgelehnt. Formatname `fc-portable-tests`, Version `1`, Byte-Reihenfolge `little`.

- `functions`: Schlüssel `func_XXXXXXXX`; Quellpfad und SHA-256, Argument- und Rückgabetypen sowie `origin` mit Suite, Belegklasse und Erläuterung. Die Adresse ist Teil des Schlüssels. Die Quelldeklaration wird bei `--sources` geprüft.
- `cases`: voneinander unabhängig initialisierte Fälle. `id` ist eindeutig. Jeder Speicherbereich in `regions` hat eine ID, Größe, initialen `u8`-Füllwert und optional ein globales `D_XXXXXXXX`-Symbol. Bereiche sind logisch getrennt; `ptr` ist `null` oder `{region, offset}`, keine Hostadresse.
- `steps` sind eine geordnete Aufrufsequenz. `arrange` beschreibt typisierte Speicherinitialisierungen vor diesem Schritt; `args` enthält typisierte Argumente; `repeat` gibt die Anzahl gleicher Aufrufe an. `return` und `memory` sind erwartete Beobachtungen nach dem letzten Aufruf. Ein `void`-Aufruf besitzt mindestens eine Speicherbeobachtung. Wiederholungen liefern keine Zwischenzustände; diese bei Bedarf als getrennte Schritte ausdrücken.
- Speicherzellen haben `region`, Byte-`offset` und einen typisierten `value`. `bytes` verwendet eine gerade Anzahl hexadezimaler Kleinbuchstaben. Andere Typen: `s8`, `u8`, `s16`, `u16`, `s32`, `u32`, `ptr`, `void`. Integerwerte sind **kanonische dezimale Zeichenketten** im jeweiligen Wertebereich; ihre Breite und Signedness bestimmen das Bitmuster. C# muss z. B. für bitweise Konversionen `unchecked` und die angegebene Breite verwenden. Die Zellen werden little-endian gespeichert. `void` hat `null` als Wert.
- `source` ist eine wertvolle, aus Matching-C abgeleitete und hier durch Ausführung bestätigte Verhaltenserwartung; `math`, `spec`, `libc` und `semantic` kennzeichnen andere Herkunft. Keine Kategorie bedeutet automatisch Retail-Messung oder vollständige Spezifikation. Derzeit existiert kein `retail`-Label.

Der Vergleich erfasst nur ausdrücklich angegebene Speicherzellen und Rückgaben, nicht automatisch sämtliche Änderungen. Mockprotokolle und sonstige Seiteneffekte sind in v1 **noch nicht** als eigenes Feld implementiert; sie dürfen daher nicht als geprüft gelten. A2-Vektoren verwenden synthetische globale Bereiche und Pointer. Alignment der QEMU-Bereiche ist 4 Byte; Pointer-Offsets müssen zur jeweiligen konkreten Funktion passen. Nicht unterstützte Signaturen werden abgewiesen. Struct-Offsets müssen explizit als Zellen modelliert werden. Unbekannte Laufzeitinitialisierung, C-Undefined-Behavior und Hardwareeffekte sind kein verifiziertes Retailverhalten. Für Zufallsgeneratoren nur explizit gesetzte globale Zustände und geeignete Folgen verwenden.

## Ausführung

Voraussetzung sind die im Projekt vorhandene MIPS-Toolchain und `qemu-mipsel`, kein neues Python-Paket. Im Projektverzeichnis:

```sh
make portable-check
python3 tools/host_leaf_provenance.py leaf_a1 leaf_a2 leaf_extra leaf_more leaf_final --json build/portable-evidence/provenance.json
```

`portable-check` validiert Quellen, führt die Format-Tests aus und erstellt für jeden Fall einen separaten MIPS-Treiber. Er kompiliert die konkrete `src/game`-Unit frisch, prüft Quellhash und Signatur, Linker-Map und `func_`-Definition, startet QEMU mit `exec,nochain` und prüft die Eintritts-Dispatches sowie die tatsächlichen Rückgaben und angegebenen Speicherzellen gegen die übernommenen Erwartungen. Fehlende Voraussetzungen oder Abweichungen sind Fehler. Zur dauerhaften Prüfung und zur Ausgabe eines Vergleichsformats:

```sh
python3 tools/portable_tests.py replay tests/portable/a1-a2.v1.json --artifacts build/portable-run --observed build/portable-observed.json
python3 tools/portable_tests.py compare tests/portable/a1-a2.v1.json build/portable-observed.json
```

`--artifacts` verlangt ein noch nicht existierendes Verzeichnis und enthält ELF, Linker-Maps, QEMU-Traces und `proof.json`. Ein einzelner Fall kann gegenwärtig durch Erstellen einer gültigen Teil-JSON mit genau diesem Fall und den verwendeten Funktionsdeklarationen erneut ausgeführt werden; ein `--case`-Schalter fehlt. `proof.json` beweist die Ausführung des konkreten Laufs, nicht alle möglichen Eingaben. Die Debug-Objekt-Gleichheit bezieht sich separat auf `.text` und dessen Relocations, nicht auf komplette Objektdateien.

Für .NET: JSON mit `System.Text.Json` lesen, pro Fall Bereiche und ggf. globale Daten frisch anlegen, die Schritte in Reihenfolge mit expliziten Breiten, Alignment und little-endian Speicher ausführen und dasselbe Beobachtungsformat `fc-portable-observations` Version 1 ausgeben. `compare` prüft Rückgaben und angegebene Speicherzellen. Das lässt sich später in xUnit einbinden; ein .NET-Runner ist noch nicht implementiert oder getestet.

## Abgrenzung

Keine Retail-Bytes wurden als Fixture aufgenommen. Die v1-Fälle verifizieren die MIPS-Ausgabe zu ihren Eingaben, keine formale Gleichheit aller Laufzeitumgebungen. MIPS-Dispatch, Map und Assertions beziehen sich auf QEMU und den frisch kompilierten Matching-C-Code. Die separate `in_asm`-Zeilenmetrik druckt übersetzte Blöcke und ist kein Beweis für jede tatsächlich ausgeführte Instruktion oder Branch-Coverage.
