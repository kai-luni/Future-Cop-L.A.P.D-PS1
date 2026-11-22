# Testabdeckung der Decompilation: was messbar ist und wo die Grenze liegt

Stand: HEAD `71c28e3`. Alle Zahlen in diesem Bericht sind gemessen, nicht
geschätzt — die Befehle zum Nachrechnen stehen in jedem Abschnitt.

## 1. Kurzfassung

| Frage | Antwort |
|---|---|
| Kann man die Testabdeckung des C-Codes messen? | Ja — `make coverage`, 469/469 Zeilen in den 53 abgedeckten Units (100 %) |
| Wieviele Funktionen laufen in Tests? | 100 von ~1763 — alle gelinkten werden auch ausgeführt |
| Wieviel Prozent der Codebasis ist das? | 5,6 % der 1772 Units |
| Wieviel ist überhaupt testbar? | realistisch 15–25 %, hart begrenzt bei ~10,8 % ohne tiefe Mocks |
| Warum nicht mehr? | 91,3 % der Units erreichen transitiv Hardware, die es im Host-Test nicht gibt |

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
Marker-Typen 0/1 und der Pfad „Puffer voll" in `src/game/8005B088.c` waren
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
| Testbinaries | 17 |
| gelinkte `func_`-Symbole | 100 |
| davon tatsächlich ausgeführt | 100 |
| davon Handschrift-Asm (keine Zeileninfo) | 7 |
| Quellzeilen gemessen | 1567 |
| davon ausgeführt | 1478 (94,3 %) |

Die sieben Handschrift-Units (`8001006C`, `8001010C`, `800101D4`, `80010234`,
`8001047C`, `80010750`, `80010820`) sind reine `.word`-Ströme ohne `.loc`-Zeilen;
sie werden über ihren Adressbereich im qemu-Trace als ausgeführt nachgewiesen.

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
ist ein Echtzeit-Handshake — `src/game/80097138.c` pollt `SPUSTAT & 0x400`, bis
ein Transfer fertig ist; ohne Gerät terminiert das nie. Und das Ergebnis ist
Schall, es gibt keinen Wert zum Vergleichen.

### CD-Subsystem
Vier Byte-Register bei `0x1F801800`, Ein-Byte-Kommandos, Antwort über Interrupt
statt Rückgabewert. Es fehlen drei Dinge gleichzeitig: das Laufwerk, die
IRQ-Zustellung an den Gast und das Medium selbst — die Retail-Disc-Daten liegen
bewusst nicht im Repo.

### BIOS
512-KB-Boot-ROM mit Kernel; Aufruf über Sprung auf `0xA0`/`0xB0`/`0xC0` mit
Service-Index in `$t1` (siehe `src/game/8008D650.c`: `addiu $t2,$zero,0xA0` /
`jr $t2` / `addiu $t1,$zero,0xAC`). Der Sprung faultet, und der Code liefe im
Kernel-Modus mit COP0, Exception-Vektoren und `I_STAT`/`I_MASK`. Ein Stub könnte
nur „Erfolg" zurückgeben — der Test wäre tautologisch.

## 5. Prozentzahlen: was ist überhaupt testbar?

Klassifikation aus dem Quelltext (cop2/mtc2/mfc2, MMIO `0x1F80xxxx`,
BIOS-Sprung `0xA0`/`0xB0`/`0xC0`), danach transitive Hülle über den Aufrufgraphen:

| Kategorie | Units | Anteil |
|---|---:|---:|
| alle C-Units | 1772 | 100,0 % |
| direkter Hardware-Zugriff | 153 | 8,6 % |
| — davon cop2/GTE | 38 | 2,1 % |
| — davon MMIO (SPU/CD/GPU/DMA) | 44 | 2,5 % |
| — davon BIOS-Aufruf | 76 | 4,3 % |
| ohne direkten Hardware-Token | 1619 | 91,4 % |
| transitiv hardware-abhängig | 1617 | 91,3 % |
| **frei von jeder Hardware-Abhängigkeit** | **155** | **8,7 %** |
| **Obergrenze mit GTE-Modell** (155 + 36 reine GTE-Units) | **191** | **10,8 %** |
| aktuell in Tests ausgeführt | 100 | 5,6 % |

Verteilung der Entfernung zur nächsten Hardware-Unit im Aufrufgraphen:

| Hops | Units | Anteil |
|---|---:|---:|
| 0 (greift selbst zu) | 153 | 8,6 % |
| 1 | 427 | 24,1 % |
| 2 | 530 | 29,9 % |
| 3 | 305 | 17,2 % |
| 4 | 134 | 7,6 % |
| 5 | 41 | 2,3 % |
| 6 | 20 | 1,1 % |
| 7–9 | 7 | 0,5 % |
| unerreichbar (clean) | 155 | 8,7 % |

Der zentrale Befund: nur 153 Units greifen **direkt** zu, aber 1617 erreichen
Hardware **transitiv**. Von den 1619 ohne eigenen Hardware-Token rufen nur 427
direkt eine Hardware-Unit; 1192 haben gar keinen direkten Hardware-Callee.

## 6. Kann man die 1619 trotzdem testen — Aufrufe mocken?

**Teilweise.** Technisch geht es fast überall, das Muster läuft bereits in 22
Harnessen für 100 Funktionen. Flächendeckend über alle 1619 ist es aber keine
tragfähige Strategie, sondern ein Fass ohne Boden mit stark abnehmender
Aussagekraft.

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
| Call-Recorder (Sound-IDs, Zähler, steuerbare Rückgabewerte) | `weapon_anim_harness.c` (`sound_log[]`, `fire_calls`, `fire_result`) |
| GPU-Kommandostrom-Recorder | `naming_round8_harness.c` (`packet_arena[8]`, prüft Tags/Farben/Koordinaten) |
| cop2-Trap gegen Software-GTE | `env_harness.c` + `gte_ref.c` |
| Statusbit-Stub, der terminiert | nötig überall dort, wo auf ein Ready-Bit gepollt wird |

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

22 Skripte decken 100 Funktionen ab, also ~4,5 Units pro Skript. Die großen
Harnesse sind umfangreich: `weapon_anim_harness.c` 465 Zeilen für eine Unit,
`naming_round8_harness.c` 311 Zeilen für drei, `env_harness.c` 466 Zeilen. Auf
1619 Units hochgerechnet wären das 350–400 Harnesse und zehntausende Zeilen
handgepflegter Struct-Nachbauten.

## 7. Empfehlung

1. **Priorität 1 — die 155 komplett freien Units (8,7 %).** Kein Hardware-Mock
   nötig, minimaler Aufwand, maximale Aussagekraft. Aktuell laufen davon erst 14 von 155; bei den reinen GTE-Units 6 von 36.
2. **Priorität 2 — gezielt aus dem 1–2-Hop-Bereich (957 Units, 54 %).** Nur
   Units mit echter Eigenlogik: Zustandsautomaten, Physik/Kollision, Waffen,
   Menüs, Kamera, GPU-Packet-Emitter. Am billigsten genau dann, wenn eine Unit
   ohnehin benannt oder dokumentiert wird.
3. **Nicht mocken — 3–9 Hops (507 Units, 28,6 %) und Hardware-Wrapper.** Der
   Mock-Aufwand übersteigt den Nutzen. Wer diese Ketten end-to-end prüfen will,
   fährt mit automatisierten Läufen in einem echten Emulator besser.

Realistischer Zielkorridor: **15–25 % der Codebasis**, also ~250–450 Units.
Darüber hinaus steigen Aufwand und Tautologie-Risiko schneller als der
Erkenntnisgewinn.

## 8. Reproduktion

```
make check                     # 187 Python-Tests + 10 Harnesse
make coverage                  # Zeilenabdeckung der Host-Tests
make coverage LCOV=cov.info    # dasselbe als LCOV-Tracefile
make reach                     # wieviele Funktionen die Tests erreichen
make reach LIST=1              # pro Funktion mit Urteil
```

Die Messwerkzeuge sind `tools/host_coverage.py` und `tools/host_test_reach.py`,
die Trace-Aufzeichnung läuft über `tests/host/qemu_trace.sh` (nutzt die echten
Fixtures der Testskripte statt sie nachzubauen).
