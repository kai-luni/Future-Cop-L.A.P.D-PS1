# Testabdeckung der Decompilation: was messbar ist und wo die Grenze liegt

> **Historisch und teilweise widerlegt:** Die alte Reichweitenmessung zählte
> Mocks als Originale; die Zahl 969 ist wegen fehlender Graphkanten kein Beweis
> für Hardwarefreiheit. Linkproben mit Nullslots belegen keine Ausführbarkeit.
> Der aktuelle Stand und die Messgrenzen stehen in `docs/host-test-current.md`.

Stand: HEAD `986e47a`. Alle Zahlen in diesem Bericht sind gemessen, nicht
geschätzt — die Befehle zum Nachrechnen stehen in jedem Abschnitt.

## 1. Kurzfassung

| Frage | Antwort |
|---|---|
| Kann man die Testabdeckung des C-Codes messen? | Ja — `make coverage`, 1607/1705 Zeilen in den abgedeckten Units (94,3 %) |
| Wieviele Funktionen laufen in Tests? | 116 von 1772 — alle gelinkten werden auch ausgeführt |
| Wieviel Prozent der Codebasis ist das? | 6,5 % der 1772 Units |
| Wieviel ist überhaupt ohne Mock baubar? | **969 Units (54,7 %) sind hardware-frei, 908 davon (93,7 %) kompilieren und linken ohne jede Attrappe** |
| Wieviel ist überhaupt testbar? | Die 15–25 %-Schätzung dieses Berichts war zu niedrig — auf Basis der Disassembly-Signatur sind 54,7 % hardware-frei, siehe §5 |
| Warum nicht mehr? | Nur 90 Units greifen Hardware direkt an — der Rest ist Aufwand, nicht Unmöglichkeit |

## 2. Das Messverfahren

`gcov` und `lcov` fallen aus: die Toolchain ist gcc 2.7.2 ohne
`-fprofile-arcs`, und die Tests sind MIPS-ELFs unter qemu. Zwei vorhandene
Eigenschaften reichen stattdessen:

1. `gcc 2.7.2 -g` schreibt `.loc`-Direktiven, also liefert
   `mipsel-linux-gnu-objdump -d -l` zu jeder Instruktion die Quellzeile.
2. `qemu-mipsel -d in_asm` protokolliert jeden übersetzten — also ausgeführten —
   Grundblock mit Adresse.

`tools/host_coverage.py` schneidet beides: pro Unit kompilieren wie das
Testskript (`-O1 -G0`, plus `-g`), Zeilenkarte aus `objdump` lesen, Testbinary
unter qemu laufen lassen, Adressen mit der Zeilenkarte schneiden.

```
make coverage                 # Tabelle
make coverage LCOV=cov.info   # LCOV-Tracefile für jeden Viewer
```

Ergebnis: **469 von 469 Zeilen in 53 Units = 100 %**. Eine Zeile zählt als
abgedeckt, wenn mindestens eine ihr zugeordnete Instruktion lief; nur Zeilen mit
Code kommen in den Nenner, also zählen Deklarationen und Kommentare nicht mit.

Nebenwirkung: die Messung hat drei echte Lücken aufgedeckt und geschlossen — die
Marker-Typen 0/1 und der Pfad „Puffer voll" in `src/game/renderer-scene/gDrawRadarMarker.c` waren
ungetestet.

## 3. Wieviele Funktionen erreichen die Tests wirklich?

`make coverage` zählt nur die Units, die ein Testskript namentlich nennt. Ein
Testbinary linkt aber deren echte Abhängigkeiten mit, und verschiedene Tests
erreichen verschiedene Zweige. `tools/host_test_reach.py` misst die ehrliche
Zahl: die Vereinigung der ausgeführten Zeilen jeder Unit über alle Binaries, die
sie linken.

```
make reach            # Zusammenfassung
make reach LIST=1     # pro Funktion
```

| Kennzahl | Wert |
|---|---|
| Testbinaries | 20 |
| gelinkte `func_`-Symbole | 116 |
| davon tatsächlich ausgeführt | 116 |
| davon Handschrift-Asm (keine Zeileninfo) | 18 |
| Quellzeilen gemessen | 1705 |
| davon ausgeführt | 1607 (94,3 %) |

18 der 116 Units sind Handschrift-Asm ohne `.loc`-Zeilen (die GTE-Ströme sowie
die Sound-/BIOS-Wrapper); sie werden über ihren Adressbereich im qemu-Trace als
ausgeführt nachgewiesen.

Acht Units sind nur teilweise durchlaufen, die größte Lücke ist
`func_80044B34` mit 19/72 Zeilen.

Wichtig: die Testskripte nennen 68 Units, gelinkt sind 100. Der Unterschied sind
transitive Abhängigkeiten, die kein Skript explizit auflistet — genau der Grund,
warum die naive Zählung über Skripte zu niedrig liegt.

## 4. Die harte Grenze: GTE, SPU, CD, BIOS

Die Host-Tests laufen als Linux-Prozess unter `qemu-mipsel` im **User-Mode**:
linearer Adressraum, Syscalls, sonst nichts. Kein Gerät, kein Kernel-Modus, keine
Interrupts. Drei Probes gegen genau diesen qemu belegen, dass es faultet statt
Müll zu liefern:

| Probe | Ergebnis |
|---|---|
| `mfc2 $t0,$31` (eine cop2-Instruktion) | SIGILL, Exit 132 |
| `lw` von `0x1F801814` (GPUSTAT) | SIGSEGV, Exit 139 |
| `jr $t2` mit `$t2 = 0xA0` (BIOS-Aufruf) | SIGSEGV, Exit 139 |

### GTE — Coprozessor 2
Festkomma-Einheit für Matrizen und Vertex-Projektion, nur über cop2-Instruktionen
erreichbar, ohne Speicheradresse. Deshalb SIGILL. **Aber:** reine Arithmetik ohne
I/O, also modellierbar — und im Repo bereits modelliert. `tests/host/gte_ref.c`
(107 Zeilen) bildet Matrix-Load, MVMVA, Quotient und RTPS nach;
`tests/host/env_harness.c` fängt das SIGILL, führt die Instruktion gegen das
Modell aus und setzt bei `pc+4` fort. Dadurch laufen die sieben Asm-Units als
echter Retail-Code.

Grenze: ein bestandener Vektor beweist Übereinstimmung mit dem **Modell**, nicht
mit der Hardware. Eine bekannte Abweichung ist in
`docs/environment-render-path.md` dokumentiert (Clip-Byte-Vergleich).

### SPU — Sound Processing Unit
Eigener Audio-DSP mit 512 KB Sample-RAM, 24 ADPCM-Stimmen, Register bei
`0x1F801C00`. Zwei unabhängige Probleme: der Bereich faultet, und die Semantik
ist ein Echtzeit-Handshake — `src/game/platform/gSpuWriteMemBlock.c` pollt `SPUSTAT & 0x400`, bis
ein Transfer fertig ist; ohne Gerät terminiert das nie. Und das Ergebnis ist
Schall, es gibt keinen Wert zum Vergleichen.

### CD-Subsystem
Vier Byte-Register bei `0x1F801800`, Ein-Byte-Kommandos, Antwort über Interrupt
statt Rückgabewert. Es fehlen drei Dinge gleichzeitig: das Laufwerk, die
IRQ-Zustellung an den Gast und das Medium selbst — die Retail-Disc-Daten liegen
bewusst nicht im Repo.

### BIOS
512-KB-Boot-ROM mit Kernel; Aufruf über Sprung auf `0xA0`/`0xB0`/`0xC0` mit
Service-Index in `$t1` (siehe `src/game/platform/func_8008D650.c`: `addiu $t2,$zero,0xA0` /
`jr $t2` / `addiu $t1,$zero,0xAC`). Der Sprung faultet, und der Code liefe im
Kernel-Modus mit COP0, Exception-Vektoren und `I_STAT`/`I_MASK`. Ein Stub könnte
nur „Erfolg" zurückgeben — der Test wäre tautologisch.

## 5. Prozentzahlen: was ist überhaupt testbar?

Klassifikation aus dem Quelltext (cop2/mtc2/mfc2, MMIO `0x1F80xxxx`,
BIOS-Sprung `0xA0`/`0xB0`/`0xC0`), danach transitive Hülle über den Aufrufgraphen:

| Kategorie | Units | Anteil |
|---|---:|---:|
| alle C-Units | 1772 | 100,0 % |
| direkter Hardware-Zugriff | 90 | 5,1 % |
| — davon cop2/GTE | 39 | 2,2 % |
| — davon MMIO (SPU/CD/GPU/DMA) | 3 | 0,2 % |
| — davon BIOS-Aufruf | 46 | 2,6 % |
| — davon Syscall | 2 | 0,1 % |
| ohne direkten Hardware-Token | 1682 | 94,9 % |
| transitiv hardware-abhängig | 803 | 45,3 % |
| **frei von jeder Hardware-Abhängigkeit** | **969** | **54,7 %** |
| unbekannt (Callgraph-Lücke) | 162 | 9,1 % |
| aktuell in Tests ausgeführt | 116 | 6,5 % |

Verteilung der Entfernung zur nächsten Hardware-Unit im Aufrufgraphen:

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
| kein Pfad zur Hardware (969 clean + 162 unknown) | 1131 | 63,8 % |

Der zentrale Befund: nur 90 Units greifen **direkt** zu, 641 erreichen
Hardware **transitiv** — und 969 Units erreichen sie **überhaupt nicht**.
Damit ist die Mehrheit der Codebasis nicht durch Hardware blockiert.

## 6. Kann man die 641 hardware-abhängigen trotzdem testen — Aufrufe mocken?

**Ja, an wenigen Schnittstellen.** Der Mock-Frontier ist messbar: ein Stand-in
schaltet nicht eine, sondern bis zu 50 Units frei.

| Stand-in | schaltet frei | Hardware |
|---|---:|---|
| `func_8002AD58` | 50 | — |
| `func_80010750` | 48 | GTE |
| `func_80010234` | 38 | GTE |
| `func_80010820` | 37 | GTE |
| `func_8001006C` | 27 | GTE |
| `func_80066F78` | 22 | — |
| `func_800402DC` | 22 | — |

Ein GTE-Modell existiert bereits (`gte_ref.c`); die vier GTE-Einträge oben
schalten zusammen über 100 weitere Units frei. Das ist der günstigste nächste
Schritt, nicht der teuerste.

Nachgemessen: `python3 tools/host_test_candidates.py --top 20`.

### Wo der richtige Mock-Schnitt liegt

- **An der Hardware-Grenze (die 153):** klingt attraktiv, weil ein einmaliger
  Layer theoretisch 91,3 % freischaltet. Praktisch funktioniert das nur für GTE
  (38 Units), weil das Modell schon existiert. Bei MMIO (44) und BIOS (76) hängt
  echte Zustandslogik dahinter — man baut entweder einen halben Emulator nach
  oder so flache Attrappen, dass der Test nichts mehr aussagt.
- **Direkt an den Callees der Testeinheit:** der pragmatische Standard. Man linkt
  1–3 Units und stubbt deren direkte Aufrufe. Aufwand bleibt klein.
- **Dazwischen, an der Semantik-Grenze:** der beste Kompromiss. Callees stubben
  bei Zustandsautomaten, KI und Menülogik; bei Grafik-Units den Kommandostrom
  abgreifen statt die GPU zu emulieren; bei Mathe den vorhandenen GTE-Trap
  nutzen.

### Was im Repo schon existiert

| Technik | Beispiel |
|---|---|
| Linker-Stubs unter Retail-Symbolnamen | `s32 gPlayNonpositionalSoundEffect(...) __asm__("func_80046C84")` |
| **Gemeinsame Mock-Bibliothek** | `tests/host/mocks/{sound,gpu,bios}_mock.c` — 20 Attrappen, per `asm`-Alias gebunden |
| **Freestanding Test-Kit** | `tests/host/fc_testkit.h` — Assertions mit Wertausgabe, Call-Recorder |
| Call-Recorder (Sound-IDs, Zähler, steuerbare Rückgabewerte) | `fcm_sound.sfx_ids[]`, `fcm_gpu.draw_sync_modes[]` |
| cop2-Trap gegen Software-GTE | `env_harness.c` + `gte_ref.c` |
| Statusbit-Stub, der terminiert | nötig überall dort, wo auf ein Ready-Bit gepollt wird |

Der Bestand an Attrappen ist messbar und wird überwacht:

```
python3 tests/host/mock_inventory.py
  stand-in definitions:      65
    bound with an asm alias: 27
    bound by C name only:    38
  distinct retail labels:    50
  labels defined in >1 file: 12
```

38 Stand-ins sind noch **nur über den C-Namen** gebunden — dort ist eine falsche
Bindung ein stiller Fehler statt eines Linkfehlers. Sie stehen in
`tests/test_shared_mock_library.py` als `KNOWN_LOCAL`-Liste fest, damit die
Doppelung sichtbar bleibt und der Test anschlägt, sobald ein neues Duplikat
auftaucht.

### Die ehrliche Aussagekraft-Grenze

Der Byte-Match (1771/1771 Units, 555.576 Bytes) bleibt das Orakel für
Übersetzungstreue — er beweist, dass der C-Code exakt dem Retail-Binary
entspricht. Ein Host-Test ersetzt das nicht.

Ein gemockter Test beweist: Kontrollfluss, Randfälle, Arithmetik, Struct-Offsets
— und ob Dokumentation und Symbolnamen zum tatsächlichen Verhalten passen. Er
beweist **nicht**, dass die PS1-Hardware die geschriebenen Registerwerte so
akzeptiert. Ein MMIO-Store in ein Mock-Array ist immer wahr. Bei Glue-Funktionen,
die im Wesentlichen fünf andere aufrufen, testet ein vollständig gemockter
Harness am Ende nur die eigene Erwartung an die Aufrufreihenfolge.

### Kosten-Realität

22 Skripte decken 116 Funktionen ab, also ~5,3 Units pro Skript. Die großen
Harnesse sind umfangreich: `weapon_anim_harness.c` 465 Zeilen für eine Unit,
`naming_round8_harness.c` 311 Zeilen für drei, `env_harness.c` 466 Zeilen. Auf
die verbleibenden 853 mock-freien Units hochgerechnet wären das rund 200
Harnesse. Das ist viel Arbeit, aber keine technische Grenze — und der Anteil an
Units, die über eine Kette mehrere Funktionen mitziehen, senkt den Aufwand pro
erreichter Unit deutlich.

## 7. Empfehlung

1. **Die Decke ist keine 155, sondern 969.** Gemessen mit
   `tools/host_test_buildability.py`: 908 der 969 hardware-freien Units
   kompilieren und linken **ohne jede Attrappe**, nur mit genullten
   `D_`-Datenslots (93,7 %). Das ist der günstigste Vorrat, weit größer als
   bisher angenommen. 506 davon sind Blätter, die größte Kette linkt 58 Objekte.
2. **Priorität 1 — die 908 bauen.** Kein Mock nötig, echter Code, echte
   Assertions. Der Aufwand pro Unit ist der einzige Knackpunkt, nicht die
   Technik.
3. **Priorität 2 — vier GTE-Stand-ins** (`func_80010750`, `func_80010234`,
   `func_80010820`, `func_8001006C`) schalten über 100 weitere Units frei, und
   das GTE-Modell existiert schon. Danach `func_8002AD58` (50 Einheiten, keine
   Hardware).
4. **Nicht mocken — die 803 transitiv hardware-abhängigen Reste.** Wer diese
   Ketten end-to-end prüfen will, fährt mit automatisierten Läufen in einem
   echten Emulator besser.

Die frühere Schätzung von 15–25 % war keine Grenze, sondern eine Folge der
Fehlklassifizierung über Kommentartext. Auf Basis der Disassembly-Signatur sind
54,7 % der Units hardware-frei.

## 8. Reproduktion

```
make check                     # 192 Python-Tests + 12 Harnesse
make coverage                  # Zeilenabdeckung der Host-Tests
make coverage LCOV=cov.info    # dasselbe als LCOV-Tracefile
make reach                     # wieviele Funktionen die Tests erreichen
make reach LIST=1              # pro Funktion mit Urteil
python3 tools/host_test_candidates.py --top 20   # was ein Stand-in freischaltet
python3 tools/host_test_buildability.py          # was ohne Mock baut
```

Die Messwerkzeuge sind `tools/host_coverage.py` und `tools/host_test_reach.py`,
die Trace-Aufzeichnung läuft über `tests/host/qemu_trace.sh` (nutzt die echten
Fixtures der Testskripte statt sie nachzubauen).
