# Host-Testabdeckung Future Cop L.A.P.D.: Ausbau, Messung und Befund

> **Historischer Bericht, teilweise widerlegt.** Die Angaben 116 ausgeführte
> Originalfunktionen und 969 hardwarefreie Units waren überhöht: Die alte
> Reichweitenmessung zählte auch Mocks; der Aufrufgraph ist unvollständig.
> 908 erfolgreiche Links mit künstlichen Nullslots beweisen weder korrekte
> Daten noch Hardwarefreiheit oder Ausführbarkeit. Aktuelle, konservative
> Messungen und ihre Grenzen stehen in `docs/host-test-current.md`.

Bericht über die Erweiterung der Testinfrastruktur. Alle Zahlen sind gemessen,
nicht geschätzt; die Befehle zum Nachrechnen stehen in jedem Abschnitt.

- Repository: `kai-luni/Future-Cop-L.A.P.D-PS1` (Fork von `future-cop-lapd-ps1`)
- Branch: `main`, drei Commits auf `origin/main`
- `0f4bd07` — Test-Kit und Terrain-Test
- `986e47a` — Mock-Bibliothek, ISO-9660-Test, Messwerkzeuge
- `fe49db9` — Korrektur des Abdeckungsberichts
- Stand verifiziert mit `make check`: **Exit 0, 192 Python-Tests, 14 Harness-Targets grün**

---

## 1. Ergebnis in Kurzform

| Kennzahl | vorher (`e240016`) | nachher | |
|---|---:|---:|---|
| ausgeführte `func_`-Units | 100 | **116** | +16, keine verloren |
| Anteil an 1772 C-Units | 5,6 % | **6,5 %** | |
| Testbinaries | 17 | 20 | |
| gemessene Quellzeilen | 1567 | 1705 | |
| davon ausgeführt | 1478 (94,3 %) | **1607 (94,3 %)** | |
| hardware-freie Units | 155 (Behauptung) | **969 (gemessen, 54,7 %)** | Faktor 6,2 |
| davon ohne jeden Mock baubar | unbekannt | **908 (93,7 %)** | |

Die wichtigste Zahl ist die letzte. Sie war vorher nicht erhoben worden und
verschiebt die Einschätzung, was überhaupt erreichbar ist, grundlegend.

---

## 2. Die Ausgangsbehauptung war falsch

Der Bericht `docs/test-coverage-report.md` nannte drei Zahlen, die alle aus einer
einzigen fehlerhaften Klassifizierung stammten:

| Behauptung | gemessen | Ursache der Abweichung |
|---|---|---|
| 155 hardware-freie Units (8,7 %) | **969 (54,7 %)** | Klassifizierung über den `// Categories`-Kommentar |
| harte Obergrenze 10,8 % | keine Grenze | daraus abgeleitet |
| realistischer Zielkorridor 15–25 % | 54,7 % hardware-frei | daraus abgeleitet |

Der `// Categories:`-Kopfkommentar einer Unit nennt häufig GTE, auch wenn die
Funktion cop2 nie berührt — etwa bei Units, die nur eine Matrix als `u32`-Wort
weiterreichen. Eine Textsuche zählt diese als Hardware-Abhängigkeit. Die
tatsächliche Zugriffssignatur steht im Disassembly und ist in
`analysis/analysis.json` als `sig`-Block bereits vorhanden:

```
sig: { args, arg_mask, returns, leaf, gte, syscall, mmio, stack_frame, saved,
       confident, prototype }
```

Die Klassifizierung erfolgt jetzt ausschließlich über diese Signaturen
(`tools/host_test_candidates.py`), nicht über Quelltext.

### Der Fixpunkt statt einer Ein-Pass-Zählung

Eine transitive Klassifizierung muss konvergieren. Drei Zustände:

- **`clean`** — kein Hardware-Zugriff in der gesamten Callee-Closure.
- **`hw`** — erreicht GTE, MMIO, BIOS oder einen Syscall.
- **`unknown`** — unaufgelöster indirekter Aufruf, oder `out_degree` passt nicht
  zu den bekannten Kanten.

`unknown` ist hier bewusst kein Freibrief: eine fehlende Kante wird **nie** als
Hardware-Freiheit gelesen. Nur so bleibt die Zahl belastbar.

### Ergebnis der Klassifizierung über alle 1772 C-Units

| Klasse | Units | Anteil |
|---|---:|---:|
| `clean` | 969 | 54,7 % |
| `hw` | 641 | 36,2 % |
| `unknown` | 162 | 9,1 % |

Direkten Hardware-Zugriff haben nur **90 Units** (5,1 %):

| Zugriffsart | Units |
|---|---:|
| BIOS-Aufruf | 46 |
| cop2/GTE | 39 |
| MMIO (SPU/CD/GPU/DMA) | 3 |
| Syscall | 2 |

Ein MMIO-Anteil von nur drei Units ist der überraschendste Einzelbefund: der
Großteil der Hardware im Spiel läuft über BIOS-Vektoren und GTE, nicht über
direkte Registerzugriffe. Genau deshalb ist ein Mock an dieser Grenze so
wirksam.

### Entfernung zur nächsten Hardware-Unit

Kürzeste Pfadlänge im Aufrufgraphen, gemessen per BFS über die inversen Kanten:

| Hops | Units | Anteil |
|---|---:|---:|
| 0 (greift selbst zu) | 90 | 5,1 % |
| 1 | 209 | 11,8 % |
| 2 | 176 | 9,9 % |
| 3 | 79 | 4,5 % |
| 4 | 43 | 2,4 % |
| 5 | 28 | 1,6 % |
| 6 | 13 | 0,7 % |
| 7+ | 3 | 0,2 % |
| **kein Pfad zur Hardware** | **1131** | **63,8 %** |

Der alte Bericht hatte hier 1617 transitiv abhängige Units; tatsächlich sind es
641. Die Mehrheit der Codebasis ist nicht durch Hardware blockiert, sondern war
nur falsch gezählt.

---

## 3. Die echte Decke: was ohne Mock baut

Klassifizierung ist eine Aussage über den Aufrufgraphen, nicht über den Build.
Eine `clean`-Unit kann trotzdem scheitern, weil gcc 2.7.2 die Quelle ablehnt
oder ein Datensymbol fehlt. `tools/host_test_buildability.py` beantwortet daher
die eigentliche Frage: **lässt sich die Unit so, wie sie dasteht, zu einem
lauffähigen MIPS-Binary linken?**

Verfahren pro Unit:

1. Unit kompilieren wie ein Harness (`-O1 -G0`, maspsx, `fix_align.py`, `as`).
2. Die Closure aus den **undefinierten Symbolen der Objekte** auflösen — nicht
   aus `analysis/edges.json`.
3. Verbleibende `D_`-Symbole als künstlich genullte Slots für die reine
   Linkprobe binden. Diese Slots belegen weder Größe noch Initialzustand.
4. Gegen `tests/host/crt0.s` linken, `-static -e __start`.

### Ergebnis über alle 969 clean-Units

| Zustand | Units | Anteil |
|---|---:|---:|
| **`ok` — linkt ohne jede Attrappe** | **908** | **93,7 %** |
| `link` — braucht einen Stand-in | 61 | 6,3 % |
| `compile` — gcc/maspsx/as lehnt ab | 0 | 0,0 % |
| `missing` — keine Quelldatei | 0 | 0,0 % |

**Keine einzige clean-Unit scheitert am Compiler.** Die 969 Units sind alle
übersetzbar; was fehlt, ist nur gelegentlich ein Symbol.

Struktur der Ketten:

| Kennzahl | Wert |
|---|---|
| Blätter (Kette = 1 Objekt) | 506 |
| Median Kettengröße | 1 |
| größte `ok`-Kette | 58 Objekte |
| tiefste beobachtete Closure | 134 Objekte |
| verschiedene gebundene `D_`-Symbole | 672 |
| gebundene Slots insgesamt | 5442, davon **0** Nicht-`D_` |

Die 506 Blätter sind der billigste Vorrat: eine Unit, ein Objekt, keine
Kette. Die großen Ketten sind dagegen ein Argument *für* diese Technik — eine
einzige Testausführung zieht bis zu 58 echte Funktionen mit.

### Warum `D_`-Slots zulässig sind und `func_`-Slots nicht

Ein `D_`-Symbol bezeichnet Daten, aber sein Initialwert und seine Größe folgen
nicht aus dem Namen. Die künstlichen Null-Slots dienen ausschließlich der
Linkprobe; für Ausführung brauchen Tests passende Fixtures. Eine Funktion als
Integer-Slot zu binden kann dagegen irreführend linken; das verursachte zuvor
die falsche Zahl **969/969 ok**. `make_bindings()` verweigert Nicht-`D_`-Symbole.

### Die 61 Blocker nach Ursache

| Ursache | Units | Bedeutung |
|---|---:|---|
| ruft eine Hardware-Unit | 47 | braucht einen Stand-in — der erwartete Frontier |
| ruft eine `unknown`-Unit | 23 | Callgraph-Lücke, nicht Hardware |
| Namenskonflikt in der Quelle | 5 | Defekt in `src/`, siehe §4 |
| keine Quelldatei | 1 | `func_80086F30` |

Ein Toolchain-Race, der zunächst zwei weitere Fehlschläge erzeugte
(`cannot exec 'cc1': Text file busy` bei parallelem gcc), ist behoben:
`run_retry()` wiederholt diesen Startfehler. Beide Units einzeln nachgeprüft:
`ok`.

---

## 4. Zwei Defekte, die die Messung aufgedeckt hat

### 4.1 Vier Units exportieren ihren eigenen Retail-Namen nicht

Von 1772 Units exportieren 1768 korrekt `func_<eigene Adresse>`. Vier nicht:

| Unit | Objekt exportiert | Klasse |
|---|---|---|
| **80096748** | `gDivModUnsigned64`, `__clz_tab` | clean |
| **8007D138** | `D_8007D138` (C-Name auf das *Daten*-Label aliasiert) | clean |
| 80095860 | `bios_A0_49_GPU_cw` | hw |
| 80096520 | `entrypoint` | hw |

`src/game/80096748.c:123` definiert

```c
UDItype gDivModUnsigned64(n, d, rp)
```

ohne `__asm__("func_80096748")`. Das Objekt exportiert damit
`gDivModUnsigned64`, jeder Aufrufer referenziert aber `func_80096748` — der
Link scheitert.

Kosten: 5 clean-Units sind dadurch blockiert, **3 davon ausschließlich**
(`800867B0`, `80086F58`, `800966B0`). Ein einzeiliger Alias hebt die Zahl von
908 auf 911.

`src/game/8007D138.c:78` definiert `void gConfirmSoundOptionsItem(s32 arg0)`,
während Zeile 58 denselben C-Namen auf `D_8007D138` aliasiert. Die Funktion wird
also unter dem Datenlabel emittiert. Das blockiert derzeit keine Unit, ist aber
dieselbe Fehlerklasse.

**Nicht angefasst.** `src/` ist Byte-Matching-Territorium; eine Änderung dort
berührt den Beweis der Übersetzungstreue und gehört nicht in einen Testauftrag.

### 4.2 Der Callgraph verliert direkte Aufrufe

`analysis/analysis.json` führt `func_80013AB4` als `leaf` mit `out_degree: 0`.
Der dekompilierte Rumpf ruft dreimal `func_800181D4`. Über alle Units betrifft
das **89 Units**, die clean-Funktionen aufrufen, welche in
`analysis/edges.json` fehlen.

Konsequenz für das Werkzeug: die Closure wird aus den undefinierten Symbolen der
Objektdateien aufgelöst, nicht aus der Kantenliste. Der Linker entscheidet, was
fehlt — das ist die belastbare Quelle.

```python
# tools/host_test_buildability.py, Builder.closure()
und = set()
for obj in objects:
    und |= undefined(obj, env)      # nm -u
und -= defined(objects, env)        # nm --defined-only
# jedes verbleibende func_-Symbol, das selbst eine clean-Unit mit Quelle ist,
# wird nachgeladen und erneut gelinkt
```

---

## 5. Neue Infrastruktur

### 5.1 Test-Kit — `tests/host/fc_testkit.h` (278 Zeilen)

Freestanding, kein libc. Der Grund: jeder Harness hatte bisher seine eigenen
Assertions, seine eigene `memcpy`, seine eigene Ausgabe.

| Baustein | Zweck |
|---|---|
| `fck_write`, `fck_write_int` | Ausgabe über den `write`-Syscall |
| `FCK_CHECK(cond, msg)` | boolesche Assertion, läuft bei Fehler weiter |
| `FCK_EQ(actual, expected, msg)` | Gleichheit, druckt beide Werte |
| `fck_report("<suite>")` | Bilanz, Rückgabe 0 nur wenn alles hielt |
| `fck_record`, `fck_calls_to`, `fck_call_n` | Aufruf-Recorder |
| `fck_expect_order(expected[], n, msg)` | prüft die ganze Aufrufreihenfolge |
| `fck_log_reset` | Recorder zwischen Fällen leeren |

`fck_report` schlägt fehl, wenn eine Suite **null** Checks ausgeführt hat. Ein
Harness, der stillschweigend nichts prüft, kann nicht bestehen.

### 5.2 Mock-Bibliothek — `tests/host/mocks/` (805 Zeilen, 8 Dateien)

Drei Gruppen, 20 Attrappen, jede per `asm`-Alias an das Retail-Label gebunden:

| Datei | Inhalt | Attrappen |
|---|---|---:|
| `sound_mock.{h,c}` | SE-Trigger, 4 Volume-/Modus-Getter, Stream-Start, Stream-Status | 7 |
| `gpu_mock.{h,c}` | `LoadImage` (Rect + Payload-Schatten), `DrawSync` (Modus-Log) | 2 |
| `bios_mock.{h,c}` | ein Dispatcher, echte A0-String-/Memory-Semantik, geordnetes Service-Log | 11 |
| `fcm_types.h` | `s8..u32` unter gemeinsamem Guard, Breitenprüfung | — |
| `fcm_report.h` | `fcm_say`/`fcm_abort` über das Syscall-Gate | — |

Die Bindung ist der entscheidende Punkt:

```c
/* Nur der BIOS-Service, den diese Unit tatsächlich ruft. */
void fckBiosPrintf(const char *fmt) __asm__("bios_A0_3F_printf");
```

Ein falscher Name ist damit ein **Linkfehler**, keine stillschweigend andere
Funktion. Verifiziert: ein absichtlich falsches Alias erzeugt
`undefined reference to func_80092F38`.

Vier Regeln, die einen Mock ehrlich halten:

1. **Signatur exakt der Retail-Prototyp.** 32-Bit-Wörter; ein abweichender
   Prototyp ist auf MIPS undefiniertes Verhalten, das zufällig funktioniert.
2. **Unkonfiguriert = 0**, strukturell über `fcm_*_reset()`. Ein Test, der auf
   einen Rückgabewert baut, muss ihn vorher setzen.
3. **Unbekannter BIOS-Vektor bricht laut ab** — `fcm_abort` mit Vektor und
   Service im Klartext, `exit(2)`. Nie `return 0`.
4. **Nur aufzeichnen, nicht erfinden.** `stream_starts` zählt Aufrufe; es
   behauptet nicht, der Stream laufe jetzt.

Eingebunden werden sie **pro Test**, nie global — keine Regel, kein `build.sh`
und nicht `fc_testkit.h` zieht sie herein.

### 5.3 Zwei echte neue Tests

**`terrain_surface` — 50 Checks, echte Zweier-Kette**

```
func_80067684  gQuerySurfaceHeight      (Bodenabfrage)
      |
func_80067480  gSampleTileHeight        (Bilinear-Interpolation)
```

Beide Units sind hardware-frei, also läuft hier **keine gemockte Spiellogik**.
Einzige Fixtures sind die zwei Datentabellen, im Testskript byte-genau aus
`baseroms/us/SLUS_007.39` erzeugt (`0x800 + (addr − 0x80010000)`):

- `D_8009AFE8` — 64 × 4 Eckenindizes
- `D_8009B1A4` — 64 Orientierungsbytes (Modus = Wert >> 3)

Alle 50 Erwartungswerte sind unabhängig aus den Doku-Blöcken der Units in Python
nachgerechnet, nicht aus einem Lauf aufgezeichnet. Die Übereinstimmung von
Dokumentation und dekompiliertem Code ist damit selbst das Ergebnis.

**`iso9660_path` — 22 Checks, BIOS-Mock als Beweis**

`func_8001DC48` (`gFormatIso9660Path`) rechnet fast nichts selbst: Messen,
Kopieren, Verbinden, Großschreiben delegiert es vollständig an die BIOS-Vektoren.
Eine Attrappe, die 0 zurückgibt, lässt die Unit einen leeren Pfad schreiben, und
die Kette ist unbeobachtbar. Der Mock antwortet korrekt — und damit wird die
Aufruf**kette** selbst zur Assertion:

```
strcpy → strcat → strlen → toupper
```

Dazu formatierte Pfade für mehrere Modi, die Verweigerungspfade und ein zweiter
Lauf, der einen nicht implementierten Service anfordert und mit `exit 2`
abbrechen muss. Mutationsprobe: `"ABC.DAT"` → `"ABC.DATX"` lässt den Test
fallen.

**`mocks_selftest` — 58 Checks**

Prüft die Attrappen selbst, ohne ein Spielmodul zu linken. Jeder Getter wird
**vor** dem Setzen abgefragt, damit „unkonfiguriert = 0" belegt ist. Dieser Test
hat einen echten Bug in `gpu_mock` gefunden: `LoadImage` kopierte per Default 32
Payload-Bytes und las über das Ende des Aufruferpuffers hinaus. Default ist jetzt
`copy_bytes = 0`.

### 5.4 Ein bestehender Test auf die Bibliothek umgestellt

`naming_round7_harness.c` — seine privaten Stubs **waren** die Sound-Gruppe.
Sieben Definitionen gelöscht, `#include "mocks/sound_mock.h"`, Zustände über
`fcm_sound` gesetzt und gelesen. Die 9 `CHECK`s prüfen dieselben Werte wie
vorher (Volume-Snapshot, Überschreiben bei zweitem Init, 2×2-Menü-Hintergrund-
Matrix); nur `stream_starts` heißt jetzt `fcm_sound.stream_starts`.

### 5.5 Zwei Messwerkzeuge

**`tools/host_test_candidates.py`** (257 Zeilen) — priorisiert Arbeit:

```
python3 tools/host_test_candidates.py --top 20
```

Beantwortet pro Unit: was blockiert sie, und wie viele Units kommen gratis mit.
Die Frontier-Liste rangiert eine einzelne Attrappe danach, wie viele nicht-clean
Units sie freischaltet.

**`tools/host_test_buildability.py`** (552 Zeilen) — die Decke:

```
python3 tools/host_test_buildability.py              # Zusammenfassung
python3 tools/host_test_buildability.py --unit 80067684   # eine Kette, verbose
python3 tools/host_test_buildability.py --json out.json --jobs 6
```

---

## 6. Wo der nächste Mock am meisten bringt

Gemessene Frontier — ein Stand-in, so viele zusätzliche Units:

| Stand-in | schaltet frei | Hardware | Prototyp |
|---|---:|---|---|
| `func_8002AD58` | **50** | — | `u32 func_8002AD58(u32, u32, u32)` |
| `func_80010750` | **48** | GTE | `void func_80010750(u32 a0)` |
| `func_80010234` | **38** | GTE | `void func_80010234(u32, u32, u32)` |
| `func_80010820` | **37** | GTE | `void func_80010820(u32 a0)` |
| `func_8001006C` | **27** | GTE | `void func_8001006C(u32, u32, u32)` |
| `func_80066F78` | 22 | — | `u32 func_80066F78(u32 ×4)` |
| `func_800402DC` | 22 | — | `u32 func_800402DC(u32 ×4)` |
| `func_8001A180` | 20 | — | `u32 func_8001A180(u32 a0)` |
| `func_80091270` | 19 | — | `u32 func_80091270(u32 a0)` |
| `bios_A0_3F_printf` | 19 | BIOS | Kernel-Vektor A0(3Fh) |

Die vier GTE-Einträge schalten zusammen **über 100 Units** frei, und das
GTE-Modell existiert bereits (`tests/host/gte_ref.c`, 58 Vektoren über einen
cop2-Trap verifiziert). Das ist der günstigste nächste Schritt, nicht der
teuerste.

Danach `func_8002AD58`: 50 Units, keine Hardware im Spiel, reine
Kontrollfluss-Attrappe.

---

## 7. Der Stand der Mock-Duplikate

`tests/host/mock_inventory.py` macht den Bestand messbar:

```
stand-in definitions:            65
  bound with an asm alias:       27
  bound by C name only:          38
distinct retail labels:          50
labels defined in >1 file:       12
```

38 Stand-ins sind noch **nur über den C-Namen** gebunden — dort ist eine falsche
Bindung ein stiller Fehler statt eines Linkfehlers. Die 20 in der neuen
Bibliothek sind es nicht; die alten Harnesse sind es.

Zwölf Labels existieren mehrfach mit teils widersprüchlichen Signaturen. Die
gravierendsten:

| Symbol | Konflikt |
|---|---|
| `func_80046C84` | **4 Fassungen**: hart `0` / `0` + 8-Slot-Log / konfigurierbar + 64-Slot-Log |
| `func_8006ACB0` | Rückgabe widersprüchlich: hart `1` vs. konfigurierbar (`0`) |
| `func_80019668` | `form` liefert immer `0`, `naming_round8` rechnet den echten LCG — eine Fassung *erfindet* Werte |
| `func_8001006C` / `func_80010820` | ABI-Konflikt: 3 vs. 4 bzw. 1 vs. 2 Parameter — auf MIPS undefiniertes Verhalten |
| `func_8006B23C` | Rückgabetyp `s32` vs. `void` |

Diese Duplikate stehen in `tests/test_shared_mock_library.py` als
`KNOWN_LOCAL`-Liste fest. Der Test schlägt fehl, sobald ein **neues** Duplikat
auftaucht oder ein Listeneintrag veraltet ist — die Doppelung bleibt damit
sichtbar und gezählt statt still geduldet.

Die Umstellung der verbleibenden 38 ist bewusst nicht erfolgt: der Auftrag
verlangt eine Umstellung als Beweis, nicht eine Generalüberholung aller
Harnesse.

---

## 8. Was ein Host-Test beweist — und was nicht

Der Byte-Match bleibt das Orakel für Übersetzungstreue: 1771/1771 Units,
555.576 Bytes identisch. Ein Host-Test ersetzt das nicht.

Ein Host-Test beweist: Kontrollfluss, Randfälle, Arithmetik, Struct-Offsets —
und ob Doku und Symbolnamen zum tatsächlichen Verhalten passen.

Er beweist **nicht**, dass die PS1-Hardware die geschriebenen Registerwerte so
akzeptiert. Ein MMIO-Store in ein Mock-Array ist immer wahr. Bei
Glue-Funktionen, die im Wesentlichen fünf andere aufrufen, testet ein
vollständig gemockter Harness am Ende die eigene Erwartung an die
Aufrufreihenfolge — deshalb liegt der Schwerpunkt hier auf hardware-freien
Units, wo die echte Kette läuft.

Die Zeilen-Coverage von 94,3 % ist ebenfalls keine Tugend für sich: sie bezieht
sich nur auf die 116 tatsächlich gelinkten Units. `qemu -d in_asm` protokolliert
übersetzte Blöcke, die nahe an, aber nicht identisch mit den ausgeführten
Instruktionen sind — sie ist eine obere Schranke.

---

## 9. Reproduktion

```sh
make check                                        # 192 Python-Tests + 14 Harness-Targets
make reach                                        # welche Funktionen laufen
make reach LIST=1                                 # pro Funktion mit Urteil
make coverage                                     # Zeilenabdeckung
make host-terrain-surface                         # 50 Checks
make host-iso9660-path                            # 22 Checks
python3 tests/host/mock_inventory.py              # Stand-in-Bestand
python3 tools/host_test_candidates.py --top 20    # Frontier
python3 tools/host_test_buildability.py           # Decke ohne Mock
```

---

## 10. Offene Punkte

1. **`src/game/80096748.c:123`** braucht einen `__asm__`-Alias. Einzeilig,
   hebt die Decke von 908 auf 911. Nicht gemacht — `src/` ist Byte-Matching.
2. **38 Stand-ins nur per C-Name gebunden.** Stilles Risiko in den alten
   Harnessen; die neue Bibliothek ist sauber.
3. **162 `unknown`-Units.** Callgraph-Lücken, keine Hardware. Wer sie auflöst,
   verschiebt möglicherweise weitere Units nach `clean`.
4. **`func_800226D8`** blockiert allein 11 Units — von der Messung als `hw`
   klassifiziert (Audio-Stream-Stop), aber ohne eigenen GTE-/MMIO-/BIOS-Zugriff
   in der Signatur. Die Ursache liegt tiefer in der Kette und ist ungeprüft.

Die frühere Zielmarke von 15–25 % war keine Grenze, sondern eine Folge der
Fehlklassifizierung. 54,7 % der Units sind hardware-frei, 908 davon bauen ohne
jede Attrappe — der Vorrat ist groß, der Aufwand pro Unit ist der eigentliche
Knackpunkt.
