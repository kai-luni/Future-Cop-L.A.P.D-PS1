# Globale RAM-Variablen in Future Cop: L.A.P.D.

**Stand: 18. November 2026 · US-Version (`SLUS_007.39`)**

## Worum geht es hier?

Diese Datei dokumentiert die **Datenvariablen im RAM**: Spielzustand, Treiber, Warteschlangen, Puffer und Zähler. Sie enthalten keine Hardware-Adressen, sondern Werte und Zeiger auf Spielstrukturen.

Für sie wurde geprüft, welche Rolle sie im Code haben; die neuen Namen stehen auch im C-Code und sind über `__asm__("D_XXXXXXXX")` an das Retail-Symbol gebunden. Der Datentyp blieb jeweils unverändert, und das Byte-Matching der betroffenen Units wurde vor und nach jeder Umbenennung verglichen.

Die Zeiger auf PlayStation-Register stehen in der Schwesterdatei [`hardware-pointers.md`](hardware-pointers.md).

Konfidenz: **bestätigt** = Schreiben und Lesen belegen die Rolle eindeutig; **wahrscheinlich** = Rolle aus dem Code ableitbar, einzelne Details offen.

## Zeitmessung und Interrupts

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DD50` | `gVSyncTickCounter` | `volatile s32` | Zählt die VSync-Interrupts; der VBlank-Handler erhöht ihn, die Frame-Wartefunktion pollt ihn und bricht mit „VSync: timeout" ab. **Bestätigt.** | `func_80091BF8`, `func_800913E8` |
| `D_8009CC20` | `gTimerBaselineTick` | `volatile u32` | Letzte stabile Probe des Timers 1; verstrichene Ticks = `(now - gTimerBaselineTick) & 0xFFFF`, danach neu gesampelt. **Bestätigt.** | `func_80091270` |
| `D_8009DD20` | `gIrqStuckCounter` | `int` | Zählt Dispatch-Runden, in denen Status und Maske gesetzt bleiben; ab `0x800` Log und Status löschen, sonst zurücksetzen. **Rolle bestätigt.** | `func_800916EC` |
| `D_8009DEF0` | `gDispatchSavedIrqMask` | `u32` | Sichert die Interruptmaske über die GPU-Dispatch-Sektion und schreibt sie danach zurück. **Bestätigt.** | `func_80094A3C` |
| `D_8009DEF8` | `gSavedIrqMask` | `u32` | Dasselbe für den Transferpfad: Maske sichern, Arbeit erledigen, Maske zurück. **Bestätigt.** | `func_80094F4C`, `func_8009509C` |
| `D_8009DD10` | `gIrqCallbackTable` | `struct CallbackTable *` | Zeiger auf die Tabelle mit dem IRQ-Rücksprung, der beim Betreten/Verlassen der Handler gerufen wird. **Wahrscheinlich** (Inhalt der Tabelle nicht vollständig analysiert). | `func_80091480`, `func_800914B0` |
| `D_8009FA2C` | `gFrameTickCounter` | `s32` | Frame-Taktzähler; Rumble-Timer und andere Zeitgeber werden dagegen verglichen. **Bestätigt** (Doku: „frame tick counter"). | `func_8001EC94`, `func_8001F0B8` |
| `D_8009F9FC` | `gFrameDelta` | `s32` | Frame-Kennung für Zeitvergleiche; wird auf 5 gesetzt und bei Bedarf aus der Zeitbasis berechnet. **Wahrscheinlich.** | `func_800259D8`, `func_80025B7C` |
| `D_8009FA3C` | `gFrameBarrier` | `volatile u8` | Frame-Schranke: am Frame-Ende auf 1 gesetzt, andere Stellen warten `while (gFrameBarrier)`. **Bestätigt.** | `func_8001B4A8`, `func_8001EFF8` |

## GPU-Warteschlange

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DEE8` | `gGpuJobHead` | `volatile s32` | Schreibindex des 64 Einträge großen GPU-Auftragsrings; voll, wenn `((head + 1) & 0x3F) == tail`. **Bestätigt.** | `func_80094A3C`, `func_8009509C` |
| `D_8009DEEC` | `gGpuJobTail` | `volatile s32` | Leseindex desselben Rings; `count = (head - tail) & 0x3F` ist die Zahl wartender Aufträge. **Bestätigt.** | `func_8009509C`, `func_80094F4C` |

## Grafik-Puffer und Pakete

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009FA90` | `gPacketCursor` | `u8 *` | Schreibzeiger im GPU-Paketpuffer; das Makro `ALLOC` vergleicht ihn mit dem Limit und rückt ihn weiter. **Bestätigt** (Doku nennt ihn „the GPU packet cursor"). | `func_80014490`, `func_8001484C` |
| `D_8009F9C0` | `gPacketLimit` | `u8 * volatile` | Ende des aktuellen Paketpuffers; liegt der Cursor darüber, wird neu alloziert. **Bestätigt** („the packet limit"). | `func_80014490`, `func_8001484C` |
| `D_8009F9F0` | `gPrimPoolNext` | `Prim *` | Nächster freier Eintrag im Primitive-Pool (`h < Ende` prüft die Grenze). **Bestätigt.** | `func_80014C64`, `func_80015F38` |
| `D_8009FA28` | `gPrimPoolEnd` | `Prim *` | Ende des Primitive-Pools; die Allokationsmakros vergleichen dagegen. **Bestätigt.** | `func_80014C64`, `func_800164F4` |
| `D_8009F4BC` | `gMovieSliceContext` | `s32` | Kontext eines Videostreifens: zwei Rechteck-Paare, Quellenindex `+0x30`, Ziel `+0x24`/`+0x28`, Latch `+0x11C` (0 = erster Durchlauf, 1 = Folgeschritt). **Bestätigt.** | `gMovieSliceFeedDma`, `gMovieSliceFeedRows` |
| `D_8009F4C0` | `gStripScaleEnable` | `s32` | Schalter für horizontales Skalieren eines Streifens; nur bei `== 1` und gesetztem `D_8009F4C4` aktiv. **Bestätigt.** | `func_80085074` |

## Video und Filmwiedergabe

Der Ablauf steht in [`video-loading-path.md`](video-loading-path.md). Diese Tabelle listet die
Datenvariablen dazu; `gMovieDisplayMode`/`gMovieAbortFlag`/`gMovieStreamContext`/
`gMovieStreamDescriptors`/`gMovieSliceContext` waren vorher als `gChunkSkipFlag`,
`gLoopAbortFlag`, `gQueueBlock`, `gStreamDescriptors` und `gDecodeJob` benannt.

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F4D8` | `gMovieAudioState` | `s32 *` | 0x7B80 Byte großer Audiozustand des Films: zwei 0x3C00-Byte-ADPCM-Ringe bei `+0x10`/`+0x3C10` mit Lese-/Schreibzeiger `+0x00`/`+0x04`, Uhr-Freigabe `+0x7B20`, Uhrstand `+0x7B24`, vier Beschäftigt-Wörter `+0x7B2C`, Modus `+0x7B74`. **Bestätigt.** | `gMovieAudioChunkPush`, `gMovieFrameDeadline` |
| `D_8009F4DC` | `gMovieDisplayMode` | `s32` | Anzeigemodus des Films: Bit 0 erlaubt Chunks ab 0x424C Byte, Bit 1 maskiert die Mitte im 24-Bit-Pfad, Bit 5 (mit Bit 2) wählt die RGB555→RGB888-Hochrechnung. **Bestätigt.** | `gOpenVideoLayer`, `gPlayVideoFile` |
| `D_8009F4E0` | `gMovieAbortFlag` | `u8` | Gesetzt, wenn während der Wiedergabe Cross gedrückt wird oder der Stream einen Fehler meldet; Ladebildschirme brechen damit ab. **Bestätigt.** | `gMovieStreamDemux` |
| `D_8009F958` | `gMoviePadMask0` | `u32` | Vorherige Start/Cross-Maske von Port 0 (`func_8001EF8C(0) & 0x40080000`) für die Flankenerkennung. **Bestätigt.** | `gMovieStreamDemux` |
| `D_8009F95C` | `gMoviePadMask1` | `u32` | Dasselbe für Port 1. **Bestätigt.** | `gMovieStreamDemux` |
| `D_8009F970` | `gMovieStreamContext` | `u32 *` | Kontext des CD-Stream-Lesers: Zähler `+0x414`…`+0x44C`, Positionen `+0x424`/`+0x428`, Callback `+0x570`, getauschte DMA3-/CD-Hooks `+0x574`/`+0x578`, 56-Byte-Einträge ab `+0x57C`. **Bestätigt.** | `gMovieStreamReaderInit` |
| `D_8009F974` | `gMovieStreamDescriptors` | `u32 *` | Basis des 56-Byte-Eintragsarrays (Kontext `+0x57C`); je Eintrag wird ein 24-Byte-Deskriptor bei `+12` abgelegt. **Bestätigt.** | `gMovieStreamReaderInit` |
| `D_8009F978` | `gMovieRingStart` | `u32 *` | Lesezeiger des Ringpuffers, direkt hinter dem Eintragsarray. **Bestätigt.** | `gMovieStreamReaderInit` |
| `D_8009F97C` | `gMovieRingEnd` | `u32 *` | Ende des Rings (`gMovieRingStart + gMovieRingBytes`). **Bestätigt.** | `gMovieStreamReaderInit` |
| `D_8009F984` | `gMovieRingBytes` | `s32` | Ringlänge in Bytes: 0x4400, im toleranten Modus 0x424C. **Bestätigt.** | `gMovieStreamReaderInit` |
| `D_8009F988` | `gMovieDescriptorCount` | `s32` | Zahl der Stream-Deskriptoren im Eintragsarray. **Bestätigt.** | `gMovieStreamReaderInit` |
| `D_8009F4EC` | `gMovieStreamBuffer` | `s32` | Heapblock, den `gMovieStreamReaderInit` als Streamkontext übernimmt. **Bestätigt.** | `gOpenVideoLayer`, `gPlayVideoFile` |
| `D_8009F4F0` | `gMovieAudioBlock` | `s32` | 0x7B80-Byte-Block für den Audiozustand, von `func_80087824` gefüllt. **Bestätigt.** | `gOpenVideoLayer` |
| `D_8009F4F4` | `gMovieSpuBufferA` | `s32` | SPU-Puffer A, 0xC00 Byte, vom SPU-Allokator `func_80097B28` (Freigabe `func_80098708`). **Wahrscheinlich.** | `gOpenVideoLayer` |
| `D_8009F4F8` | `gMovieSpuBufferB` | `s32` | SPU-Puffer B derselben Größe. **Wahrscheinlich.** | `gOpenVideoLayer` |
| `D_8009F51C` | `gMovieVlcTableA` | `u32` | Erstes Tabellenblatt der Huffman-/VLC-Dekodierung, von `func_80089A88` gefüllt. **Bestätigt.** | `gMovieBuildVlcTables` |
| `D_8009F520` | `gMovieVlcTableB` | `u32` | Zweites Tabellenblatt derselben Dekodierung. **Bestätigt.** | `gMovieBuildVlcTables` |

Ohne Namen bleiben `D_8009F960` (0x400) und `D_8009F964` (0x800): beide sind Bildgeometrie, aber aus
dem Code ist keine Achsenzuordnung belegbar. `D_8009F980` heißt inzwischen `gMovieRingSectors`:
`gCalcMovieBufferSize` schreibt dort die Ring-Kapazität in 2048-Byte-Sektoren, der Ring-Schreiber
liest sie als Divisor.

## Spielzustand und Warteschlangen

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009FA00` | `gScreenState` | `s32` | Aktueller Bildschirmzustand der Hauptschleife (z. B. 1 = „loading", 13 = HUD-Phase). **Bestätigt.** | `func_80024E3C`, `func_8001EFF8` |
| `D_8009F3A8` | `gFrontEndBootstrapModule` | `u8[]` | Dateiname `FE.mis` für den initialen Front-End-Modullauf. **Bestätigt.** | `gGameMain` |
| `D_8009F3B0` | `gMissionModuleSuffix` | `u8[]` | Endung `.mis`, die beim Aufbau des Modulnamens angehängt wird. **Bestätigt.** | `gGameMain` |
| `D_8009F9A8` | `gPendingScreenState` | `s32` | Nächster Bildschirmzustand; spiegelt `unk10` der Hauptschleife, solange er ungleich 0 ist, läuft die Schleife weiter. **Bestätigt.** | `func_80076A2C` |
| `D_8009FA60` | `gGameClock` | `s32` | Zähler für den Palettenwechsel; das HUD liest `(>> 4) & 7` daraus. **Bestätigt.** | `func_8001EFF8` |
| `D_8009FA20` | `gFrameCallbackEnabled` | `u8` | Schalter für den Pro-Frame-Rücksprung; 0 bedeutet „läuft noch nicht". **Bestätigt** (Doku: „per-frame-callback enabled flag"). | `func_8001E4E4`, `func_8001E540` |
| `D_800A0BC0` | `gRingBuffer` | `Queue298EC` | Ringpuffer mit Länge, Basisindex und acht 8-Byte-Slots für Kommandos. **Bestätigt.** | `func_800298EC` |
| `D_800A0B70` | `gStreamBufferList` | `State` | Kopf der Liste von Stream-/Asset-Puffern (`head` bei +0x10); neue Knoten werden hinten angehängt. **Bestätigt.** | `func_800288BC` |
| `D_800A0A00` | `gObjectSlots` | `Unk800A0A00[8]` | Acht Objekt-Slots; `unk00`/`unk01` = -1 markiert einen freien Slot. **Bestätigt.** | `func_8002333C` |

## Spielobjekte, Grafik und Zufall

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F9E4` | `gCamera` | `struct Camera *` | Die aktive Kamera; die Funktionen setzen Position, Blickpunkt, Rotation und die daraus gebaute Matrix. **Bestätigt.** | `func_800177AC`, `func_80017860` |
| `D_8009AA38` | `gIdentityMatrix` | `struct Matrix3x3` | Grundmatrix, aus der Instanzen ihre Transformationsmatrix kopieren. **Wahrscheinlich.** | `func_80013BD8` |
| `D_800A0300` | `gBoneMatrices` | `struct Matrix[]` | Matrix je Knochen-/Segmentindex; Eintrag `i-1` ist der Elternknoten von `i`. **Wahrscheinlich.** | `func_80013CB8` |
| `D_8009F588` | `gCameraList` | `void *` | Kopf der Liste der Transform-Sätze; wird beim Init auf 0 gesetzt und beim Durchlaufen verkettet. **Bestätigt.** | `func_8001774C`, `func_80017994` |
| `D_8009F58C` | `gNextCameraId` | `s32` | Fortlaufende ID für Transform-Sätze (`node->id = gNextCameraId++`). **Bestätigt.** | `func_80017994` |
| `D_8009F214` | `gRandSeed` | `s32` | Zustand des Zufallszahlengenerators (Multiplikation mit 16807, Park-Miller). **Bestätigt.** | `func_80019720` |
| `D_8009F5B8` | `gResourceListHead` | `struct Resource *` | Kopf der einfach verketteten Ressourcenliste; wird in mehreren Funktionen durchlaufen und am Ende freigegeben. **Bestätigt.** | `func_80019AA8`, `func_800198DC` |
| `D_800A07A8` | `gSoundSystem` | `struct SoundSystem` | Globaler Zustand des Klangsystems mit Bank-Liste und Anzahl. **Bestätigt.** | `func_80021508` |
| `D_8009F92C` | `gSaveProfileImage` | `u8 *` | 0x948 Byte großer Profilkontext des Speicherkartenpfads: Live-Kopf (0x200 Byte) bei `+0`, Zustandsbytes `+0x200`…`+0x208`, 0x678-Byte-Kartenabbild bei `+0x20C`, Optionsblock `+0x40C`, zehn 0x70-Byte-Sätze `+0x424`, Karten-Treiberzeiger `+0x904`. **Bestätigt.** | `func_8007F9CC`, `func_80081204` |

## Ton (SPU-Treiber)

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009EEBC` | `gSpuTransferAddr` | `u16` | Aktuelle SPU-RAM-Adresse für den FIFO-Transfer; wird in das Transferadressregister geschrieben. **Bestätigt.** | `func_80097138`, `func_80098EE8` |
| `D_8009EECC` | `gSpuAddrShift` | `s32` | Schiebewert zwischen SPU-Adresse und Byte-Offset (`<< gSpuAddrShift`), im Retail 3. **Bestätigt.** | `func_80097808`, `func_80098EE8` |
| `D_8009EEC8` | `gSpuAlignEnable` | `s32` | Schalter für die Adressausrichtung, im Retail 2. **Wahrscheinlich.** | `func_80096EB8`, `func_80097808` |
| `D_8009EED0` | `gSpuAlignBase` | `s32` | Basiswert der Ausrichtung, im Retail 8. **Wahrscheinlich.** | `func_80097808` |
| `D_8009EED4` | `gSpuAlignMask` | `s32` | Maske der Ausrichtung (`addr &= ~mask`), im Retail 7. **Wahrscheinlich.** | `func_80097808` |
| `D_8009EEE4` | `gSpuInitData` | `u8[]` | 0x10-Byte-Block, der beim Kaltstart nach SPU-RAM `0x200` übertragen wird. **Wahrscheinlich.** | `func_80096EB8` |

## Controller (SIO0)

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009C760` | `gPadPresentFlag` | `s32` | 1, sobald ein Controller erkannt wurde; steuert, ob die Ports abgefragt werden. **Bestätigt.** | `func_8008A290`, `func_8008A43C` |
| `D_8009C768` | `gPadSlotIndex` | `int` | Laufender Port-Index beim Abfragen der Slots (Einträge liegen bei `Basis + Index * 240`). **Bestätigt.** | `func_8008A43C`, `func_8008A6CC` |
| `D_800A6FF8` | `gPadActuators` | `Slot[]` | Ein Eintrag pro Port für den Aktuator-Zustand (Rumble-Motoren). **Wahrscheinlich.** | `func_8001E9AC`, `func_8001E87C` |

## Controller-Puffer

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_800AAC50` | `gPadMappingTable` | `struct PadMapping[]` | Tastenzuordnung je Port. **Bestätigt.** | `func_8001E588` |
| `D_800A78F0` | `gPadBuffers` | `struct PadBuffer[]` | Empfangspuffer je Port, aus dem die Pad-Bits gelesen werden. **Wahrscheinlich.** | `func_8001E7B8`, `func_8001E9AC` |

## CD-ROM-Treiber

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009CB84` | `gCdStateBytes` | `u8[]` | Drei Zustandsbytes des Treibers; nach einem Reset steht `[0] = 2`, während einer Antwort werden sie fortgeschrieben. **Bestätigt.** | `func_8008F6D8`, `func_8008E7A0` |
| `D_8009C8B4` | `gCdStatusByte` | `u8` | Letztes Statusbyte einer CD-Antwort; wird unter anderem auf Bit `0x10` geprüft. **Bestätigt.** | `func_8008E7A0`, `func_8008E010` |
| `D_8009C8B0` | `gCdLogLevel` | `int` | Größer 0 schaltet die CD-Protokollierung ein; der Setter gibt den vorherigen Wert zurück. **Bestätigt.** | `func_8008E0D8`, `func_8008E7A0` |
| `D_8009C8BC` | `gCdResponseCount` | `int` | Zähler der bearbeiteten CD-Antworten, wird pro Antwort erhöht. **Bestätigt.** | `func_8008E7A0`, `func_8008FF20` |
| `D_8009CBC4` | `gCdSeenResponseCount` | `s32` | Zuletzt verarbeiteter Stand von `gCdResponseCount`; eine Abweichung löst das Neulesen des Zustands aus. **Bestätigt.** | `func_8008FF20` |

## Spieler und Match

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009FA0C` | `gPlayerCount` | `s32` / `int` | Anzahl der Spieler im laufenden Match (1 = Einzelspieler, 2 = Zweispieler); viele Pfade verzweigen darauf. **Bestätigt.** Die Funktionsdoku in `src/game/8002D1A0.c` nennt sie „player count". | `func_8002D1A0`, `func_80027510` |
| `D_8009F9EC` | `gSinglePlayerEntity` | `struct Ent *` / `Game *` | Zeiger auf die Spielfigur im Einzelspielermodus; wird nur bei `gPlayerCount == 1` ausgewertet und über die Entity-Id verglichen. **Bestätigt.** | `func_8002D1A0`, `func_80027510` |
| `D_8009F8B8` | `gActivePlayerIndex` | `s32` | Index des Spielers, dessen Menü aktiv ist (1 bei Zweispieler-Menü, sonst 0). **Bestätigt.** | `func_8007A7A0`, `func_8007A5F0` |

<!-- OFFEN: gPlayerEntities (Zwei-Slot-Array der Spieler-Entities) bleibt vorerst
     unbenannt. In src/game/80027510.c wird dasselbe Symbol doppelt deklariert
     (einmal als Union `gPlayerEntities`, einmal als Array `D_8009FA58_arr`); eine
     Umbenennung verändert dort die erzeugten Bytes. Erst die Doppeldeklaration
     auflösen, dann benennen. -->

## Upload-Pfad und Renderer-Hooks

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F3E5` | `gUploadRequestFlag` | `u8` | Anforderungsflag für den Upload-Pfad; die Funktion läuft nur bei 0 und löscht es am Ende. **Bestätigt.** | `func_80025370` |
| `D_8009F9C4` | `gUploadInProgress` | `s32` | Upload-Sperre: während des Ladens auf 1, danach wird der alte Wert zurückgeschrieben; steuert die Verzweigung auf den „PA"-Loader. **Bestätigt.** | `func_80025370` |
| `D_8009F68C` | `gScratchBlockPtr` | `s32` | Zeiger auf einen Arbeitsblock, der beim Init einmalig freigegeben wird (nur wenn ungleich 0). **Bestätigt.** | `func_80024D14` |
| `D_8009F2E4` | `gFormatNamePair` | `u8[]` | Format-String `"%s%s"`, mit dem Präfix und Name zusammengesetzt werden. **Bestätigt.** | `func_80024E3C`, `func_80025370` |
| `D_8009C744` | `gReadyCheckHook` | `s32 (*)(void)` | Hook, der die Bereitschaft eines Treiberobjekts abfragt; `func_8008B168`/`func_8008B210` werden dort installiert. **Bestätigt.** | `func_8008B094` |

Nicht benannt, weil ein Rename die Codegen bricht (das Symbol wird in den betroffenen Units zusätzlich lokal definiert bzw. ohne `__asm__` gebunden): `D_8009C728` (Dispatch-Zeiger), `D_8009C72C` (Handler ohne Modus), `D_8009C73C` (State-Abfrage).

## Rest ohne Datei-Beleg: zehn Fälle einzeln geklärt

Zehn Symbole, die keinen erklärenden Kommentar in ihrer Unit hatten. Drei waren am Code direkt lesbar, sieben wurden mit Gemini 4 Hydrogen geklärt und die genannten Stellen anschließend selbst geprüft.

| Global (RAM) | C-Name | Bedeutung | Beleg |
|---|---|---|---|
| `D_8009DDF4` | `gMaxDrawWidth` | Obere Grenze der Breite: `buf[2]` wird auf `Wert - 1` geklemmt. | `80093AF0.c:64` |
| `D_8009DDF6` | `gMaxDrawHeight` | Dasselbe für die Höhe (`buf[3]`), in `80094010` als `maxw`/`maxh` gelesen. | `80093F78.c:33` |
| `D_8009F590` | `gFreeRangeTablePtr` | Zeiger auf den 0x810-Byte-Frei-Bereichsblock (Tag 3), dessen zweites Wort `gFreeRangeCount` ist. | `80017BE8.c:46` |
| `D_8009F8BE` | `gUiInteractionEnabled` | Freigabe-Flag der Frontend-Eingabe: wird gesetzt, bevor die Auswahlschleife Eingaben annimmt, und bei Film-/Kartensperren gelöscht. | `8007FACC.c:27`, `8007F8E0.c:36`, `80078DD0.c:188`, `8008125C.c:189` |
| `D_8009F458` | `gCrimeWarProgress` | Freigeschalteter Missionsfortschritt im Crime-War-Modus (0…8); der Zähler wird bei Treffer auf `gSelectedMissionIndex` erhöht. | `800782EC.c:61-67`, `8007A168.c:47` |
| `D_8009F3F1` | `gSavedCrimeWarProgress` | Zuletzt bestätigter Stand dazu; weicht er ab, setzt der Code `gRowClearFlag = 1` (ungesicherter Fortschritt). | `800782EC.c:70-72` |
| `D_8009F3F8` | `gHighlightShadeTable` | Achtstufige Helligkeitstabelle für pulsierende Icons und Rahmen, gelesen mit `Pegel + 0x80`. | `80078DD0.c:291-292`, `8007ABBC.c:185` |
| `D_8009F880` | `gSelectedMissionIndex` | Cursor in der Missionsliste, läuft zwischen `D_8009F881` und `D_8009F882` zyklisch. | `8007B854.c:212-214` |
| `D_8009F882` | `gMissionListMaxIndex` | Inklusive Obergrenze der wählbaren Einträge (7 im Crime-War-Modus, `gUnlockFlag + 3` in Precinct Assault). | `800776B8.c:43`, `80077708.c:67` |
| `D_8009F4C4` | `gStripDitherEnabled` | Begleitflag zur Streifenskalierung: 1 bei `gStripScaleEnable == 1` (Step 3), 0 im Step-2-Modus. | `800859D8.c:30-35` |

**Nicht umbenannt:** `D_8009FA5C` — es ist nur das zweite Element hinter `gPlayerEntities` (die `bp--`-Schleife in `80024A24.c:175-181` nullt beide rückwärts), und die Units deklarieren es unvereinbar (`struct {…}`, `_b`…`_e`, `s32[]`). Dieselbe Sperre wie bei `gPlayerEntities`.

## Letzte belegte Runde und die Grenze des Benennbaren

Einundsechzig weitere Globals in **einem** Commit. Damit sind alle Symbole abgearbeitet, deren Rolle in den Datei-Dokus belegt ist; die 63 übrigen Kandidaten sind unten mit Grund aufgeführt.

### Frontend-Namen und Formate

| Global (RAM) | C-Name | Bedeutung |
|---|---|---|
| `D_80010DCC` | `gTimeMmSsFormat` | Format für die mm:ss-Ausgabe, neben der Dezimalvariante. |
| `D_800117C0` | `gCounterFormatString` | Format des Zählertextes, gezeichnet bei `x + 138 - Breite`, y 20. |
| `D_80011E3C` | `gEaLogoName` | Dateiname `"Ea_logo.fsv"`, gezeichnet bei (0,0) mit Modus 4. |
| `D_80011E60` | — | Dateiname `"credits.wve"`, beim Abspann verwendet. |
| `D_8009F8E8` | `gTitleScreenDeadline` | Bestätigt: Ablaufzeit des Titelbildschirms in Ticks; initialisiert in `func_8007799C`, gelesen in `func_800778E4`. |
| `D_8009F8CC` | `gMenuSetupCalled` | Bestätigt: verhindert wiederholten Aufruf von `func_80077990`; beim Menüwechsel gelöscht. |
| `D_8009F45A` | `gFrontEndReturnMode` | Bestätigt: steuert die Rückkehr ins Frontend; wird unter anderem bei Missionsstart und Demoablauf gesetzt und von `func_800782EC` ausgewertet. |
| `D_8009F890` | `gMenuUpHoldTicks` | Bestätigt: Start-Ticks der Aufwärts-Eingabe für zwei Spieler; `func_80078DD0` setzt sie, `func_800785F8` löscht sie. |
| `D_8009F898` | `gMenuDownHoldTicks` | Bestätigt: Start-Ticks der Abwärts-Eingabe für zwei Spieler; `func_80078DD0` setzt sie, `func_800785F8` löscht sie. |
| `D_8009F8A0` | `gMenuLeftHoldTicks` | Bestätigt: Start-Ticks der Links-Eingabe für zwei Spieler; `func_80078DD0` setzt sie, `func_800785F8` löscht sie. |
| `D_8009F8A8` | `gMenuRightHoldTicks` | Bestätigt: Start-Ticks der Rechts-Eingabe für zwei Spieler; `func_80078DD0` setzt sie, `func_800785F8` löscht sie. |
| `D_800120A4` | `gProfileImageName` | Name, den die Profilbild-Speicherschleife an `func_80080B70` übergibt. |
| `D_8009BBF8` | `gMainMenuItems` | Sieben Hauptmenü-Einträge zu je 0x1C Bytes; zwei können zur Laufzeit freigeschaltet werden. |
| `D_8009C030` | `gRowRecordTable` | Quelle der Zeilendatensätze (y 0x35/0x5F/0x89). |
| `D_8009F3E6` | `gLayoutPointerFlagA` | Beeinflusst die Zeigerzuweisung, nur bei `a0 == 3` erreicht. |
| `D_8009F3E7` | `gLayoutPointerFlagB` | Dasselbe für die abschließende Zeigerauswahl. |
| `D_8009F3F4` | `gPublishedBlockPtr` | Genullter und veröffentlichter Blockzeiger mit fester Lebensdauer. |
| `D_8009F8B0` | — | **Nicht umbenannt** (Byte-Vergleich kippt): wählt die Ereignisse für den Reset. |
| `D_8009F8BC` | `gPanelSyncValue` | Wird aus `panel[8]` nachgezogen. |
| `D_8009F8BD` | `gUiHoldFlag` | Wird gelöscht, wenn der Zähler bei `0x921` auf 0 steht. |
| `D_8009F8C4` | `gPendingRowCapture` | Hält die Zeile fest, sobald der Wert 10 (der letzte Eintrag) erreicht ist. |
| `D_8009F8F8` | `gScalarCursor` | Skalarer Cursor, den die Zustände 1 bis 4 schreiben. |
| `D_8009FA70` | `gScreenResetFlag` | Wird genullt, danach läuft `func_800780DC` in fester Reihenfolge. |
| `D_8009FAB0` | `gRemainingCounter` | Restzähler, um `object->unkA4` verringert. |
| `D_8009FAC4` | `gArmedFlag` | Wird mit 1 scharfgeschaltet, parallel zu `+0x201` mit 2 und `+0x202` mit 1. |
| `D_800A3718` | `gMenuActivationBlock` | Wird an `func_80078650` gegeben; dabei werden `+0x200` bis `+0x204` mit 5, 0, 0 beschrieben. |
| `D_800A374D` | `gPortCheckTable` | `[0] == 3` schaltet Port 7 frei, gezeichnet bei (0xA0, 0x69). |
| `D_800A3808` | `gMenuDescriptor` | Menü-Beschreibung, von `func_80078650` ohne Push aktiviert. |
| `D_800A3838` | `gScreenDescriptor` | Bildschirm-Beschreibung; die Display-Ids 0x44C und 0x10CC werden dort eingereiht. |

### BIOS-, IRQ- und Rücksprungmerker

| Global (RAM) | C-Name | Bedeutung |
|---|---|---|
| `D_800A3B64` | `gCallbackTable2` | Callback-Tabelle, mit zwei Funktionszeigern befüllt. |
| `D_800A3B80` | `gCombinationPtr` | Ziel- und Laufzeiger für Kombinationsdaten. |
| `D_800A3C20` | `gCompareAddress` | Vergleichsadresse einer Bereichsprüfung. |
| `D_800A3E00` | `gIrqNode` | IRQ-Knoten, den die Geräteinitialisierung installiert. |
| `D_800A3E10` | `gSavedRaScratch` | Merker für `$ra` neben den Sprungtabellen `jtbl_800A3E18`/`jtbl_800A3E1C`. |
| `D_800A3E20` | — | Parkt `$ra` um die BIOS-Abfrage `0xB0/0x57`. **Nur Inline-Asm, kein C-Name setzbar.** |
| `D_800A3E40` | `gRaParkPatch` | Parkt `$ra` um das Überschreiben von 12 Bytes bei `*(B0(0x56)+0x18)+0x70`. |
| `D_800A3E50` | `gStatusBuffer8` | 8-Byte-Puffer, der bei Status 2 oder 5 gefüllt wird. |
| `D_800A3E68` | `gPollDeadline` | Frist aus aktuellem Tick plus 0x3C0. |
| `D_800A3E6C` | `gPassCounter` | Durchlaufzähler der Abfrageschleife, startet bei 0. |
| `D_8009DF10` | — | Parkt `$ra` um den Aufruf von `func_80095DB0`. **Nur Inline-Asm, kein C-Name setzbar.** |

### Grafik-, Tabellen- und Treiberhelfer

| Global (RAM) | C-Name | Bedeutung |
|---|---|---|
| `D_8002139C` | `gPerFrameHook` | Pro-Frame-Hook, den der Interrupt-Pfad aufruft. |
| `D_80075698` | `gAttachMessageName` | Meldung 0 derselben Datei, Anhänge-Pfad. |
| `D_80088D74` | `gCodeDataSource` | Datenquelle; `func_800889DC` liefert die Code-/Datenbasis für das zweite Handle. |
| `D_80095808` | `gAbortCallbackId` | Wird nach dem Abbruchpfad `func_8009520C()` an `func_800914E0(2, …)` gegeben. |
| `D_8009F610` | `gCdCommandParam` | Wert, der mit `func_8008E1BC(0xE, …)` gesendet wird; ein Fehler liefert `-4`. |
| `D_8009AF88` | `gDescriptorBlock` | Deskriptor, dessen Zufallswort bei `&Wert - 0x10` landet. |
| `D_8009C7A8` | `gPadUpdateByte` | Wird im Pad-Pfad fortgeschrieben; aufgerufene Funktionen können weitere Wirkung haben. |
| `D_8009C800` | `gModeClearFlag` | Löscht das Modusargument, wenn `func_8008D7…` es erlaubt. |
| `D_8009CC8A` | `gIrqDispatchBusy` | Busy-Halbwort im Dispatcher-Block `D_8009CC88`; `gDispatchIrqs` prüft es, `func_80091B60` löscht es, `func_800915D4` gibt es zurück. |
| `D_8009DD30` | `gVSyncCallbackTable` | Acht Funktionszeiger, die der VSync-Handler `func_80091BF8` je Bild der Reihe nach aufruft; `gInitVSyncHandler` setzt die Tabelle zurück. |
| `D_8009EA5C` | `gModeEntryWord` | Wird von den unterstützten Modi als flüchtiger Eintrag geschrieben. |
| `D_8009EED8` | `gSpuClearFlag` | Kann beim Aufräumen gelöscht werden. |
| `D_8009EEDC` | `gSpuTransferCallback` | Abschluss-Callback des SPU-DMA-Transfers; wird in `func_800972F8` gerufen und sonst durch `func_8008DFF0(0xF0000009, 0x20)` ersetzt. **Bestätigt.** |
| `D_8009F324` | `gMessageConeFlag` | Teil des Meldungsdatensatzes, steuert den Kegeltest. |
| `D_8009F6F0` | `gGpStateWord` | 32-Bit-Zustandswort bei `0x4DC($gp)`, genullt. |
| `D_8009F7AC` | `gMessageDoneFlag` | Wird vor der Vorverarbeitung gelöscht, wenn gesetzt. |
| `D_8009FA10` | `gRowClearFlag` | Bei 0 werden die Bytes `+0x1F`/`+0x20` aller 16 Einträge genullt. |
| `D_8009FA18` | `gInitStateWord` | 32-Bit-Zustandswort, genullt. |
| `D_800A22A0` | `gOwnerNodeListA` | Knotenliste, die `func_8005BCD4(&…, owner)` durchsucht. |
| `D_800A22B0` | `gOwnerNodeListB` | Zweite Liste für denselben Besitzer-Lookup. |
| `D_800A22C0` | `gLockEndpoints` | Globale Sperre mit zwei Endpunktzeigern (`+0`/`+4`). |
| `D_800A3958` | `gScratchTable` | 0x100 Bytes, in den Scratchpad kopiert. |
| `D_800A3A58` | `gSecondScratchTable` | Zweite Tabelle, Zielschrittweite 16 Bytes je Zeile. |
| `D_800A62C8` | `gGpuLibPair` | Wort- und Bytefeld, das die GPU-Bibliothek mitführt. |
| `D_800A6D78` | `gFileSlotTable` | Dateislots des Loaders, indiziert über `func_80010000.index`. |
| `D_800A6E18` | `gCountWrapLimit` | Umschlaggrenze der Zählung (0x1F801128 oder 0x10000). |
| `D_800A6E38` | `gExceptionFrame` | Nimmt `$at`, `$v0` und die COP0-Cause beim Ausnahmezustand auf. |
| `D_800A6F60` | `gMarkerColorTable` | Größe je Effekttyp: `[type & 31]`, Größenklasse `type >> 5`. |
| `D_800A6FE0` | `gPlayerNameTable` | Spielernamen im Header, `+ slot * 9`. |
| `D_800A7028` | `gSpuInitWord` | Wort, das beim SPU-Aufsetzen mitgeschrieben wird. |
| `D_800A8540` | `gRandomTable` | Tabelle für den Zufalls-Opcode 0x0B, ein nicht-negativer Operand indiziert sie. |
| `D_800A85C0` | `gLengthTable` | Längentabelle, aus der `func_8008CD10` Einheiten kopiert. |
| `D_800A8EA6` | `gEffectUvV` | Feste V-Koordinate der 3×3-Effektfläche (U ist 128/201 als Konstante). |

### Die 63 Kandidaten, die stehen bleiben

**Handler- und Callback-Adressen.** Sie werden nur als `handler`-Feld registriert oder indirekt über den Dispatcher erreicht; der Beleg nennt keine Aufgabe: `D_8001C808`, `D_8001EFF8`, `D_8002DEA8`, `D_8003190C`, `D_80036E6C`, `D_80037708`, `D_8003B2D0`, `D_80044610`, `D_800578B4`, `D_80059ABC`, `D_8005B934`, `D_8005CA04`, `D_8006E814`, `D_8007D150`, `D_8007D18C`, `D_8007D27C`, `D_8007F618`, `D_80080038`, `D_80085074`, `D_80086950`, `D_80086D74`, `D_800A36B8`, `D_800A36E8`, `D_800A3778`, `D_800A38F8`.

**Byte-Vergleich kippt nach dem Rename** (Symbol zusätzlich lokal definiert oder ohne `__asm__` gebunden): `D_8009BEF4`, `D_8009C810`, `D_8009F614`, `D_8009F79C`, `D_8009F7C0`, `D_8009F8B0`, `D_800A3E80`, `D_800A445C`, `D_800A4480`, `D_800A4484`. Dazu `D_8009C8A8`/`D_8009C8AC` (Mehrfachdeklaration mit abweichenden Typen). `D_8009F4D8` stand hier früher auch: der Block wird in über zwanzig Units mit jeweils eigenem Struct-Typ deklariert, deshalb half nur ein Name mit `__asm__`-Bindung **pro Unit** (jetzt `gMovieAudioState`).

**Nur Inline-Asm, kein C-Name setzbar:** `D_8009EA00`, `D_8009F7C8` (in `8001C808.c` von Hand gesetzt, in weiteren Units asm-only), `D_A000DFAC`.

**Bedeutung nicht bestimmbar:** `D_80010000` (erstes Wort des Programmbilds), `D_80010B0C`, `D_8004C324`, `D_8008DCC0`, `D_8008DCD4`, `D_800908D4`, `D_8009EF04`, `D_8009F628` (nur „direkter Schreibzugriff"), `D_8009F670` (Konfiguration oder Fahrzeugindex), `D_8009FA99`, `D_800A0974` (nur „direkter Schreibzugriff"), `D_800A09DC` (Wert unbelegt), `D_800A0C10` (Teil einer Vierer-Gruppe), `D_800A2730`, `D_800A3938`, `D_800AB388`.

## Frontend-Pfad: Hauptmenü bis Precinct Assault

Die Variablen, die den Weg **Hauptmenü → 1 Player → Precinct Assault → Aufstellungsbildschirm → Hub → Levelauswahl** tragen. Grundlage ist die Menü-Record-Tabelle in `asm/us/main.s` sowie die Einträge der Retail-EXE; die Missions- und Filmtabellen sind zusätzlich über die Host-Tests `tests/host/precinct_path_harness.c` abgesichert.

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009B730` | `gFrontEndEntrySequence` | `struct Entry78C88[5]` | Eintragsfolge des Frontends: je vier Bytes aus Level-Index, Spieltyp, Spielerzahl und einem vierten Wert. Wird beim Aufbau der Menüs abgearbeitet. **Bestätigt.** | `func_80078C88` |
| `D_8009BB66` | `gMenuPreviewScaleP1` | `s16` | Vorschau-Skalierung des ersten Spielers im Panel-Block `D_8009BB34` (Feld `+0x32`); wird beim Bestätigen der Spieleraufstellung auf 100 gesetzt. **Wahrscheinlich** (Feldrolle aus dem Schreibmuster, Einheit der Skala offen). | `func_8007A5F0` |
| `D_8009BBBA` | `gMenuPreviewScaleP2` | `s16` | Dasselbe für den zweiten Spieler (Feld `+0x86` desselben Blocks). **Wahrscheinlich.** | `func_8007A5F0` |
| `D_8009F881` | `gMissionListMinIndex` | `u8` | Untere Grenze des Missionscursors; beide List-Setups und die Ladepfade setzen sie auf 0. **Bestätigt.** | `gSetupCrimeWarMissionList`, `gSetupPrecinctAssaultMissionList` |
| `D_8009F884` | `gMissionListTablePtr` | `u8 *` | Zeiger auf die aktive Missionsliste: Crime War → `gMissionListTable` (8 Einträge), Precinct Assault → `gFrontEndListTable` (5 Einträge). **Bestätigt.** | `gSetupCrimeWarMissionList` |
| `D_8009F888` | `gMissionListEntryCount` | `u8` | Zahl der Einträge der aktiven Liste: 8 bei Crime War, 4 + Freischalt-Flag bei Precinct Assault. **Bestätigt.** | `gSetupPrecinctAssaultMissionList` |
| `D_8009F889` | `gMissionListMode` | `u8` | Art der aktiven Liste: 0 = Crime War, 1 = Precinct Assault; adressiert damit das Fortschrittspaar `D_8009F458`/`D_8009F459`. **Bestätigt.** | `gSetupPrecinctAssaultMissionList` |
| `D_80012088` | `gCwRulesMovieName` | `char[12]` | Zeichenkette `"cwrules.wve"`: das Regelvideo des Crime-War-Modus, das die Einträge 0 und 1 des Hauptmenüs einmalig einreihen. **Bestätigt.** | `gSelectCrimeWar1P` |
| `D_80012094` | `gMultiBrfMovieName` | `char[12]` | Zeichenkette `"MultiBrf.wve"`: das Briefingvideo beim ersten Precinct-Assault-Einstieg. **Bestätigt.** | `gSelectPrecinctAssault1P` |

## Steuerung, Skript-VM, Panels und CD-Treiber: die große Runde

Hundert weitere Globals in **einem** Commit. Drei Kandidaten kippten beim Byte-Vergleich (`D_8009BEF4`, `D_8009C810`, `D_8009F614`) und bleiben stehen. Außerdem sind vier Namen aus der vorigen Runde nachgetragen, die in der Doku fehlten: `gDeferredRegList` (`D_8009F708`), `gDeferredRegPool` (`D_8009F70C`), `gPendingMessageMask` (`D_8009F7A8`) und `gRegistryTable` (`D_8009F800`).

### Objekt- und Renderhelfer

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F348` | `gPlayerAvatarSprite` | — | Sprite je Spieler, gezeichnet bei y 74, sobald `player+0xAE` positiv ist. |
| `D_8009F374` | `gSpawnMotionContext` | — | Startet eine Bewegung, wenn gesetzt; sonst erhält das Objekt einen Callback. |
| `D_8009F388` | `gTracerEdgeColour` | — | Farbwort: `(Wert >> 8) \| 0x3A000000`, Farbe ist `field40 >> 8`. |
| `D_8009F45C` | `gAtlasTable` | — | Atlas mit `count`; der Index muss darin liegen. |
| `D_8009F460` | `gSpriteAtlasBlock` | — | Wird vor dem Neuaufbau freigegeben; bei Typ ungleich 2 wird ein Flag gesetzt. |
| `D_8009F468` | `gFontBankTable` | — | Zeiger auf den **aktiven Front-End-Menüschirm**: `[0]` ist der Cursor der Gruppe 0, `+2` die Eintragszahl, `+0x24` der Elternschirm, `+0x28` die Eintragstabelle. Der C-Name stammt aus einer früheren Runde und ist falsch; die Rolle ist über 37 Units belegt. |
| `D_8009F508` | `gReleaseDoneFlag` | — | Wird im Completion-Callback gesetzt. |
| `D_8009F5BC` | `gResourceListTail` | — | Bei ungleich 0 wird hinter dem Ende angehängt, sonst Kopf und Ende gesetzt. |
| `D_8009FABC` | `gCharTable` | `u8[]` | 0x100-Byte-Zeichentabelle; der Zeiger rückt um 16 weiter. |
| `D_8009F284` | `gGlobalTick` | — | Globaler Tick, gegen den die Frame-Helfer messen. |
| `D_8009FA30` | `gFrameTickBase` | — | Wartet mindestens 5 Ticks seit diesem Wert und aktualisiert das Frame-Delta. |
| `D_8009FA64` | `gCameraNearBase` | — | Erhält `(Wert << 8) / 608` für die Nahgrenze. |

### Skript-VM, Listen und Tabellen

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F2CC` | `gLabelRecord` | — | Datensatz mit drei Labelwerten (`unk04/unk08/unk0C`). |
| `D_8009F2F4` | `gBannerString` | — | Banner-String für `func_80049100(…, 0x41, 0x14, 1)`. |
| `D_8009F30C` | `gDeferredSpawnList` | — | Liste der NodeC-Einträge; passende werden entlinkt und freigegeben. |
| `D_8009F310` | `gSpawnTableRow` | — | Zwischengespeicherte Basis, wird auf 0 zurückgesetzt und über die Chunks iteriert. |
| `D_8009F318` | `gLightObjectCount` | — | Zähler der Meldung 10. |
| `D_8009F31C` | `gBatchSpawnActive` | — | Wird gesetzt, solange „keine Zelle" gilt. |
| `D_8009F320` | `gAnchorNodeList` | — | Kopf der Knotenliste, Durchlauf mit vorab gesichertem `next`. |
| `D_8009F328` | `gConeTestFlag` | — | Nur bei ungleich 0 läuft der Kegeltest. |
| `D_8009F329` | `gBandTestFlag` | — | Bit 0 aktiviert den Bandtest. |
| `D_8009FA48` | `gChainedNodes` | — | Vier Knoten der Schrittweite 0x6014, angehängt beim Aufbau. |
| `D_8009FA40` | `gUnitListHead` | — | Kopf der Einheitenliste, über `next` bis null durchlaufen. |
| `D_8009F9D8` | `gByteTable` | `u8[]` | Byte-Tabelle, in die das Ergebnis indiziert; drei Bytes werden gepackt. |
| `D_8009F9F8` | `gOpcodeTable` | — | 32 Einträge; ein negativer Wert führt `func_80019668()` modulo. |
| `D_8009FA08` | `gTableBuffer` | — | Basisadresse des allokierten Tabellenpuffers. |
| `D_8009F9D0` | `gFixedPool1` | — | Erster Pool: Größe `a0`, Anzahl `a1`. |
| `D_8009FAAC` | `gFixedPool2` | — | Zweiter Pool: Größe `a2`, Anzahl `a3`. |
| `D_8009FA4C` | `gFixedPool3` | — | Dritter Pool: Größe `a4`, Anzahl `a5`. |
| `D_8009F800` | `gRegistryTable` | — | 0x80-Slot-Registertabelle (Nachtrag). |
| `D_8009F708` | `gDeferredRegList` | — | Einfach verkettete Liste `{next, object}` (Nachtrag). |
| `D_8009F70C` | `gDeferredRegPool` | — | Pool aus 256 Acht-Byte-Knoten (Nachtrag). |
| `D_8009F7A8` | `gPendingMessageMask` | — | Bitmaske ausstehender Meldungen, je erledigtem Eintrag gelöscht (Nachtrag). |
| `D_8009F7C8` | `gClipLimitOffset` | — | Wird von 200 abgezogen, wenn die Klemmung greift. |
| `D_8009F7D0` | `gStreamDescriptorPtr` | — | Zeiger auf den Stream-Deskriptor. |
| `D_8009F7F0` | `gMessageReturnData` | — | Rückgabedaten bei Meldung 4 mit `arg3 == 1`, sonst 0. |

### Frontend, Panels und Mission

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009B42C` | `gScreenIndexTable` | — | Bildschirm-Index des Frontends. |
| `D_8009B4E4` | `gWideTextTable` | — | Tabelle, aus der `[table[i]].unk08` in breiter Schrift gezeigt wird. |
| `D_8009BEF4` | — | — | **Nicht umbenannt** (Byte-Vergleich kippt): 14 Wörter im 12-Byte-Schritt. |
| `D_8009BFD4` | `gPresetByteTable` | — | Vier Voreinstellungen als 14 aufeinanderfolgende Bytes, `src = Basis + Wert * 14`. |
| `D_8009AD08` | `gViewPresetTable` | — | Voreinstellungstabelle, die auch die View-Handler nutzen. |
| `D_8009AEC4` | `gPresetEntryTable` | — | Voreinstellungs-Eintrag; Feld `+0xA4` liefert den Wert. |
| `D_8009AFA0` | `gSourceBlock20` | — | 20-Byte-Block, den `func_8005A070` kopiert. |
| `D_8009ABED` | `gItemLabelByte` | `u8` | Label-Byte zwischen 8 und 7; 8 ruft `func_80060A0C`. |
| `D_8009F65C` | `gTeamFlag` | — | Mannschafts-Flag, schützt den Kamerapfad. |
| `D_8009F680` | `gWasSuspended` | — | Wird auf 1 gesetzt, wenn Menümodus 0 oder `unk312` gesetzt ist. |
| `D_8009F698` | `gResourceSubChunkPtr` | — | Veröffentlichter `0x74544F53`-Subchunk-Zeiger einer `0x43746F73`-Ressource. |
| `D_8009F69C` | `gRegisterGuard` | — | Einzige Wächterprüfung auf 0, danach geradliniger Rumpf. |
| `D_8009FA6C` | `gOpenPanelCount` | — | Auswahl im Panel; Code 0 verlässt es mit Sound 0x3E und `-1`. |
| `D_8009FA80` | `gSlotIndexLimit` | — | Vergleichswert, bevor der Index weiterläuft. |
| `D_8009FA88` | `gListTailBranch` | — | Einzelne Verzweigung: Anhängen am Ende oder Kopf-Initialisierung. |
| `D_8009FAB8` | `gHudMode2` | — | Bei Wert 2 wird das HUD aktiviert. |
| `D_8009FAC0` | `gPanelSoundIds` | — | Sound-Ids 0x2F/0x30 des Panels, danach mischt `func_80025F58` neu. |
| `D_8009FAC8` | `gFrameSwitch` | — | Frame-Umschalter; bei 0 kehrt der Aufruf sofort zurück. |
| `D_8009FAD8` | `gSavedStackPointer` | — | Wird genullt, danach installiert `func_8001E87C` die Geräte-Hooks. |
| `D_8009F870` | `gIdleDeadline` | — | Leerlauf-Frist, um 0x2328 Ticks vorgestellt. |
| `D_8009F874` | `gOverlayDelay` | — | Wird auf `-1` gesetzt, nachdem die Fläche gesetzt wurde. |
| `D_8009F904` | `gSnapshotResult` | — | Momentaufnahme, danach zwei `func_8001EB2C`-Aufrufe und Vergleich mit 1. |
| `D_8009F918` | `gTestResultFlag` | — | Bei erfolgreichem Test auf 1 gesetzt. |
| `D_8009F91C` | `gSnapshotPointer` | — | Wird freigegeben, wenn er ungleich 0 ist. |
| `D_8009F920` | `gPreviewTrackId` | — | Track-Id, für die die Vorschau-Voice gestartet wurde. |
| `D_8009F92A` | `gStatusTextId` | — | Status-Text-Id (0, solange nichts läuft). |
| `D_8009F944` | `gMissionClearFlag` | — | Wird genullt und sonst nirgends geschrieben. |
| `D_8009F968` | `gLayerModeBits` | — | Bit 0 wählt die schmale Variante (`0x27`). |
| `gFocusPositions` | `gFocusPositions` | — | Wird genullt, bevor `func_80017C68(0x168, …)` die Pools anlegt. |
| `D_8009F9B0` | `gStreamPreemptFlag` | — | Markiert beim Verdrängen abgebaute Stream-Voices. |
| `D_8009F9CC` | `gDisplaySetupToggle` | — | Wechselt zwischen 0 und 1 nach dem Display-Aufbau. |
| `D_800A0AA0` | `gMapOpenPanelAction` | — | Fünf Acht-Byte-Aktionsdatensätze (Tastenmaske, Phase). |
| `D_800A0B00` | `gMapDigit3Action` | — | Phasenfeld je Slot, an `func_800260B8` übergeben. |
| `D_800A0B10` | `gMapDigit4Action` | — | Zweite Auswahltabelle, liefert Auswahl 4. |
| `D_800A0B20` | `gBackActionRecord` | — | Zurück-Aktion (Phase 0), schließt das Panel. |
| `D_800A085C` | `gSubsystemTable` | — | Tabelle, die beim Hochfahren des Subsystems einmal gefüllt wird. |
| `D_800A09D4` | `gMode1IndexBase` | — | Indexbasis für Modus 1. |
| `D_800A09F0` | `gCurrentChannel` | — | Aktueller Kanal; `-1` ist die Markierung „kein Kanal". |
| `D_800A0C30` | `gReleaseQueues` | — | Beide aufgeschobenen Freigabe-Warteschlangen für Asset-Handles. |
| `D_800A0C50` | `gVagmRing` | — | VAGM-Ring; ausstehende Einträge werden über `func_8002A31C` abgearbeitet. |
| `D_800A1070` | `gListHeads` | — | Sieben Listenköpfe; der neue Kopf wird dort eingetragen. |
| `D_800A10E0` | `gPlayerSlots` | — | Drei Slots je Spieler, belegt/leer geprüft. |
| `D_800A1160` | `gVoiceTable` | — | 52-Byte-Einträge, nur für nicht-negative Kanalindizes. |
| `D_800A1920` | `gPositionAngleTable` | — | Zieltabelle für Position und Winkel. |
| `D_800A19A0` | `gGlyphEntryTable` | — | Einträge, deren `glyphs`-Zeiger `func_800181D4` freigibt. |
| `D_800A2630` | `gTilePatternBuf` | — | Verlaufsvariante des k==6-Zweigs. |
| `D_800A2931` | `gSlfxFlagByte` | `u8` | SLFX-Flag, beim Zerlegen der Karten-Chunks geschrieben. |
| `D_800A2940` | `gHeightGridDesc` | — | Nimmt Ausgabezeiger und Metadaten der Zelldaten auf. |
| `D_800A3190` | `gViewGroundAnchors` | — | Positionspaare, die `func_80066C9C` für die Szene schreibt. |
| `D_800A31A0` | `gObjectKeyTable` | — | Schlüsseltabelle, über die `func_80063194` Objekte auflöst. |
| `D_800A3578` | `gSlotRecordTable` | — | Je Slot drei `{marker, id}`-Paare. |

### Debug-Ausgaben und Zeichenhilfen

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8001223C` | `gHexUpperTable` | — | Zweite Hex-Tabelle mit Großbuchstaben, neben der Kleinbuchstaben-Tabelle. |
| `D_80012A88` | `gTraceFormatString` | — | Formatstring eines reinen Entwicklungs-Trace-Kanals. |
| `D_80012A94` | `gDebugTagRect` | — | Tag-String für `func_80092CF4(…, rect)`, den Debug-Wrapper. |
| `D_80012AA0` | `gDebugTagArg` | — | Tag-String der Debug-Hook-Variante mit einem Argument. |
| `D_80012AAC` | `gDebugTagRectEmpty` | — | Tag für denselben Wrapper; ein leeres Rechteck kehrt sofort zurück. |
| `D_8009DDF0` | `gDebugTagBytes` | — | Drei Bytes neben dem Flag, die zusammen mit dem Tag-String gelesen werden. |
| `D_8009DEB0` | `gSeqFirstWord` | — | Erstes Wort der Sequenz, das beim Umschreiben mitgesetzt wird. |
| `D_8009C560` | `gDefaultSymbolTable` | — | Ersatzquelle für 0x60 Symbole, wenn kein Puffer übergeben wird. |
| `D_8009C7D0` | `gSessionBlockBase` | — | Basisadresse des Sitzungsblocks, beim Anlegen von `func_80076EB0` gesetzt. |
| `D_8009CBEC` | `gReadWindowTable` | — | Tabelle, deren Wert `+4` (0x4B0 = 1200 Ticks) das Lesefenster begrenzt. |
| `D_8009CC24` | `gFrameTargetBase` | — | Basis der Frame-Ziele: `Ziel = Basis + input - 1` bei `input > 0`. |

### SPU-Arena, Tabellen und Wächter

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009EA34` | `gNormalisedMode` | — | Normalisierter Modus, den der Aufruf zurückgibt. |
| `D_8009EA3C` | `gArenaOffsetFlag` | — | Solange 0 ist der Offset 0, sonst rechnet der Pfad anders. |
| `D_8009EA40` | `gArenaCursor` | — | Einfügezeiger, der die Arena-Position mitführt. |
| `D_8009EA8E` | `gSpuInitWords` | — | 0x18 Wörter, die `func_80096EB8(0)` beim Aufsetzen füllt. |
| `D_8009EE94` | `gSecondCallGuard` | — | Ein zweiter Aufruf ist wirkungslos. |
| `D_8009EEC0` | `gOpOrderFlag` | — | Bei 0 werden die Operationen 2, 1 und 3 in dieser Reihenfolge abgearbeitet. |
| `D_8009EEF4` | `gSetupGuard` | — | Bei 0 läuft das Setup, danach wird höchstens 0xF01 Mal abgefragt. |
| `D_8009EF08` | `gTableCursor` | — | Zeiger in die SPU-Tabelle; der abschließende freie Lauf wird zurückgefaltet. |
| `D_8009EF0C` | `gSpuEntryTable` | — | Einträge, denen `func_80098708` Bit 0x80000000 setzt. |
| `D_8009F6B8` | `gBitFieldState` | — | Wird als Bitfeld gelesen und fortgeschrieben. |
| `D_8009F6BC` | `gOutStreamId` | — | Stream-Id, die nach außen gegeben und mit `func_80024064`/`func_80023F08` gestartet wird. |
| `D_8009F6C0` | `gLatchFlag` | — | Bit 0x40 setzt nur dieses Latch, alle anderen Flags lösen Aufrufe aus. |
| `D_8009F6DC` | `gCallGateFlag` | — | Sperrt ein Paar von Aufrufen mit festen Argumenten. |

### Meldungs-Handler und Callback-Slots

Diese Adressen tragen alle denselben Charakter: sie werden als `handler`-Feld in Anforderungs- oder Knotenstrukturen abgelegt und über den Dispatcher erreicht. Sie bleiben deshalb bei ihren Retail-Namen — der Beleg nennt nur die Registrierung, nicht die Aufgabe.

`D_8001C808`, `D_8002DEA8`, `D_8003190C`, `D_80036E6C`, `D_80037708`, `D_8003B2D0`, `D_80044610`, `D_800578B4`, `D_80059ABC`, `D_8005B934`, `D_8005CA04`, `D_8007F618`, `D_80080038`, `D_80086950`, `D_80086D74`, `D_800A36B8`, `D_800A36E8`, `D_800A3778`, `D_800A38F8`.

**Nicht umbenannt:** `D_8009BEF4`, `D_8009C810` — Byte-Vergleich kippt nach dem Rename. `D_8009F614` ist in der vierten Runde doch als `gCdTargetLoc` durchgegangen: der frühere Fehlschlag kam von einer Inline-Asm-Zeichenkette, die der Rename mitzog, nicht vom Symbol selbst.

## Textformatierung, Audio-Pegel, Missions- und Dateitabellen

Fünfzig weitere Globals in **einem** Commit. Vier Kandidaten aus der CD-/Verzeichnisgruppe kippten beim Byte-Vergleich (`D_800A445C`, `D_800A4480`, `D_800A4484`, `D_800A3E80`) und bleiben stehen; `D_8009F4D8` kam später über per-Unit-Bindungen als `gMovieAudioState` doch noch durch.

### Frontend-Texte und Formate

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F34C` | `gScoreFormatString` | `u8[]` | Format, mit dem der Punktestand `state->score[…]` gezeichnet wird. |
| `D_8009F354` | `gStageLineFormat` | `u8[]` | Zeilenformat für `Stage + 1` an Position (274, 216). |
| `D_8009F368` | `gFormatStringB` | `char[]` | Zweiter Formatstring für `func_8008CD30(buffer, …, wert)` in einen 0x20-Byte-Puffer. |
| `D_8009F484` | `gMenuValueFormat` | `u8[]` | Format für den geklemmten Menüwert `clamp(zustand + 1, 1, 0xA)`. |
| `D_8009F490` | `gNumberFormatString` | `u8[]` | Zahlenformat, rechts neben dem Eintrag gezeichnet. |
| `D_8009F4A4` | `gDefaultNameString` | `u8[]` | Ersatzname, wenn der Datensatz keinen eigenen Namen liefert. |
| `D_8009F3B8` | `gTextBlock` | `struct Text8` | 8-Byte-Textblock, mit `lwl/lwr`-Paaren unaligned kopiert. |
| `D_8009F470` | `gPlaceholderGlyph` | `u16` | Platzhalter-Zeichen, das `func_8007A9D0` in die acht Glyphen kodiert. |
| `D_8009B300` | `gFrontEndModuleName` | `u8[]` | Modulname des Frontends; `strcat(…, ".mis")` vervollständigt ihn. |
| `D_8009B32C` | `gDescriptorTable` | `Ent[]` | Deskriptor-Tabelle des Bildschirms, Index ist der Parameter. |
| `D_8009C1AC` | `gHexDigitTable` | `u8[]` | 16 Einträge Hex-Ziffern, adressiert mit `(byte + i) & 0xF`. |
| `D_8009C1BC` | `gOctalDigitTable` | `u8[]` | 8 Einträge, adressiert mit `(byte + i) & 7`; erzeugt NUL-terminierte Zeichen. |
| `D_8009C1C4` | `gDifficultyLabelMap` | `u16[]` | Schwierigkeitslabel: 2→0, 1→1, 0→2, 3→3. |
| `D_8009C1E0` | `gSymbolBitLengthTable` | `Entry[]` | Bitlänge je Symbol, 0x60 Einträge. |

### Frontend-Zustände und Layout

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F3C0` | `gFrontEndListFlag` | `u8` | Bei ungleich 0 werden Listenzeiger und Zähler des Frontends gebunden. |
| `D_8009F3C1` | `gPlayerWrapLimit` | `u8` | Obere Grenze der Auswahl (3 bzw. 2), ab der auf 0 gewickelt wird. |
| `D_8009F3C3` | `gPausedMenuFlag` | `u8` | Verriegeltes Flag des Pausenmenüs (Text-Id 0x526C). |
| `D_8009F3C4` | `gLockedMenuFlag` | `u8` | Verriegeltes Flag des gesperrten Menüs (Text-Id 0x5276). |
| `D_8009F4C8` | `gStripStepWidth` | `s32` | Schrittweite des Streifen-Cursors, mal 8 Pixel. |
| `D_8009F4CC` | `gBufferCount` | `s32` | Anzahl der Puffer, immer 2. |
| `D_8009F4D0` | `gFrameTimeMax` | `u32` | Laufendes Maximum der Frame-Zeit. |
| `D_8009FACC` | `gEffectId` | `u32` | Effekt-Id, die die Text-Engine für den aktuellen Bildschirm abspielen soll. |
| `D_8009FA98` | `gPresetIndexPerPort` | `u8` | Voreinstellungsindex je Port (Zweibyte-Array ab `D_8009FA98`). |
| `D_8009ACF4` | `gFormCodeTable` | `Entry[]` | Tabelle `[Form][Code - 6]` für die Codes 6 bis 0xF. |

### Audio-Pegel

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F330` | `gMusicVolume` | `s32` | Musiklautstärke, mit geklemmtem Wert geschrieben. |
| `D_8009F334` | `gMusicVolumeSetting` | `s32` | Einstellwert, den `func_80045288` bei `(arg0 & 0xFF) != 0` zurückgibt. |
| `D_8009F338` | `gVoiceVolumeBase` | `s32` | Basiswert, aus dem die skalierte Voice-Lautstärke berechnet wird. |
| `D_8009F739` | `gStreamAudioFlag` | `u8` | Flag des Stream-/CD-Audiopfads, beim Herunterfahren gelöscht. |
| `D_8009F74C` | `gVoiceList` | `struct SoundVoice *` | Knotenliste der Voices; ein Knoten kann beim Stummschalten erhalten bleiben. |
| `D_8009F768` | `gLoadedFont` | `u16` | Lädt derzeit eine Schrift; bei 0 kehrt der Aufruf sofort zurück. |
| `D_8009F770` | `gScriptTimerTicks` | `struct ScriptTimer *` | Countdown-Einträge des Skript-Timers, um die verstrichene Zeit dekrementiert. |
| `D_800A1800` | `gVolumeLevels` | `s32` | Lautstärkepegel-Tabelle (0x7F ist die Obergrenze der Bibliothek), `[0] = -1`. |

### Mission und Streaming

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F930` | `gMissionTable` | `s32 *` | 0x104-Byte-Tabelle, deren erstes Wort eine 32-Bit-Maske ist. |
| `D_8009F934` | `gMissionState` | `s32` | Nimmt den Zustand der Missions-Boot-Kette `func_80024A24` auf. |
| `D_8009F938` | `gCellBucketGrid` | `Bucket *` | Eimer-Array-Zeiger, über `func_80083A10` indiziert. |
| `D_8009F93C` | `gCellGridWidth` | `int` | Rasterbreite in Zellen. |
| `D_8009F940` | `gCellGridHeight` | `int` | Rasterhöhe in Zellen. |
| `D_8009F950` | `gWorkAreaBlock` | `void *` | Zielblock samt Arbeitsbereich beim Allokieren. |

### Datei- und Verzeichnistabellen

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_800A448C` | `gPathTable` | `u8[]` | Pfadtabelle mit 44-Byte-Schritt (Parent-Id bei `+0x800A4484`), von `func_80090218` gefüllt. |
| `D_800A5A80` | `gDirRecordBuffer` | `u8[]` | Datensatz-Puffer des Verzeichnis-Durchlaufs nach dem Sektorlesen. |
| `D_800A3E88` | `gCachedDirName` | `char[]` | Zwischengespeicherter Verzeichniseintrags-Name. |
| `D_8009C820` | `gCommandParamTable` | `s32[]` | Je Kommandobyte ein Flag, ob ein Parameterpuffer nötig ist. |
| `D_800A0798` | `gEventBuffer` | `u8[]` | Ereignispuffer der Abfrage `func_8008E1BC(1, 0, …)`; Bit 0x10 beendet die Schleife. |

### Pad-Treiber und Raster

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009C7A0` | `gPadMotorValue` | `s32` | Erhält das Doppelte des Ergebnis-Nibbles, oder 0x20 wenn dieses 0 ist. |
| `D_8009C8CC` | `gIndexEntryTable` | `int[]` | Einträge für Indizes unter 28. |
| `D_8009C94C` | `gLowByteEntryTable` | `int[]` | Einträge für Low-Byte-Werte unter sieben. |
| `D_8009C814` | `gCdUseDmaFlag` | `s32` | Bit 0 schaltet im CD-Lesepfad `func_800908D4` von `gCdRead` auf den DMA-Callback um; `gStartCdInit` löscht das Wort. **Bestätigt.** |
| `D_800A0C70` | `gSlotSpawnDescs` | `s32[]` | Belegungs-Spaltenbyte je Region und Slot. |

## Effekt-Renderer, Lichtraster, Frontend und Pad-Treiber

Fünfzig weitere Globals in **einem** Commit. Zwei Kandidaten fielen beim Byte-Vergleich durch (`D_8009F79C`, `D_8009F7C0`) und bleiben stehen; `D_8009EA00` erwies sich als reines Inline-Asm-Symbol ohne C-Deklaration und wurde durch `D_8009EEE0` ersetzt.

### Effekte, Objekte und Skript-VM

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_800A2950` | `gMapGridBounds` | `s32[4]` | Welt-Rechteck für x/z mit 0x800 Rand, gegen das Positionen geprüft werden. |
| `D_800A6578` | `gSpawnTypeFlags` | `u32[]` | 256-Spalten-Zellenraster, dessen Zellwort per Zeigerrechnung adressiert wird. |
| `D_800A7040` | `gSpawnRecordMap` | `s32[2][256]` | Spawn-Datensatz je Karte; beim Eintragen wird das Raster-Byte gelöscht. |
| `D_800A8DC0` | `gPlayerCollisionCandidates` | `Entry[]` | Kandidatenliste für Spieler-Kollisionen gegen den Gegenspieler. |
| `D_800A8EA8` | `gPendingEntryList` | `Request[]` | Ausstehende Einträge; beim Verwerfen wird der Completion-Callback (`+0x0C`) genullt. |
| `D_800A8F94` | `gEffectPoolCapacity` | `s32` | Kapazität des Effektpools; bei Erreichen schrumpft die Grenze von 0x38000 auf 0x20000. |
| `D_800A8F98` | `gEffectListHead` | `Node *[]` | Listenkopf der Effektkette, Zähler steht zwei Langwörter daneben. |
| `D_800A2290` | `gSideStepTable` | `u8[][8]` | Tabelle `[Seite][Schritt]` für den Verfolger-Kamerapfad. |
| `D_800A22F0` | `gDomeEffectBlock` | `Block *[4]` | Angehängter Effektblock (0x1B0 Bytes) mit Kuppelnetz und zwölf Bewegungs-Einträgen. |
| `D_800A2300` | `gEffectKeyTable` | `Entry[]` | Schlüssel-Tabelle; das Feld `unk02` landet in `obj->unk20`. |
| `D_800A3538` | `gTracerPool` | `Pool` | Objektpool, den `func_8006EDC4` verwaltet. |
| `D_800A353C` | `gTracerActiveList` | `PoolNode *` | Kopf der Objektliste, je Eintrag einmal aufgerufen. |
| `D_800A3320` | `gOperandStack` | `s32[]` | Globaler Operandenstapel der Skript-VM. |
| `D_800A3340` | `gVmRecordTable` | `Entry[]` | Datensätze der VM; Rückgabe ist der Zeiger auf den Treffer oder 0. |
| `D_8009F9A4` | `gVmCountdowns` | `s32[32]` | 32 Countdown-Einträge der Skript-VM, um `amount` dekrementiert und bei 0 geklemmt. |

### Lichtraster und Fragmente

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F780` | `gLightGrid` | `Node **` | Eimer-Raster der räumlichen Lichtknoten. |
| `D_8009F784` | `gLightNodePool` | `FixedPool *` | Pool-Handle für 0x180 Knoten zu je 0x44 Bytes. |
| `D_8009F78C` | `gLightGridWidth` | `s32` | Spaltenzahl des Lichtrasters. |
| `D_8009F790` | `gLightGridHeight` | `s32` | Zeilenzahl des Lichtrasters. |
| `D_8009F700` | `gLightStrengthScale` | `s32` | Faktor der Lichtstärke: `(Radius - Distanz) * Wert >> 16`. |
| `D_8009F710` | `gShadowThreshold` | `s16` | Schwelle, unterhalb derer Schattenhalbwörter gesetzt werden. |
| `D_8009F798` | `gFragmentFreeList` | `Node *` | Freiliste der Fragmentknoten. |
| `D_8009F7B0` | `gFreedPointer` | `s32` | Zeiger, der beim Aufräumen freigegeben und genullt wird. |
| `D_8009F7C4` | `gPadWord2` | `u32` | Zweites Steuerwort des Layout-Maskenpfads. |

### Frontend-Zustände

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F3E8` | `gPauseFlag` | `u8` | Pause-Flag, wird beim Start von Sound 0x49 gelöscht. |
| `D_8009F3EC` | `gPendingModuleNameFlag` | `s32` | Zeigt an, dass ein Modulname kopiert und um `".mis"` ergänzt werden soll. |
| `D_8009F3F0` | `gModuleLoadState` | `s8` | Zustandsnummer des Modul-Ladepfads (nach dem Kopieren auf 5 gesetzt). |
| `D_8009F464` | `gStillRunningState` | `s32` | Zustand des Boot-Profilladevorgangs: 2 = läuft (`func_8007F8BC`), 3 = fertig (`func_80080B08`); `func_8007F8CC` meldet „fertig". |
| `D_8009F496` | `gPendingFlag` | `u8` | Ausstehender Profil-Transfer, den `func_8007FACC` konsumiert und dabei den Kartenschritt abbaut. |
| `D_8009F4D4` | `gInitGuardFlag` | `u8` | Wächter gegen einen zweiten Durchlauf der einmaligen Arbeit. |
| `D_8009F504` | `gLayerOpenResult` | `u8` | 1 nach erfolgreichem Öffnen der Ebene, sonst 0. |
| `D_8009F518` | `gTextLookupTable` | `void *` | Aktive Text-Lookup-Tabelle, die `func_800898E8` freigibt. |
| `D_8009F8C8` | `gDeferredTransition` | `s32` | Verzögerte Zustandsumschaltung, wenn das Übergangsbyte nicht 1 ist. |

### Pad- und CD-Treiber

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009C740` | `gPadDataOffset` | `u32` | Offset auf den Datenzeiger samt 0xF0-Versatz bei aktivem Pfad. |
| `D_8009C75C` | `gPadBasePort` | `char *` | Basis-Port, der an den Handler gegeben wird. |
| `D_8009C76C` | `gPadDispatchIndex` | `s32` | Dispatch-Index, steuert die Nachbereitung des Aufrufs. |
| `D_8009C7AC` | `gPadHandlerTable` | `s32 (*[])(void)` | Tabelle der indiziert aufgerufenen Pad-Handler. |
| `D_8009C8C4` | `gLastCommandResult` | `u8` | Zuletzt gespeichertes Kommandoergebnis des Treibers. |
| `D_8009C8C5` | `gLastResultByte` | `u8` | Einzelbyte dazu, vorzeichenlos gelesen und weitergereicht. |
| `D_8009CAEC` | `gCommandParamMask` | `int[]` | Je Kommando ein Flag: nur dann ist ein Parameterpuffer erforderlich. |
| `D_8009CBD0` | `gCdAsyncReadState` | `struct State` | Zustandsblock des asynchronen CD-Lesens: Moduswort (+0x0C), Ziel-LBA, Busy-Flag (+0x24) und die gesicherten Callbacks (+0x28, +0x2C). |
| `D_8009CC88` | `gIrqDispatchState` | `struct { u16 active, busy; void (*handlers[11])(void); u16 mask; }` | Zustandsblock des Interrupt-Dispatchers; `gDispatchIrqs` arbeitet auf Aktiv-Flag, Handler-Tabelle und Maske. **Bestätigt.** |
| `D_800A9448` | `gTimeoutThreshold` | `volatile u32` | Zeitüberschreitungsschwelle (60, 0x50, 0x91, 400). |
| `D_8009EEE0` | `gInstalledHandler` | `s32` | Zwischengespeicherter, aktuell installierter Handler; bei Treffer kehrt der Rumpf sofort zurück. |
| `D_8009EA2C` | `gSpuDmaCallback` | `void *` | SPU-DMA-Kanal-4-Callback, beim Aufräumen gelöscht. |

**Nicht umbenannt:** `D_8009F79C` (Zähler der aktiven Knoten) und `D_8009F7C0` (Controller-/Profilwörter `[0..1]`) — Byte-Vergleich kippt nach dem Rename. `D_8009EA00` existiert nur als Inline-Asm-Symbol ohne C-Deklaration und kann deshalb keinen C-Namen tragen.

## Heap, CD-Treiber, Layer und Mischer

Fünfzig weitere Globals, wieder in **einem** Commit. Zwei davon mussten in `8001C808.c` von Hand nachgezogen werden, weil dort Mehrfach-Deklarationen stehen, die das Umbenennungsskript überspringt.

### Heap des Loaders

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F9E8` | `gHeapMainBlockSize` | `s32` | Größe des Hauptblocks, im Retail `0x15A000`. |
| `D_8009F594` | `gFreeRangeCount` | `s32` | Anzahl gültiger Wörter in der Frei-Bereichstabelle. |
| `D_8009F5AC` | `gFreeRangeCursor` | `s32` | Cursor in diese Tabelle, um `-0x12` gewickelt ab `0x14`. |
| `D_8009FA9C` | `gHeapPoolBase` | `s32` | Basis der Pools; die Freibereiche entstehen als `{base, base, base+size, base+size}`. |
| `D_8009F230` | `gAllocCallback` | `s32` | Installierter Allokations-Callback (Tag 2, Next-Fit). |
| `D_8009F22C` | `gFreeCallback` | `s32` | Standard-Callback zum Freigeben, von `func_8001D248` gesetzt. |
| `D_80010028` | `gHeapHighWater` | `s32` | Hochwassermarke, wächst um `4 + size` je Anforderung. |
| `D_8001002C` | `gHeapHighWaterPeak` | `s32` | Zweite Marke, wird nicht zurückgesetzt. |

### CD-/Kartentreiber

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F250` | `gTransferPending` | `volatile u32 *` | Transfer ausstehend; im Zustand 2 wird auf Bit `0x1000000` des Ziels gewartet. |
| `D_8009F254` | `gTransferStatusPtr` | `volatile u32 *` | Zeiger auf das Statuswort dieses Transfers. |
| `D_8009F244` | `gTransferError` | `int` | Wird bei einem Vergleichsfehler auf `-1` gesetzt. |
| `D_8009F24C` | `gTransferRetryCount` | `int` | Wartezähler, wird auf `0x8CA` nachgeladen. |
| `D_8009F228` | `gCdAbortFlag` | `int` | Abbruch-Flag des Treibers (Ergebnis `-0x1D`). |
| `D_8009F234` | `gMediaBypassFlag` | `u8` | `1` = Bereitschaftsprüfung wird übersprungen (Rückgabe 0 statt -2). |
| `D_800A6DD8` | `gFileOpenResult` | `char[]` | Ergebnispuffer von `func_8001DC48(mode, …)`. |
| `D_800A6E20` | `gFileTable` | `File[]` | Dateitabelle, die nach dem Moduswechsel aufgebaut wird. |
| `D_8009F620` | `gFileSlotState` | `int` | Slot-Zustand der Datei; negative Codes sind Fehler (-9, -0x11). |

### Layer, Ringe und Warteschlangen

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_800A06E0` | `gLayerBlocks` | `struct Display[]` | Layer-Blöcke; `[0]` liegt bei (0,0), `[1]` bei (0,0x100). |
| `D_800A06B0` | `gDrawBlocks` | `struct Draw[]` | Zeichenblöcke, 0x140 × 0xF0, über `func_800921F4` gesetzt. |
| `D_8009F224` | `gSelectedLayer` | `s8` | Gewählter Layer-Index; negativ bedeutet „kein Layer". |
| `D_8009F225` | `gLayerIndex` | `u8` | Zweites Layer-Feld, im Retail `2`. |
| `D_800A0660` | `gUploadRing` | `u8[]` | Upload-Ring; beim Flush werden die eingereihten Handles freigegeben. |
| `D_8009AA68` | `gSingleShotQueue` | `u32[]` | Warteschlange für einmalige Aufträge, wie die Handle-Warteschlange behandelt. |
| `D_800A0480` | `gStateTable` | `s32[]` | Zustandstabelle, von `func_800197A8` befüllt. |
| `D_800A04D0` | `gStateEntryTable` | `s32[]` | Zu jedem Eintrag der alte Wert plus der Zuwachs. |
| `D_8009FA68` | `gCompletionFlag` | `int` | Abschluss-Flag eines Slots im Anforderungspfad. |
| `D_8009F9C8` | `gInitDoneFlag` | `s32` | Nur bei ungleich 0 laufen die drei Aufräumaufrufe. |
| `D_8009F99C` | `gScreenModeHook` | `void (*)(void)` | Wird in den Bildschirmmodi 4 und folgende aufgerufen, wenn gesetzt. |

### Text- und Zeichenhilfen

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009AB08` | `gOperationState` | `struct Unk8001F180 *` | Aktiver Operationszustand (`+0x8F4`) und eingereihte Kommandos (`+0x8F8`). |
| `D_8009AB18` | `gPunctuationTable` | `u16[]` | Lookup für Satzzeichen-Bereiche beim Aufteilen druckbarer ASCII-Zeichen. |
| `D_8009AB0C` | `gCharRangeTable` | `struct Range[]` | Bereichstabelle (Basis/Start) für Ziffern, Groß- und Kleinschreibung. |

### Mischer und SPU

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F27C` | `gVolatileBase` | `volatile s32` | Flüchtige Basisadresse für die Kanalarbeit. |
| `D_8009F26C` | `gCvkSystemActive` | `u8` | Aktivierungs-Byte; nur bei ungleich 0 läuft der Slot-Durchlauf. |
| `D_8009F640` | `gCshdNodeSlots` | `s32[2]` | Zwei Zustandswörter, beim Aufsetzen genullt. |
| `D_8009F278` | `gSpuBufferPtr` | `s32` | SPU-Puffer, angefordert mit `func_80097B28(0xC000)`. |
| `D_8009F648` | `gStreamCommandState` | `s32` | Bei `1` beginnt/stoppt der Aufrufer den Stream, sonst andere Aktion. |
| `D_8009F280` | `gActiveStreamId` | `s32` | Aktive Stream-Id, an `func_8002A31C` mit Lautstärke 0 übergeben. |
| `D_800A0968` | `gMixerState` | `u8[]` | Initialisierter Mischer-Zustand samt Einträgen. |
| `D_800A0970` | `gMixerWord` | `s32[2]` | Zugriffsfunktion liefert `[0]`. |
| `D_800A07B4` | `gChannelTable` | `s32` | Kanaltabelle für Konvertierung und Voice-Auswahl. |

### Mission, Modul und Panel

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F684` | `gMissionId` | `s32` | Missions-/Level-Id, an den Aufrufer weitergegeben. |
| `D_800A0B30` | `gModuleNameBuffer` | `u8[]` | Modulname, über den BIOS-`strcpy`-Stub mit 0x80 Bytes kopiert. |
| `D_8009FA84` | `gDemoPlayback` | `s32` | Nur bei ungleich 0 läuft der Modul-Ladepfad. |
| `D_8009C1CC` | `gSpecialistModules[5]` | `s32[5]` | Die fünf Spezialisten-Module, sonst `"PA\"`. |
| `D_8009F678` | `gSlotVoiceHandle` | `s32[]` | Voice-Handle je Slot; wird gestoppt, wenn der Track wechselt. |
| `D_8009FA78` | `gSlotTrackId` | `s32[]` | Track-Id je Slot für denselben Vergleich. |
| `D_8009ABE8` | `gItemCodeTable` | `u8[]` | Sechs Item-Codes der Auswahl im Panel. |
| `D_800A0A80` | `gAssetHandle` | `s32[3]` | Handle, Resource und Flag eines Assets samt Zustand. |
| `D_800A0A88` | `gPlayerCountRecord` | `int` | Spielerzahl-Datensatz; nur der Wert 1 zählt als Einzelspieler. |
| `D_8009F688` | `gMissionParam` | `s32` | Parameter für `func_80059BFC(flag, …)`, Flag 2 bei `unk0C == 2`. |

`D_8009F61C` (Anfangs-Statuswort des CD-Lesers) und `D_8009F618` (Klammer-Flag um den Callback-Aufruf) tragen seit der vierten Runde die Namen `gCdCurrentLoc` und `gCdCallbackActive`; beide Units mit Zugriff sind byte-identisch geblieben.

## Grafik-, Sound- und Weltbereich: die zweite Runde

Fünfundfünfzig weitere Globals, wieder in **einem** Commit umbenannt. Bei allen war die Rolle in der Datei-Doku belegt; die Belegstelle steht jeweils daneben.

### GPU-Bibliothek und Debug-Ausgaben

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_8009DE90` | `gGpuCmdBlock` | `u32[]` | Drei-Wort-Kommandoblock, der an Ort und Stelle gebaut wird. | `80092FF8.c:47` |
| `D_8009DE9C` | `gGpuTpageWord` | `u32[]` | Texpage-Wort, gespeist aus `(Wert & 0xFFFFFF) \| 0x04000000`. | `80093178.c:70` |
| `D_8009DDE8` | `gVramWriteCallback` | `struct Callbacks *` | Callback, an den der VRAM-Schreibvorgang weitergereicht wird (er läuft dann im Bibliothekscode). | `8005A414.c:46` |
| `D_8009DDA8` | `gGpuCallbackTable` | `u8[]` | Callback-Tabelle der GPU-Bibliothek (Eintrag `+0x04`). | `80094A18.c:16` |
| `D_8009DDFC` | — | — | Abbruch-Flag des ausstehenden Aufrufs. **Nur Inline-Asm, kein C-Name setzbar.** | `80094CEC.c:21` |
| `D_8009DEFC` | `gTimeoutStamp` | `s32` | Ablaufmarke (aktueller Tick + 0xF0) für ausstehende Aufrufe. | `800951D8.c:11` |
| `D_8009DF00` | `gPendingDeadline` | `s32` | Gegenstück zur Marke; genullt bedeutet „nichts ausstehend". | `800951D8.c:12` |
| `D_800A63D0` | `gGpuLibSavedRa` | `s32` | Merker für den Rücksprungadress-Speicher der Bibliotheksroutine. | `80095DB0.c:20` |
| `D_8009E0A0` | `gAtanTable` | `s16[]` | Verhältnis-Tabelle, in die das Ergebnis einer Divisionsschutz-Prüfung indiziert. | `80095C30.c:57` |
| `D_8009F990` | — | — | Merker für die eingehende Rücksprungadresse im MDEC-Pfad. **Nur Inline-Asm, kein C-Name setzbar.** | `80096520.c:11` |

Debug-Texte, die der GPU-Wrapper `func_80092CF4` ausgibt:

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_80012A58` | `gOutOfBoundsLogTag` | `char[]` | Tag, das geloggt wird, wenn ein Rechteck außerhalb der Grenzen liegt. |
| `D_80012A64` | `gRectFormatString` | `char[]` | Format, mit dem das Rechteck ausgegeben wird. |
| `D_80012A78` | `gMode2LogTag` | `char[]` | Tag, das Modus 2 bedingungslos loggt. |
| `D_80012AD0` | `gClearOTagFormat` | `u8[]` | Format `"ClearOTagR(%08x,%d)...\n"` des Sony-Stil-Logs. |
| `D_80012BA8` | `gTimeoutLogMsg` | `u8[]` | Meldung bei Zeitüberschreitung (0x100000 Versuche). |
| `D_80012BB8` | `gMdecStatusLogMsg` | `u8[]` | Statusmeldung des MDEC-Pfads. |
| `D_80012BC8` | `gMdecTimeoutFormat` | `u8[]` | Format `"%s timeout:\n"` für den MDEC-Timeout. |
| `D_8001231C` | `gCdInitFailMsg` | `u8[]` | `"CdInit: Init failed\n"` nach fünf Fehlversuchen. |
| `D_8001233C` | `gCdFallbackMsg` | `u8[]` | Fallback-Text für Low-Byte-Werte ab sieben. |
| `D_80012488` | `gCdErrorLogMsg` | `char[]` | Meldung des CD-Fehlerpfads, über `func_8008FF08` ausgegeben. |

### Sound

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_8009F730` | `gSoundModeFlag` | `s32` | Globales Sound-Modus-Flag (Stereo/Aktiv). | `800451E0.c:7` |
| `D_8009F734` | `gSoundSpatialMode` | `s32` | Räumlichkeits-/Listener-Modus des Mischers. | `800451EC.c:7` |
| `D_8009F738` | `gVolumeLimitSelector` | `u8` | Wählt die obere Lautstärkegrenze 63 statt 127. | `800450A4.c:26` |
| `D_800A1810` | `gStreamTable` | `struct Unk453B8Stream[]` | Stream-Tabelle (spielt, Track, Handle). | `80047088.c:44` |
| `D_800A1830` | `gSoundBankTableLow` | `u8[]` | Primäre Sound-Id→Bank-Tabelle, Grenze `0x47`. | `80046A58.c:20` |
| `D_800A18D0` | `gSoundBankTableHigh` | `u8[]` | Zweite Tabelle für Ids ab `0x46`, Grenze `0x82`, Bias `-0x46`. | `80045494.c:24` |
| `D_800A1880` | `gSoundIdTable` | `u8[]` | Id-Tabelle der unteren Hälfte, an `func_80045494` übergeben. | `80045494.c:18` |
| `D_800A7938` | `gSoundEntryTable` | `struct Entry[]` | Bank-Slots mit Einträgen, Zählern und Datenbytes. | `80046A58.c:10` |
| `D_8009F73C` | `gVoiceFreeList` | `struct SoundVoice *` | Freiliste der Voice-Knoten. | `80046698.c:47` |
| `D_8009F744` | `gVoicePriorityList` | `struct SoundVoice *` | Prioritätsliste der aktiven Voices. | `80046698.c:46` |
| `D_8009F724` | `gChannelListHead` | `struct SoundChannel *` | Kopf der Kanal-Liste. | `80045548.c:29` |
| `D_8009F720` | `gChannelCount` | `s32` | Zähler der belegten Kanäle, wird bei Bedarf dekrementiert. | `80045548.c:33` |
| `D_8009F760` | `gOverlayTimer` | `s32` | Timer der Overlay-Einblendung. | `80048790.c:11` |

### Frontend

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_8009F3E0` | `gModulePrefixPA` | `int` | Pfad-Präfix `"PA\"` (Bytes 50 41 5C 00). | `800833A8.c:21` |
| `D_8009F3D8` | `gModulePrefixCW` | `int` | Pfad-Präfix `"CW\"` (Bytes 43 57 5C 00). | `800833A8.c:22` |
| `D_8009F3E4` | `gSetupDoneFlag` | `u8` | „schon eingerichtet"-Flag beim Einstieg in den Bildschirm. | `80077990.c:14` |
| `D_8009F3C2` | `gUnlockFlag` | `u8` | Freischalt-Flag, aus den Zuständen der ersten vier Einträge gesetzt. | `80077FFC.c:15` |
| `D_8009F87C` | `gPlayerNameLength` | `s16` | Länge des Spielernamens, im Retail `0x40`. | `800780C0.c:13` |
| `D_8009F87F` | `gLineCount` | `u8` | Anzahl numerierter Zeilen (`1 .. Wert + 1`) bei Zeilentyp 2. | `8007B854.c:63` |
| `D_8009F8F0` | `gPlayerSelection` | `volatile s32` | Auswahlindex je Spieler; hoch/runter läuft zyklisch über 0..3. | `8007A7A0.c:31` |
| `D_800A3948` | `gPasswordBuffer` | `u8[]` | Zehn-Zeichen-Passwort-/Cheat-Puffer, der geprüft und angewandt wird. | `8007EA78.c:44` |
| `D_8009C0A8` | `gLoadoutTable` | `struct Entry[3]` | Drei Ausrüstungs-Einträge (Wertezeiger, Ersatzwert, Grenze), für beide Spieler geklemmt. | `8007D2DC.c:17` |
| `D_8009C0E4` | `gMenuEntryTable` | `Entry[]` | 28 Menü-Einträge `(id, x, y, align)`, über `func_800788F0` gezeichnet. | `8007E67C.c:26` |
| `D_8009C18C` | `gMenuTailEntryTable` | `Entry[]` | Fünf weitere Einträge, die im Anschluss gezeichnet werden. | `8007E67C.c:27` |
| `D_8009BABE` | `gSoundWidgetLabel` | `u16[]` | Label des Sound-Schalters; der Callback kippt das globale Sound-Flag und aktualisiert `[0]`. | `8007D27C.c:60` |

### Welt- und Zellenraster

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_800A2970` | `gCellClipRect` | `Grid` | Rechteck `(x, y, Breite, Höhe)`, auf dem der Durchlauf arbeitet. | `80062614.c:19` |
| `D_8009F820` | `gCellMap` | `s32` | Zellenkarte, ein Byte je Zelle, 0 = leer. | `80062614.c:20` |
| `D_800A2990` | `gCellNodeArray` | `Node[]` | Knoten-Array, indiziert über das Byte der Zellenkarte. | `80062614.c:21` |
| `D_8009F818` | `gCellEntityList` | `s32[2]` | Liste der Entitäten, die Ansicht 0 bei gesetztem Bit anhängt, Grenze 256. | `80062874.c:46` |
| `D_8009F828` | `gCellEntityCount` | `s32` | Zähler zu `gCellEntityList`. | `80062874.c:47` |
| `D_8009F834` | `gGridOriginX` | `u8` | Ursprung der Rasterspalte: `(x >> 17) - Wert`, außerhalb 0..4 verworfen. | `80062874.c:43` |
| `D_8009F835` | `gGridOriginY` | `u8` | Ursprung der Rasterzeile: `(z >> 17) - Wert`. | `80062874.c:42` |
| `D_800AB3B8` | `gCoverageGrid` | `Cell[]` | Abdeckungsraster, 16-Byte-Zelle je `(Zeile * 5 + Spalte)`, erstes Wort ist die Bitmaske. | `80062874.c:44` |
| `D_8009F814` | `gViewCount` | `s32` | Anzahl der Ansichten, beginnt bei 1. | `80062874.c:57` |
| `D_8009F838` | `gQueueCount` | `s32` | Füllstand der Warteschlange; bei `0x18` ist sie voll. | `8006314C.c:17` |

## Grafik-, Sound- und Weltbereich: die erste Runde

Fünfunddreißig Globals aus dem Grafikbereich, alle einzeln byte-genau geprüft und in **einem** Commit umbenannt. Die ersten 28 sind direkt aus den Datei-Dokus belegt, die letzten sieben wurden mit Google Gemini 4 Hydrogen geklärt und die Belegstellen anschließend im Code nachgeprüft.

### GPU-Bibliothek

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009DD90` | `gGpuDrawOffset` | `s32` | Ursprungs-/Zeichenbereichs-Feld des Bibliothekszustands, als Rückgabewert gelesen. |
| `D_8009DDF2` | `gGpuDebugLevel` | `u8[]` | Debug-Stufe des Paketbaus; `[0] >= 2` schaltet das optionale Log ein. |
| `D_8009DE00` | `gDrawEnvCache` | `u8[]` | Zwischengespeicherter 0x5C-Byte-`DRAWENV`-Block, den `func_80093488` als Kopierquelle nutzt. |
| `D_8009DE5C` | `gDispEnvCache` | `struct Cache` | Zwischengespeicherter 0x14-Byte-`DISPENV`-Block. |
| `D_800A6280` | `gGpuPacketArray` | `u32[]` | Paketpuffer, den `func_800949A0` an die GPU übergibt. |
| `D_800A62A8` | `gGpuPacketArrayAlt` | `u32[]` | Zweiter Paketpuffer derselben Einheit. |
| `D_800A9450` | `gGpuDmaQueue` | `volatile struct QueueEntry[]` | GPU-/DMA2-Warteschlange; `func_80094A3C` reiht dort Aufträge ein. |
| `D_800A8F90` | `gCommandListHead` | `Manager80031DE8` | Kopf der Kommando-Liste im Display-List-Interpreter (innerer `do-while` über vier Slots, großer Opcode-Switch). |

### Mathe-Tabellen der 3D-Pipeline

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009A234` | `gSineTable` | `u16[]` | Sinus-Tabelle: `sin = [index] << 1`, `cos = [0x400 - index] << 1`, Quadrant über `angle >> 10`. |
| `D_8009F5B0` | `gReciprocalTablePtr` | `u16 *` | 0x800-Byte-Block mit gepackten Exponent/Mantisse-Paaren als Kehrwert-Lookup; wird getestet, bevor er gelesen wird, und absichtlich hängend gelassen. |

### Sprites, Texturen und Tiles

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F77C` | `gSpriteDescriptorTable` | `void *` | Veröffentlichter Zeiger auf die aktive Sprite-/Tile-Deskriptortabelle. |
| `D_8009F7D8` | `gTextureDescriptorTable` | `void **` | Zeiger auf das Textur-Deskriptor-Array, beim Missionsstart genullt. |
| `D_8009F76A` | `gTexBankVramY` | `u16` | VRAM-Y-Basis der Texturbank (Upload bei `y = Wert + 0x100`, `x = 0x380`). |
| `D_8009F778` | `gSpriteRegionTable` | `void *` | Regions-Tabelle je Sprite: `+ idx*8 + 4` liefert `(u0, v0, Breite, Höhe, Textur-Id)`. |
| `D_8009FAA0` | `gGreyTexpageSub` | `u32` | UV-Basiswort für die drei Bildschirm-Vertizes (`<< 16 | 0x101`). |
| `D_800A3548` | `gHeaderPrimitiveRecord` | `u8[]` | Kopf-Primitiv, das aus den Clip-Flag-Bytes des Modells gefüllt wird. |
| `D_8009B234` | `gModelTextureTable` | `Texture[]` | Textur-Einträge je Modell; das Feld `+0x72` (s8) wählt den Index, `< 0` bedeutet keine Textur. |
| `D_800A6F38` | `gTileDefTable` | `u8[]` | 100-Einträge-Tabelle der Tile-Definitionen; `func_80065CA8(tile, index, x, y)` zeichnet daraus. |
| `D_8009F80C` | `gMapGridHeader` | `Other *` | Tile-Karte; `->cells` wird auf 0..53 geklemmt und in `gTileDefTable` nachgeschlagen. |
| `D_8009F808` | `gTileBlockTable` | `int *` | Block-Tabelle: `block = [((slot >> 2) & 0x3FFF)]`. |
| `D_8009B1A4` | `gTileShapeTable` | `s32[]` | Formcode je Tile: `>> 3` wählt im Switch die Vertex-Menge `v[0..7]` (zwei Varianten). |
| `D_8009F83C` | `gTileUvOffset` | `u16` | Farbwort, das aus den Farb-Bytes des Tile-Records gesetzt wird. |
| `D_8009F360` | `gRampFillCounter` | `s32` | Füllzähler eines Farbverlaufs; bis `0x14` wird die Farbe durchgeschrieben. |
| `D_800A1970` | `gPaletteTable` | `u16[]` | Farbpalette; Slot 0 wird mit RGB `(0x1F, 0x1F, 0x1F)` belegt. |
| `D_8009AAE0` | `gGlyphTable` | `u16[]` | 0x14 Einträge, vom Text-Renderer beim Ausgeben der Glyphen gelesen. |

### Frontend-Grafik

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009BEEC` | `gLabelIdTable` | `u16[]` | Label-Ids: die Werte 2/1/0/3 werden den Labels 0/1/2/3 zugeordnet (Rendern über `func_800788F0`). |
| `D_8009BF9C` | `gChoiceSpriteTable` | `u16[][2]` | Sprite-Ids je Auswahl: `[selData[i]][choice]`. |
| `D_8009F87E` | `gSelectedRow` | `u8` | Hervorgehobene Zeile im Listenbildschirm; diese Zeile bekommt Farbe `0x11`. |

### Weitere Helfer derselben Gruppe

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F598` | `gHeapSearchCursor` | `s32` | Next-Fit-Suchcursor des Heap-Allokators in die Frei-Bereichstabelle `D_8009F590`. |
| `D_8009F608` | `gDisplayEnabled` | `u8` | Globales Enable-Flag der Display-/Fade-Layer, beim Pufferwechsel an `func_8001C088` übergeben. |
| `D_8009F314` | `gPendingSpawnList` | `struct Object *` | Warteliste neu erzeugter Objekte, die am Ende von `func_8003F570` nach `kind` einsortiert wird (keine Renderliste). |
| `D_8009F8C0` | `gPlayerMenuDepth` | `u8` | Auswahltiefe je Spieler im Profil-Bildschirm (0 inaktiv, 1 Profil, 2 Level-Unterliste). |
| `D_8009F8D8` | `gRumbleEnabled` | `u8` | Vibrations-Flags je Controller im Frontend, aus `D_800A6FF8` übernommen und zurückgeschrieben. |
| `D_800A2210` | `gPathNetTables` | `struct Unk4F0F0Entry *[]` | Zeiger auf geladene `Cnet`-Pfadnetz-Tabellen (Header `s16 unk00` / `s16 count`, 12-Byte-Knoten); Skripte schalten dort Sperrflags (`& 2`). |
| `D_8009F238` | `gFileSearchGroup` | `int` | Datei-Suchgruppen-Index für `D_8009F240->items[]`; bei negativem Wert wird er als negierter Open-Mode an `func_8001D424` gegeben. **Schreibstelle nicht bestimmbar** — im Retail-Image steht 0. |

**Nicht umbenannt:** `D_800A8EA0` ist kein eigenes Symbol, sondern der Slot `[8]` von `gTexpageBaseTable` (`0x800A8E80 + 8*4`); ein eigener Name würde das Array auseinanderreißen.

## Pad-Treiber und Spielsitzung

Die folgenden Rollen wurden mit Google Gemini 4 Hydrogen (read-only, Repo-Zugriff) erarbeitet und anschließend in den Units selbst nachgeprüft.

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_8009C730` | `gPadTxByteHook` | `u32` | Handler, der bei der SIO0-Kommunikation das nächste Sende-Byte liefert; im Init mit `func_8008C07C` belegt. **Bestätigt.** | `src/game/8008BDB0.c:65`, `8008C07C.c:37` |
| `D_8009C770` | `gPadActuatorCostSum` | `int` | Aufsummierte Aktuator-Kosten eines Polling-Durchlaufs; wird vor der Port-Schleife genullt und nur erhöht, solange die Summe unter 61 bleibt. **Bestätigt.** | `src/game/8008C138.c:78`, `8008A43C.c:62` |
| `D_8009C790` | `gPadVblankIrqFlag` | `s32` | Verriegeltes Flag eines Pad-VBlank-Durchlaufs: am Beginn von `func_8008A43C` auf 1 gesetzt, von `func_8008A5A8` gelesen und genullt. **Bestätigt.** | `src/game/8008A43C.c:39`, `8008A5A8.c:26` |
| `D_8009FA8C` | `gPlayerInvulnerable` | `u8[]` | Ein Unverwundbarkeits-Flag je Spieler; verhindert Schadensabzug und Terrain-Schaden, wird im Spawn-Pfad bei `mode0`/`mode1 == 3` gesetzt. **Bestätigt.** | `src/game/800579D4.c:230`, `80058210.c:89`, `80053188.c:171` |
| `D_800A8EA4` | `gEnvMapTexpageWord` | `s32` | Genau der Eintrag `[9]` von `gTexpageBaseTable` (`0x800A8E80 + 9*4`): gepacktes `(CLUT << 16) \| TPAGE`-Wort für den Env-Map-Texturslot, als `vec.extra` übergeben. **Bestätigt.** | `src/game/80063C5C.c:117`, `80016A64.c:111`, `80016E6C.c:55` |
| `D_8009FA94` | `gGameSession` | `struct Obj *` (je Unit abweichend) | Persistenter Sitzungs- und Konfigurationsblock (244 Byte, `func_8001E240(0xF4, 3)`): `mode` (1 = 1P, 2 = 2P), `mode0`/`mode1` (Schwierigkeit, God-Mode), Übergangscode bei `+0x10`, Loadout und Match-Zähler. **Bestätigt** in der Rolle, einzelne Felder offen. | `src/game/80076EB0.c:80`, `8008125C.c:283`, `800579D4.c:214` |

`D_8009C750` bleibt unbenannt und ist in der Doku als **nicht bestimmbar** vermerkt: der Callback wird nur aufgerufen (zweiter Eintrag der IRQ-Chain-2-Tabelle, VBlank-Bit), die schreibende Stelle wurde auch mit Modellhilfe nicht gefunden.

## Model-Loader: Chunk-Slots der Cobj-Ressource

`func_80012E48` läuft über die Rohtabelle einer Modellressource und legt je Chunk-Tag einen Zeiger in einen eigenen globalen Slot. Die Tags sind ASCII, big-endian gelesen:

| Tag | Slot | C-Name | Datentyp | Bedeutung |
|---|---|---|---|---|
| `4DGI` | `D_8009F52C` | `gModelGeometryChunk` | `u8 *` | Kopf-Chunk der Geometrie (Zeiger auf den Chunk-Beginn inkl. Header) |
| `3DQL` | `D_8009F530` | `gModelPolygonList` | `u8 *` | Polygonliste: 12-Byte-Records (Gruppe/Typ, Textur-Offset, 4 Vertex- und 4 Normal-Indizes) |
| `3DTL` | `D_8009F534` | `gModelTriangleList` | `u8 *` | Dreiecksliste |
| — | `D_8009F528` | `gModelTriangleListSize` | `u32` | Länge der Dreiecksliste abzüglich 12 Byte Chunk-Header |
| `3DHY` | `D_8009F538` | `gModelHierarchy` | `u8 *` | Hierarchie/Skelett des Modells |
| `3DMI` | `D_8009F53C` | `gModelMeshInfo` | `u8 *` | Mesh-Informationen |
| `3DBB` | `D_8009F540` | `gModelBoundingBox` | `u8 *` | Bounding-Box des Modells |
| `AnmD` | `D_8009F544` | `gModelAnimData` | `u8 *` | Animationsdaten; der Index wird zu einer Anzahl (16-Byte-Records) umgeschrieben |
| `4DVL` | `D_8009F554` | `gVertexBlockTable` | `s32` | Indizierte Tabelle der Vertex-Blöcke |
| `4DNL` | `D_8009F558` | `gNormalBlockTable` | `s32` | Indizierte Tabelle der Normalen-Blöcke |
| `3DRL` | `D_8009F55C` | `gRelocBlockTable` | `s32` | Indizierte Tabelle der Relokations-Blöcke |
| `3DRF` | `D_8009F560` | `gRefBlockTable` | `s32` | Indizierte Tabelle der Referenz-Blöcke |

Alle zwölf wurden einzeln umbenannt und byte-genau geprüft. **Bestätigt** ist die Zuordnung Tag→Slot (Switch in `func_80012E48`); die inhaltliche Deutung der Tags `3DTL`, `3DMI`, `3DRL` und `3DRF` stammt aus dem Tag-Namen und bleibt **wahrscheinlich**.

Nicht benannt bleiben `D_8009F548` (`3DHS`), `D_8009F54C` (`3DTA`), `D_8009F550` (`3DAL`) sowie die beiden Knotenköpfe `D_8009F564`/`D_8009F568`: dort ist die Rolle im Code noch nicht belegt.

## Disc-Darstellung im Mech-Renderer

`func_800597C0` zeichnet die zwei Discs (Walker/Hover) und schreibt dafür gemeinsame Farb-Bytes.

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F370` | `gDiscColourWordSecondary` | `s32` | Zweites Farbwort für `func_8001484C` (vier Dreiecke, Skalierung 0x70, Tiefenversatz 0x200). **Bestätigt.** | `func_800597C0` |
| `D_8009FAA4` | `gFrameTime` | `u32` | Byte, dessen `>> 2` als hohe Farbe der Disc dient, wenn das Info-Block-Feld `+0x324` positiv ist. **Bestätigt.** | `func_800597C0` |
| `D_8009AF3C` | `gDiscPartIndexTable` | `u8[]` | Byte-Tabelle der Modellteil-Indizes, von denen zwei für die Discs genutzt werden. **Bestätigt.** | `func_800597C0` |

`D_8009F36D` (Rampen-Byte, `u8`-Definition innerhalb der Unit) und `D_8009F36C` (nur in Kommentaren erwähnt, kein eigenes Symbol) bleiben unverändert.

## Anforderungsliste des Spawn-Pfads

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F668` | `gRequestListHead` | `s32` | Kopf der doppelt verketteten Anforderungsliste, an die `func_80024EC8` neue Knoten anhängt. **Bestätigt.** | `func_80024EC8` |
| `D_8009F66C` | `gRequestListTail` | `s32` | Ende derselben Liste; neue Knoten werden hinten eingefügt. **Bestätigt.** | `func_80024EC8` |
| `D_8009F9F4` | `gRequestOffsetBias` | `s32` | Slot, den der Spawn-Pfad zuletzt belegt hat und später zurückmeldet. **Wahrscheinlich.** | `func_80024EC8` |

## 3D-Pipeline und Renderpuffer

Diese Gruppe bildet den Kern der 3D-Ausgabe: Transformationspuffer, Clip-Flags, Lichtvektoren und die Pools, aus denen die Polygon- und Paketdaten kommen. Die Belege stammen aus den Renderern `func_80013A14`, `func_80013F08`, `func_8001B2C8` und `func_8001B4A8`.

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009FA1C` | `gLevelVertexBlock` | `void *` | 0x400-Byte-Block für Vertizes, beim Level-Setup mit Tag 2 alloziert. **Bestätigt.** | `func_80013A14` |
| `D_8009F578` | `gPartVertexBuffer` | `void *` / `Vertex *` | 0x800-Byte-Puffer mit den transformierten Vertizes eines Bauteils; der Polygon-Emitter liest hier. **Bestätigt.** | `func_80013A14`, `func_80015B68` |
| `D_8009F57C` | `gClipFlagBuffer` | `void *` / `u8 *` | Ein Clip-Flag-Byte je Vertex; der Wert `0xC0` bedeutet sichtbar. **Bestätigt.** | `func_80015B68` |
| `D_8009F580` | `gModelUvOffset` | `s16` | Gemeinsames 16-Bit-Wort für Tiefe und Tag; zwei Bytes werden hineingepackt. **Bestätigt.** | `func_80013AA0`, `func_80013A14` |
| `D_8009FAE0` | `gScriptVertexBuffer` | `char[]` | Zielpuffer der transformierten Vertizes im skriptgesteuerten Pfad. **Bestätigt.** | `func_80013F08` |
| `D_8009A228` | `gLightVectorIn` | `unsigned char[]` | 12-Byte-Konstantvektor, den die Lichtberechnung mit der Kamera-Matrix multipliziert. **Bestätigt.** | `func_80013C80` |
| `D_800A02E0` | `gLightVectorOut` | `unsigned char[]` | 12-Byte-Zielvektor derselben Lichtberechnung. **Bestätigt.** | `func_80013C80` |
| `D_800A1940` | `gTexpageTable` | `u16[]` | Texpage-Wort je Zielzeile; daraus wird das Textur-Paket gebaut (`& 0x9FF | 0xE1000000`). **Bestätigt.** | `func_80048BF8`, `func_80049100` |
| `D_800A63E0` | `gDisplayListHead` | `u8[]` | Kopf der Display-Liste, in die sich die Primitive einhängen. **Wahrscheinlich.** | `func_8001B4A8` |
| `D_800A6418` | `gPreparedVertexTable` | `s16[][4]` | Aufbereitete Vertizes je Vorbereitungsschritt (`>> 4` verschoben, vier Halbwörter pro Eintrag). **Wahrscheinlich.** | `func_80071FA4` |

### Pools und Tabellen des Renderers

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F5E0` | `gPacketPoolBase` | `s32` | Startadresse des ersten Paketblocks, Gegenstück zu `gPrimPoolNext`. **Bestätigt.** | `func_8001B2C8` |
| `D_8009F5E4` | `gNodePoolBase` | `s32` | Startadresse des ersten 28-Byte-Record-Blocks, Gegenstück zu `gPacketCursor`. **Bestätigt.** | `func_8001B2C8` |
| `D_8009F5D0` | `gRenderNodeTable` | `s32` | Erster von zwei Puffern mit je 1024 Einträgen à 28 Byte (Doppelpuffer-Verfahren). **Bestätigt.** | `func_8001B2C8` |
| `D_8009F5D4` | `gRenderNodeTableAlt` | `s32` | Zweiter Puffer desselben Paars. **Bestätigt.** | `func_8001B2C8` |
| `D_8009F5F8` | `gNodeCapacity` | `s32` | Anzahl der 28-Byte-Einträge je Puffer, im Retail `0x400`. **Bestätigt.** | `func_8001B2C8` |
| `D_8009F5F4` | `gFrameBlockCount` | `s32` | Anzahl der 0x34-Byte-Blöcke, die der Renderer pro Bild reserviert. **Bestätigt.** | `func_8001B2C8` |
| `D_8009F5FC` | `gNodeTableLength` | `s32` | Länge der Bucket-Tabelle; bestimmt die Bucket-Anzahl beim Einsortieren. **Bestätigt.** | `func_8001B4A8` |
| `D_8009F600` | `gNodeTableIndex` | `s32` | Laufender Index in diese Tabelle; wird bei der Vorbereitung genullt. **Bestätigt.** | `func_8001B4A8` |
| `D_8009F5C0` | `gSubScreenActive` | `s32` | Flag für den Sub-Screen-Modus; steuert den 0xA0 breiten Clip beim Zeichnen. **Bestätigt.** | `func_8001B4A8` |
| `D_8009F9D4` | `gScreenOffsetValue` | `volatile s32` | Bildschirm-Offset, im Retail 0 oder `0x100`; wird als Farbe übergeben. **Bestätigt.** | `func_8001B2C8`, `func_800259D8` |
| `D_800A02F0` | `gLightVectorSource` | `struct Vec3s32` | 12-Byte-Quellvektor der Lichtberechnung, Gegenstück zu `gLightVectorOut`. **Bestätigt.** | `func_80013C80` |
| `D_800A8E80` | `gTexpageBaseTable` | `u32[]` | Texpage-Basis je Texturgruppe (`(descriptor >> 8) & 7` adressiert den Eintrag). **Bestätigt.** | `func_800648E8` |
| `D_8009F570` | `gVertexSourcePtr` | `void *` | Veröffentlichter Zeiger auf die Vertex-Quelle, die der Polygon-Emitter liest. **Wahrscheinlich.** | `func_80013F08`, `func_80016A64` |
| `D_8009F574` | `gVertexSourceAltPtr` | `void *` | Zweite veröffentlichte Quelle (Vertex-Quelle + 4) für denselben Emitter. **Wahrscheinlich.** | `func_80013F08`, `func_80016A64` |

## Tabellen, Pools und Verzeichnisse

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_80076A0C` | `gExceptionVectorBackup` | `unsigned int[]` | Acht Wörter Sicherung des ursprünglichen Exception-Vektors `0x80`, die `func_800772E0` zurückschreibt. **Bestätigt.** | `func_80077340` |
| `D_8009AFB4` | `gRadiusTable` | `s16[]` | Halbwort-Tabelle der Radien für die Ringdarstellung; drei Linien-Primitives werden daraus gebaut. **Bestätigt.** | `func_8005A828` |
| `D_8009AFE8` | `gTileCornerIndices` | `u32[]` | Vier-Byte-Tabelle der Vertex-Indizes. **Bestätigt.** | `func_800648E8` |
| `D_8009B314` | `gSegmentFlagTable` | `u8[]` | Ein Flag-Byte je Segmentindex, adressiert über den Parameter der Segmentabfrage. **Bestätigt.** | `func_800770A8` |
| `D_8009C8C0` | `gCdTargetLocation` | `unsigned char[]` | Vier-Byte-Puffer für die Zielposition des CD-ROM-Laufwerks. **Bestätigt.** | `func_8008E040` |
| `D_8009DD64` | `gDmaCallbackTable` | `s32[8]` | Rückruftabelle je DMA-Kanal; wird beim Init über `func_80091F38` genullt. **Bestätigt.** | `func_80091CC0` |
| `D_8009EA60` | `gVoiceSampleNote` | `u16[]` | Gespeicherte Sample-Note je SPU-Stimme. **Bestätigt.** | `func_80099368` |
| `D_8009F7F8` | `gDefinitionEntryCount` | `s32` | Anzahl belegter Einträge der 0x30-Byte-Definitionstabelle; beim Missionsstart auf 0 gesetzt. **Bestätigt.** | `func_800609F4` |
| `D_8009F848` | `gDescriptorRegistryCount` | `s32` | Anzahl belegter Einträge im Sechser-Register der Objekt-/Textur-Deskriptoren; beim Missionsstart genullt. **Bestätigt.** | `func_8006A880` |

## Objekt- und Entity-Verwaltung

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F7A0` | `gEntityIdCounter` | `s32` | Fortlaufende Entity-Id, wird pro Allokation vorab erhöht und beim Init genullt. **Bestätigt.** | `func_8004D3FC` |
| `D_8009F9DC` | `gLiveNodeCount` | `s32` | Zähler der lebenden Listeneinträge; beim Init genullt und pro Freigabe erniedrigt. **Bestätigt.** | `func_8004AB4C` |
| `D_8009F7B8` | `gPlacementSlotCount` | `s32` | Anzahl belegter Slots im Platzierungs-Header; beim Missionsstart auf 0 gesetzt. **Bestätigt.** | `func_8005130C` |
| `D_8009F38C` | `gBeamNodeList` | `struct Unk734B0 *` | Kopf der Modell-/Szenengraph-Liste; wird beim Missionsstart geleert. **Bestätigt.** | `func_800734B0` |
| `D_8009F6F8` | `gMapZoneList` | `struct Unk31Node *` | Kopf der Liste der Karten-Trigger und Zonen; beim Missionsstart geleert. **Bestätigt.** | `func_80031984` |
| `D_8009F840` | `gScriptVmDisabled` | `u8` | Sperrflag für die Bytecode-Skriptmaschine; ungleich 0 hält die Ausführung an. **Bestätigt.** | `func_80069D58` |
| `D_8009FA50` | `gSceneFrameCounter` | `u32` | Frame-Zähler der Szene, inline über `lui`/`lw` geladen. **Wahrscheinlich.** | `func_800437E8` |

## Schrift und Frontend-Tabellen

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F35C` | `gFont` | `struct FontEntry *` | Zeiger auf den aktuell geladenen Font; 0 bedeutet „kein Font installiert". **Bestätigt.** | `func_80048E88`, `func_80049100` |
| `D_8009F364` | `gFontBankCount` | `s32` | Anzahl der Fontbänke; ein Index ab diesem Wert wird abgelehnt. **Bestätigt.** | `func_80048E28` |
| `D_8009B724` | `gMissionListTable` | `u8[]` | Acht-Einträge-Standardtabelle der Missionsliste im Frontend; wird beim Zurücksetzen wiederhergestellt. **Bestätigt.** | `func_800776B8` |
| `D_8009B72C` | `gFrontEndListTable` | `s8[]` | Zustandstabelle der Frontend-Liste; beim Zurücksetzen werden optional alle fünf Einträge freigeschaltet. **Bestätigt.** | `func_80077708` |

## Schuss- und Strahl-Pfad

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_800A3344` | `gVmRecordObjects` | `u8[]` | Objektbereich der Datensatz-Tabelle, also `gVmRecordTable` + 4: je Datensatz zwei 0x20-Byte-Objekte (bei `+0x04` und `+0x24`), die vier aufgelösten Referenzen sitzen bei `+0x44`. **Bestätigt.** | `gRegistryLoad` |

Drei Wörter dieses Pfads bleiben ohne C-Namen: `D_8009F378`, `D_8009F380` und `D_8009F384` gehen als viertes Argument an `func_8006F0E8`, wo der Wert als Divisor wirkt (`sra $s1,4`, dann `div`), obwohl die statischen Werte wie Farbwörter aussehen. Was sie bedeuten, sagt der Code nicht.

## Diagnose

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DDEC` | `gLogPrintf` | `void (*)(char *, ...)` | Funktionszeiger auf die zentrale Log-Routine; im Retail mit `func_8008E000` vorbelegt und im ganzen Code als Ausgabe verwendet. **Bestätigt.** | `func_80092910`, `func_800930B0` |

Alle hier genannten Variablen behalten ihren ursprünglichen Typ; die betroffenen Units sind nach der Umbenennung weiterhin byte-identisch.

## CD-ROM-, Datei- und Memory-Card-Pfad: die vierte Runde

Zwölf Globals dieses Pfads in **einem** Commit. Die Namen folgen dem, was die Units selbst schon beschreiben: das Verzeichnis-Lesen, das Suchen einer Datei, das Setzen des Zielorts und der Status des eingehängten Callbacks.

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_1F801800` | `gCdStatusReg` | `volatile u8` | Index-/Statusregister des CD-ROM-Controllers; das Schreiben von 0 quittiert den Interrupt. | `8001CE54.c:140` (Schreiben), `8001C680.c:3` (Deklaration) |
| `D_1F801803` | `gCdInterruptReg` | `volatile u8` | Interrupt-Freigabe und -Flag desselben Controllers, direkt hinter dem Statusregister. | `8001CE54.c:141` |
| `D_8009AABC` | `gDefaultSearchMgr` | `u8[]` | Voreingestellte Suchverwaltung; wird als Rückfall zugewiesen, wenn der Aufrufer keine eigene mitbringt. | `8001D248.c:149` |
| `D_8009F240` | `gFileSearchMgr` | `void *` / `struct Manager *` | Aktive Suchverwaltung der Dateiebene; die Units deklarieren sie unterschiedlich typisiert. | `8001D248.c:149` (Schreiben), `8001D5E4.c:13` (Lesen) |
| `D_8009F248` | `gCdReadCommand` | `u8` | Kommandobyte, das an `func_8008E1BC` geht; steuert, ob gelesen oder gesucht wird. | `8001CE54.c:186` |
| `D_8009F258` | `gAltFileSearchDir` | `s32[]` | Zweites Suchverzeichnis, an den Dateipfad als alternatives Ziel übergeben. | `8001E110.c:12` |
| `D_8009F5E8` | `gUploadRingWriteIdx` | `volatile s32` | Schreibzeiger in den Upload-Ring; jedes `gQueueVramUpload` schiebt ihn um eins weiter. | `8001BBEC.c:13` |
| `D_8009F614` | `gCdTargetLoc` | `u8[4]` (`CdlLOC`) | Zielort auf der Disc in BCD; wird vor jedem Lesen aus dem Dateislot geschrieben. | `8001D0A0.c:58` (Schreiben), `8001CE54.c:154` (Vergleich) |
| `D_8009F618` | `gCdCallbackActive` | `s32` | Klammer-Flag um den Callback-Aufruf: solange ungleich 0, läuft die Übergabe. | `8001D130.c:95` (Schreiben), `8001CE54.c:150` (Lesen) |
| `D_8009F61C` | `gCdCurrentLoc` | `u8[4]` (`CdlLOC`) | Zuletzt gelesener Ort; mit `gCdTargetLoc` verglichen, um zu entscheiden, ob neu gesucht werden muss. | `8001CE54.c:153` |
| `D_800A8E68` | `gCdDriveStateTable` | `s32[]` | Zustandstabelle des Laufwerks, beim Aufsetzen der Dateiebene gesetzt. | `8001D248.c:103` |

`D_8009F610` stand schon vorher als `gCdCommandParam` in der Datei; zwei Units deklarierten ihn noch mit Retail-Namen und sind jetzt angeglichen.

**Nicht umbenannt:** `D_8009F611` und `D_8009F615` sind einzelne Bytes **innerhalb** von `gCdCommandParam` und `gCdTargetLoc` (`D_8009F615 - 1` zeigt auf den Zielort). Ein eigener Name würde ein Byte aus dem Puffer herausreißen, deshalb bleiben sie stehen. `func_80010000`/`D_80010000` (Laderzustand an der Speichergrenze) bleibt ebenfalls: das Symbol wird in zwei Units unterschiedlich benannt und steht zusätzlich in Inline-Asm-Blöcken.

## Controller-Pfad, Hook-Tabelle und Heap-Snapshots: die fünfte Runde

Die Controller-/DualShock-Strecke war der letzte große Block ohne Namen. Ihre Hooks stehen als Tabelle
im Datenbereich: dieselbe Tabelle, die den Port-Zustand kennt, trägt die Zeiger auf Start-, Abbruch-
und Abschluss-Routinen. Neu benannt sind daneben ein Formatierer-Spezialfall und drei Sicherungen des
Heap-Freigebers.

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_80012250` | `gHexLowerTable` | `char[16]` | Zifferntabelle für `%x`: Kleinbuchstaben 0–f, indexiert vom Divisionsrest. **Wahrscheinlich.** | `8008CD3C.c:258` (Lesen) |
| `D_8009C360` | `gVlcSubTableEntries` | `s32[]` | Zweite VLC-Tabelle (9-Bit-Codes) der Film-Dekodierung, von `gBuildMovieVlcTables` gefüllt. **Wahrscheinlich.** | `80089A88.c:102` (Lesen) |
| `D_8009C728` | `gPadStepErrorHook` | `void (*)(void)` | Wird gerufen, wenn ein Übertragungsschritt des Controllers fehlschlägt. **Bestätigt.** | `8008BDB0.c:63` (Schreiben), `8008A43C.c:62` (Lesen) |
| `D_8009C72C` | `gPadResetPortHook` | `void (*)(void)` | Setzt einen Port-Block zurück; beim Aufsetzen des Treibers eingehängt. **Bestätigt.** | `8008BDB0.c:64` (Schreiben), `8008A290.c:44` (Lesen) |
| `D_8009C734` | `gPadConfigHook` | `void (*)(void)` | Konfiguriert die Freigabebits der Ports; zeigt auf `gGetPadPortCode`. **Bestätigt.** | `8008BDB0.c:66` (Schreiben), `8008BA34.c:72` (Aufruf) |
| `D_8009C738` | `gPadAddrToCodeHook` | `void (*)(void)` | Übersetzt eine Port-Adresse in den Protokoll-Code; zeigt auf `gLookupPadPort`. **Wahrscheinlich.** | `8008BDB0.c:67` (Schreiben) |
| `D_8009C73C` | `gPadLookupPortFn` | `struct PadPort *(*)(s32)` | Löst einen Slot in seinen Port-Block auf; von den Wrapper-Funktionen benutzt. **Bestätigt.** | `8008BDB0.c:68` (Schreiben), `80089E8C.c:36` (Lesen) |
| `D_8009C748` | `gPadDrainPortHook` | `void (*)(void)` | Läuft, solange noch Ports abzuarbeiten sind. **Bestätigt.** | `8008A6CC.c:72`, `8008BA34.c:105` (Lesen) |
| `D_8009C74C` | `gPadCompleteHook` | `void (*)(void)` | Abschluss-Callback, sobald der Zähler offener Ports null erreicht. **Bestätigt.** | `8008BDB0.c:75` (Schreiben), `8008A6CC.c:87` (Lesen) |
| `D_8009C750` | `gPadIrqCallback` | `void (*)(void)` | Optionaler Nutzer-Callback im VSync-Handler des Controllers. **Wahrscheinlich.** | `8008A3D4.c:35` (Lesen) |
| `D_8009C754` | `gPadPollPreHookB` | `void (*)(void)` | Zweiter optionaler Vorlauf-Callback vor dem Poll. **Wahrscheinlich.** | `8008B810.c:47` (Lesen) |
| `D_8009C758` | `gPadPollPreHookA` | `void (*)(void)` | Erster optionaler Vorlauf-Callback vor dem Poll. **Wahrscheinlich.** | `8008B810.c:46` (Lesen) |
| `D_8009C764` | `gPadPollPreHookFlag` | `s32` | Schaltet die beiden Vorlauf-Callbacks frei. **Wahrscheinlich.** | `8008B810.c:45` (Lesen) |
| `D_8009C774` | `gPadExtEnableFlag` | `s32` | Freigabe für die erweiterten Port-Zustandsabfragen. **Bestätigt.** | `8008BDB0.c:60` (Schreiben), `80089E40.c:29` (Lesen) |
| `D_8009C778` | `gPadStartSlot` | `s32` | Erster aktiver Slot: 0, wenn Port 0 freigegeben ist, sonst 1. **Bestätigt.** | `8008A290.c:43` (Schreiben), `8008A43C.c:58` (Lesen) |
| `D_8009C77C` | `gPadEndSlot` | `s32` | Letzter aktiver Slot: 1, wenn Port 1 freigegeben ist, sonst 0. **Bestätigt.** | `8008A290.c:55` (Schreiben), `8008A43C.c:58` (Lesen) |
| `D_8009C780` | `gPadPendingCounts` | `s32[]` | Zähler je Slot, wie viele Rückrufe noch abzuarbeiten sind. **Bestätigt.** | `8008A6CC.c:61` (Lesen), `8008BA34.c:109` (Schreiben) |
| `D_8009C7E0` | `gDefaultFormatSpec` | `Spec` | Vorlage einer Formatangabe, die `gFormatStringBody` zu Beginn jedes Umwandlungszeichens kopiert. **Wahrscheinlich.** | `8008CD3C.c:79` (Lesen) |
| `D_8009F980` | `gMovieRingSectors` | `s32` | Kapazität des Film-Rings in 2048-Byte-Sektoren. **Bestätigt.** | `800896D4.c:32` (Schreiben), `80089700.c:87` (Lesen) |
| `D_800A3B60` | `gPadIrqNode` | `s32[]` | Knoten der BIOS-Interruptkette (Priorität 2), an dem der Controller-Poll hängt. **Bestätigt.** | `8008A5BC.c:57`, `8008A688.c:40` (Lesen) |
| `D_800A3B70` | `gPadPort0IdleCount` | `s32` | Zählt Bilder ohne Aktivität auf Port 0. **Bestätigt.** | `8008A43C.c:47` (Schreiben), `8008A290.c:42` (Lesen) |
| `D_800A3B74` | `gPadPort1IdleCount` | `s32` | Dasselbe für Port 1. **Bestätigt.** | `8008A43C.c:54` (Schreiben), `8008A290.c:54` (Lesen) |
| `D_800A3B90` | `gPadWorkBufA` | `u8[]` | Erster Arbeitspuffer je Port, im Abstand 0x23 zugewiesen. **Wahrscheinlich.** | `8008BDB0.c:81` (Lesen) |
| `D_800A3BD8` | `gPadWorkBufB` | `u8[]` | Zweiter Arbeitspuffer derselben Aufteilung. **Wahrscheinlich.** | `8008BDB0.c:80` (Lesen) |
| `D_800A6568` | `gFreedBlockSnap` | `s32[]` | Sicherung des Kopfs des freigegebenen Blocks, bevor er mit dem Nachbarn verschmilzt. **Wahrscheinlich.** | `8008CB40.c:50` (Schreiben) |
| `D_800A8E78` | `gSavedFreeCursor` | `s32[]` | Zeigerstand der Freiliste vor dem Umsetzen des Cursors. **Wahrscheinlich.** | `8008CB40.c:63` (Schreiben) |
| `D_800A8F80` | `gPrevBlockSnap` | `s32[]` | Sicherung des Vorgänger-Kopfs während derselben Verschmelzung. **Wahrscheinlich.** | `8008CB40.c:57` (Schreiben) |

Drei Symbole dieses Blocks trugen ihre Namen schon vorher und sind nur mitgewandert: `D_8009C744`
(`gReadyCheckHook`), `D_800A3B64` (`gCallbackTable2`) und `D_8009C7AC` (`gPadHandlerTable`).

**Nicht umbenannt:** die Timer-Register `0x1F801120`, `0x1F801124` und `0x1F801128` werden als rohe
Zeigerliterale (`*(volatile u16 *)0x1F801120`) angesprochen, nicht über ein Symbol — dort gibt es
nichts umzubenennen. Die BIOS-Sprungstellen `func_8008C950` bis `func_8008D650` bleiben ebenfalls
Retail-benannt: handgeschriebene Trampoline, kein C-Code.

## CD-ROM-Treiber, Interrupt-Dispatcher und ISO-9660-Cache: die sechste Runde

Der CD-ROM-Block war die letzte große Baustelle mit eigenen Datenstrukturen. Er zerfällt in drei
Schichten: die Meldungstexte des Treibers (alle `char[]` im Bereich `0x80012498–0x8001284C`), die
Zustands- und Callback-Wörter des Treibers (`0x8009C8xx`, `0x8009CBxx`) und die Tabellen des
ISO-9660-Lesers im Datenbereich (`0x800A3Exx`, `0x800A44xx`, `0x800A5A80`). Die MMIO-Zeiger dieser
Runde (`0x1F801018`, `0x1F801020`, `0x1F8010B0–0x1F8010F0`, `0x1F801070`, `0x1F801074`,
`0x1F801110`, `0x1F801114`, `0x1F801800–0x1F801803`, `0x1F801C00`, `0x1F801814`) sind sämtlich
Zeigerwörter, die eine Initialisierungsroutine einmal füllt — das Register dahinter steht im Namen.

### Meldungstexte des CD-Treibers

| Global (RAM) | C-Name | Bedeutung | Beleg |
|---|---|---|---|
| `D_80012498` | `gCdTimeoutFmtMsg` | Formattext der Zeitüberschreitung (`CD_sync`/`CD_ready`/`CD_cw`). **Bestätigt.** | `8008ECFC.c:126`, `8008EF7C.c:148` (Lesen) |
| `D_800124B4` | `gCdDiskErrorMsg` | Meldung „Disk Error" aus dem Interrupt-Handler. **Wahrscheinlich.** | `8008E7A0.c:105` |
| `D_800124C0` | `gCdErrorFmtMsg` | Formattext der Fehlerdetails des Interrupt-Handlers. **Wahrscheinlich.** | `8008E7A0.c:117` |
| `D_800124DC` | `gCdUnknownIntrMsg` | Diagnose für einen unbekannten CD-Interrupt. **Wahrscheinlich.** | `8008E7A0.c:234` |
| `D_80012510` | `gCdSyncNameStr` | Funktionsname „CD_sync" für Zeitüberschreitungs-Logs. **Wahrscheinlich.** | `8008ECFC.c:81` |
| `D_80012518` | `gCdReadyNameStr` | Funktionsname „CD_ready" für dieselben Logs. **Wahrscheinlich.** | `8008EF7C.c:103` |
| `D_80012524` | `gCdCmdLogFmtMsg` | Formattext der Kommando-Aufzeichnung. **Wahrscheinlich.** | `8008F244.c:71` |
| `D_8001252C` | `gCdNoParamMsg` | Meldung bei fehlendem Parameter. **Wahrscheinlich.** | `8008F244.c:84` |
| `D_8001253C` | `gCdCmdNameStr` | Funktionsname „CD_cw" für Zeitüberschreitungs-Logs. **Wahrscheinlich.** | `8008F244.c:175` |
| `D_8001257C` | `gCdInitLogMsg` | Bannertext von `CD_init`. **Wahrscheinlich.** | `8008F8E8.c:55` |
| `D_80012588` | `gCdInitAddrFmt` | Formattext, der die Adresse des Zustandsblocks ausgibt. **Wahrscheinlich.** | `8008F8E8.c:58` |
| `D_80012594` | `gCdDataSyncLabel` | Bezeichner der Daten-Synchronisation. **Wahrscheinlich.** | `8008FAC8.c:73` |
| `D_800125AC` | `gCdDepthErrFmt` | Fehlertext bei Verzeichnistiefe ≥ 8. **Wahrscheinlich.** | `8008FF20.c:95` |
| `D_800125C8` | `gCdBadPathErrFmt` | Fehlertext bei leerem Pfadsegment. **Wahrscheinlich.** | `8008FF20.c:105` |
| `D_800125FC` | `gCdSearchLogFmt` | Debug-Text der Dateisuche. **Wahrscheinlich.** | `8008FF20.c:120` |
| `D_8001261C` | `gCdFoundLogFmt` | Debug-Text bei Treffer. **Wahrscheinlich.** | `8008FF20.c:136` |
| `D_80012628` | `gCdNotFoundErrFmt` | Fehlertext bei Nichttreffer. **Wahrscheinlich.** | `8008FF20.c:147` |
| `D_80012638` | `gCdPvdReadErrMsg` | Fehlertext, wenn der PVD-Sektor nicht lesbar ist. **Wahrscheinlich.** | `80090218.c:74` |
| `D_80012664` | `gCdIsoMagicStr` | Signatur `CD001` des ISO-9660-Volume-Descriptors. **Wahrscheinlich.** | `80090218.c:79` |
| `D_8001266C` | `gCdBadSigErrMsg` | Fehlertext bei falscher `CD001`-Signatur. **Wahrscheinlich.** | `80090218.c:81` |
| `D_8001269C` | `gCdPathReadErrFmt` | Fehlertext beim Lesen der Pfadtabelle. **Wahrscheinlich.** | `80090218.c:91` |
| `D_800126C0` | `gCdPathTblLogMsg` | Überschrift des Pfadtabellen-Dumps. **Wahrscheinlich.** | `80090218.c:99` |
| `D_800126E0` | `gCdPathEntryFmt` | Formattext eines Pfadtabellen-Eintrags. **Wahrscheinlich.** | `80090218.c:131` |
| `D_800126F4` | `gCdPathCountFmt` | Formattext der gezählten Pfadeinträge. **Wahrscheinlich.** | `80090218.c:145` |
| `D_80012718` | `gCdDirReadErrMsg` | Fehlertext, wenn der Verzeichnissektor nicht lesbar ist. **Wahrscheinlich.** | `80090580.c:79` |
| `D_80012738` | `gCdDirCacheLogMsg` | Überschrift des Verzeichnis-Cache-Dumps. **Wahrscheinlich.** | `80090580.c:84` |
| `D_80012754` | `gCdDotDirName` | Zwei-Byte-Name des `.`-Eintrags. **Wahrscheinlich.** | `80090580.c:99` |
| `D_80012758` | `gCdDotDotPrefix` | Erste zwei Bytes des `..`-Eintrags. **Wahrscheinlich.** | `80090580.c:103` |
| `D_8001275A` | `gCdDotDotNul` | Abschließendes Nullbyte desselben Eintrags. **Wahrscheinlich.** | `80090580.c:104` |
| `D_8001275C` | `gCdDirEntryFmt` | Formattext eines Cache-Eintrags. **Wahrscheinlich.** | `80090580.c:118` |
| `D_80012778` | `gCdDirCountFmt` | Formattext der Eintragszahl. **Wahrscheinlich.** | `80090580.c:132` |
| `D_8001279C` | `gCdSectorErrMsg` | Fehlertext bei abweichender Sektoradresse nach dem Lesen. **Wahrscheinlich.** | `800908D4.c:96` |
| `D_80012830` | `gIrqInactiveErrFmt` | Fehlertext, wenn ein Interrupt im inaktiven Zustand eintrifft. **Wahrscheinlich.** | `800916EC.c:54` |
| `D_8001284C` | `gIrqStuckErrFmt` | Fehlertext, wenn ein Interrupt über 0x800 Durchläufe hängen bleibt. **Wahrscheinlich.** | `800916EC.c:94` |

### Zustand und Callbacks des CD-Treibers

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_8009C810` | `gCdReadCallback` | `u32` | Callback nach abgeschlossenem CD-Lesen; `gStartCdInit` setzt ihn auf den DMA-Abschluss. **Bestätigt.** | `8008DEA0.c:57` (Schreiben), `80091240.c:36` (Lesen) |
| `D_8009C8A8` | `gCdSyncCallback` | `u32` | Callback nach abgeschlossenem Kommando. **Bestätigt.** | `8008DEA0.c:55` (Schreiben), `8008EF7C.c:182` (Aufruf) |
| `D_8009C8AC` | `gCdReadyCallback` | `u32` | Callback, sobald Daten bereitstehen. **Bestätigt.** | `8008DEA0.c:56` (Schreiben), `8008EF7C.c:171` (Aufruf) |
| `D_8009C8B8` | `gCdIrqExtraByte` | `u8` | Zweites Byte des IRQ-Antwortpakets. **Bestätigt.** | `8008E7A0.c:100,113` (Lesen/Schreiben), `8008F8E8.c:67` |
| `D_8009CB88` | `gCdStateBlock` | `char[]` | Adresse des Treiber-Zustandsblocks, wie sie `CD_init` protokolliert. **Wahrscheinlich.** | `8008F8E8.c:58` |
| `D_8009CBC0` | `gCdCachedDirIndex` | `s32` | Index des gerade zwischengespeicherten Verzeichnisses. **Bestätigt.** | `80090218.c:143` (Schreiben), `80090580.c:74` (Lesen) |
| `D_8009CBD8` | `gCdReadCursorPtr` | — | Zielzeiger des laufenden Sektors innerhalb `D_8009CBD0` (+0x08). **Wahrscheinlich.** | `800908D4.c:105` |
| `D_8009CBDC` | `gCdReadModeWord` | `s32` | Moduswort des Leselaufs (+0x0C). **Bestätigt.** | `80090C18.c:111` (Lesen), `80090FA0.c:92` (Schreiben) |
| `D_8009CBF0` | `gCdReadSectorPtr` | — | Zeiger auf die Ziel-LBA des Leselaufs (+0x20). **Wahrscheinlich.** | `800908D4.c:92` |
| `D_8009CBF4` | `gCdReadBusyPtr` | — | Busy-Flag des Leselaufs (+0x24). **Bestätigt.** | `80090FA0.c:72` (Lesen), `800908A0.c:38` (Schreiben) |
| `D_8009CBF8` | `gCdSavedCallbackPtr` | — | Gesicherter primärer Callback (+0x28). **Bestätigt.** | `800908A0.c:33` (Lesen), `80090FA0.c:118` (Schreiben) |
| `D_8009CBFC` | `gCdSavedReadCbsPtr` | — | Gesichertes Paar aus Ready- und DMA3-Callback (+0x2C…+0x30). **Bestätigt.** | `80090E00.c:104` (Lesen), `80090FA0.c:119` (Schreiben) |

### Interrupt-Dispatcher

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_8009CC8C` | `gIrqHandlerSlots` | `void (*[11])(void)` | Elf Handler-Zeiger im Dispatcher-Block `D_8009CC88` (+0x04). **Bestätigt.** | `800918BC.c:35` (Lesen), `:52` (Schreiben) |
| `D_8009CCB8` | `gIrqRepeatMask` | `u16` | Maske der Interrupts, die nach der Behandlung erneut geprüft werden (+0x30). **Bestätigt.** | `800916EC.c:79` (Lesen), `800918BC.c:54` (Schreiben) |
| `D_8009CCC4` | `gIrqQueueHeadPtr` | — | Zeigerwort auf den Anfang des Handler-Stapels (+0x3C). **Wahrscheinlich.** | `80091614.c:66` (Schreiben) |

### BIOS-IRQ-Knoten und ISO-9660-Tabellen

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_800A3E04` | `gIrqNodeHandler` | `s32[]` | Handler- und Prädikatswörter (+0x04/+0x08) des BIOS-IRQ-Knotens `D_800A3E00`. **Wahrscheinlich.** | `8008D964.c:47` (Schreiben) |
| `D_800A3E0C` | `gIrqNodeLinkEnd` | `s32` | Abschließendes Verkettungswort (+0x0C) desselben Knotens. **Wahrscheinlich.** | `8008D964.c:53` (Schreiben) |
| `D_800A3E58` | `gCdReadyBuffer8` | `u8[]` | Acht-Byte-Antwortpuffer des Ready-Interrupts. **Bestätigt.** | `8008E7A0.c:185,209,231` (Schreiben), `8008EF7C.c:228` (Lesen) |
| `D_800A3E60` | `gCdDataBuffer8` | `u8[]` | Acht-Byte-Antwortpuffer des INT4-Interrupts. **Bestätigt.** | `8008E7A0.c:191` (Schreiben), `8008EF7C.c:202` (Lesen) |
| `D_800A3E70` | `gCdWaitFuncName` | `char *` | Name der gerade wartenden Funktion für Zeitüberschreitungs-Logs. **Bestätigt.** | `8008ECFC.c:83,103` (Schreiben/Lesen), `8008EF7C.c:105` |
| `D_800A3E80` | `gCdDirEntryCache` | `DirEntry[64]` | Zwischenspeicher der 64 Verzeichniseinträge eines Sektors. **Bestätigt.** | `8008FF20.c:139` (Lesen), `80090580.c:95` (Schreiben) |
| `D_800A445C` | `gCdDirSectorTable` | `s32[]` | Sektor je Verzeichnis-ID, Schrittweite 0x2C. **Wahrscheinlich.** | `80090580.c:77` (Lesen) |
| `D_800A4480` | `gCdPathDirIdTable` | `u8[]` | Verzeichnis-ID der ISO-Pfadtabelle, Schrittweite 0x2C. **Bestätigt.** | `80090218.c:116` (Schreiben), `:129` (Lesen) |
| `D_800A4484` | `gCdPathParentTable` | `u8[]` | Eltern-ID derselben Tabelle. **Bestätigt.** | `80090218.c:117` (Schreiben), `800904DC.c:17` (Lesen) |
| `D_800A4488` | `gCdPathExtentTable` | `u32[]` | Startsektor derselben Tabelle. **Bestätigt.** | `80090218.c:113` (Schreiben), `:128` (Lesen) |
| `D_800A448C` | `gPathTable` | `char[]` | Namensfeld der ISO-Pfadtabelle, Schrittweite 0x2C. **Wahrscheinlich.** | `80090218.c:120` (Schreiben) |

Drei Namen aus früheren Runden waren falsch geraten und sind in dieser Runde berichtigt:
`D_8009CC88` hieß `gFrameClockAnchor` (jetzt `gIrqDispatchState`), `D_8009CC8A` hieß
`gFrameClockValid` (jetzt `gIrqDispatchBusy`), `D_8009DD30` hieß `gValueMatchTable` (jetzt
`gVSyncCallbackTable`); dazu `D_8009C814` von `gCardRetryCount` auf `gCdUseDmaFlag` und
`D_8009CBD0` von `gSectorBufferPtr` auf `gCdAsyncReadState` (der Block ist eine Struktur, kein
Zeiger). `D_8009DD10` (`gIrqCallbackTable`) behielt seinen Namen.

**Nicht umbenannt:** `D_800125E0`, `D_800124F0`, `D_800A3E50` (`gStatusBuffer8` blieb), die
Registeradressen in `0x1F80xxxx` (nur als Zeigerwörter geführt) sowie die zwölf Units dieser Runde,
deren Rolle nicht belegbar ist — sie stehen in `docs/FUNCTION_MAP.md` ohne Namen.

## Runde A – Modellressourcen, Speicherverwaltung und Engine-Dienste

Die folgenden RAM-Symbole wurden anhand der Zugriffe in den 100 Units dieser Runde benannt.
Die Spalte „Beleg“ nennt die Funktionen, in denen die Verwendung nachvollziehbar ist.

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beleg |
|---|---|---|---|---|
| `D_80010AE8` | `gCardSlotZeroLabel` | `const struct Packed9` | Neun Bytes für „UNKNOWNA“ mit Nullabschluss. Der Parser kopiert sie in beide Namensfelder von Kartenplatz 0. **Wahrscheinlich.** | `gParseCardDirectory` (Lesen) |
| `D_80010AF4` | `gCardFileSecondaryLabel` | `const struct Packed9` | Neun Bytes für „UNKNOWNB“ mit Nullabschluss. Der Parser nutzt sie als zweiten Namen bei Verzeichnistyp 0x51. **Wahrscheinlich.** | `gParseCardDirectory` (Lesen) |
| `D_80010B00` | `gCardOtherSlotLabel` | `const struct Packed9` | Neun Bytes für „UNKNOWNC“ mit Nullabschluss für Kartenplätze außerhalb der gesondert behandelten Typen. **Wahrscheinlich.** | `gParseCardDirectory` (Lesen) |
| `D_80010B0C` | `gCardDevicePathFormat` | `u8[] / char[]` | Formatzeichenfolge „bu0%1ld:%s“ für den Kartenpfad aus Portnummer und Dateiname. **Wahrscheinlich.** | `gQueueCardFileRead`, `gQueueCardFileWrite` (Lesen) |
| `D_8009F218` | `gVramUploadEndIndex` | `s32` | Endgrenze, bis zu der die 20 Plätze große Upload-Warteschlange abgearbeitet wird. **Wahrscheinlich.** | `gSubmitRenderFrame` (Lesen) |
| `D_8009F21C` | `gVramUploadReclaimIndex` | `s32` | Umlaufender Index der Freigabeschleife für die 20 Plätze große VRAM-Upload-Warteschlange. **Bestätigt.** | `gSubmitRenderFrame` (Lesen und Schreiben) |
| `D_8009F220` | `gVramUploadSubmitIndex` | `s32` | Umlaufender Index der Übergabeschleife für VRAM-Uploads und Endgrenze der Freigabeschleife. **Bestätigt.** | `gSubmitRenderFrame` (Lesen und Schreiben) |
| `D_8009F226` | `gAppliedDisplayLayer` | `u8` | Kopie des gewählten Anzeigeindex beim Anwenden der zugehörigen Display-Umgebung. **Wahrscheinlich.** | `gApplySelectedDisplayEnv` (Schreiben) |
| `D_8009F260` | `gCardEmptySlotLabel` | `const struct Packed6` | Sechs Bytes für „Empty“ mit Nullabschluss, die für freie Kartenplätze in beide Namensfelder kopiert werden. **Wahrscheinlich.** | `gParseCardDirectory` (Lesen) |
| `D_8009F548` | `gModel3dhsPayload` | `u8 * / u32 *` | Zeiger auf Offset +8 des Modellchunks mit Tag 0x33444853 (3DHS). Er wird in das Modellfeld +0x38 übernommen. **Bestätigt.** | `gIndexModelChunks` (Schreiben), `gRelocateModelResource` (Lesen) |
| `D_8009F54C` | `gModelAnimTrackChunk` | `u8 * / AnimNode *` | Optionaler 3DTA-Chunk. Bei der Relokation wird er als Kopf in die Liste `gAnimNodeList` eingehängt. **Bestätigt.** | `gIndexModelChunks` (Schreiben), `gRelocateModelResource` (Lesen) |
| `D_8009F550` | `gModelAnimWaveChunk` | `u8 * / u32 *` | Optionaler 3DAL-Chunk. Bei der Relokation wird der Kopf von `gAnimWaveList` darauf gesetzt. **Bestätigt.** | `gIndexModelChunks` (Schreiben), `gRelocateModelResource` (Lesen) |
| `D_8009F5A0` | `gHeapRangeCountThreshold` | `s32` | Schwellenwert für die Zahl gültiger Wörter in der Heap-Grenztabelle. Die Initialisierung setzt ihn auf 0x182. **Bestätigt.** | `gInitHeap` (Schreiben), `gIsHeapRangeCountAtThreshold` (Lesen) |
| `D_8009F5A8` | `gPrimaryPrngCursor` | `s32` | Index des ersten additiven Zufallszahlengenerators mit 20 Zustandswörtern. Die Startfunktion setzt ihn auf 0x14, der Generator aktualisiert ihn nach jedem Schritt. **Bestätigt.** | `gSeedPrng` (Initialisierung), `gAdvancePrimaryPrng` (Lesen und Schreiben) |
| `D_8009F5C8` | `gPacketBufferBases` | `s32 / u32[] / void *` | Basiswort des ersten 0x34-Byte-Paketpuffers und Beginn des Paars, das beim Pufferwechsel indiziert wird. **Bestätigt.** | `gAllocateRenderBuffers` (Schreiben), `gSubmitRenderFrame` (Lesen) |
| `D_8009F5CC` | `gSecondPacketBufferBase` | `s32 / void *` | Zweites Basiswort des Paketpufferpaars `gPacketBufferBases` bei +4. **Bestätigt.** | `gAllocateRenderBuffers` (Schreiben), `gFreeRenderBuffers` (Lesen) |
| `D_8009F5D8` | `gOrderingTableBuffers` | `s32 / u32[] / void *` | Basiswort des ersten Ordering-Table-Puffers und Beginn des beim Framewechsel indizierten Paars. **Bestätigt.** | `gAllocateRenderBuffers` (Schreiben), `gSubmitRenderFrame` (Lesen) |
| `D_8009F5DC` | `gSecondOrderingTableBuffer` | `s32 / void *` | Zweites Basiswort des Ordering-Table-Paars `gOrderingTableBuffers` bei +4. **Bestätigt.** | `gAllocateRenderBuffers` (Schreiben), `gFreeRenderBuffers` (Lesen) |
| `D_8009F5EC` | `gMdecUploadReclaimIndex` | `s32` | Index der Freigabeschleife für bereits verarbeitete Einträge des vier Plätze großen Upload-Rings. **Bestätigt.** | `gAllocateRenderBuffers` (Initialisierung), `gSubmitRenderFrame` (Lesen und Schreiben) |
| `D_8009F5F0` | `gMdecUploadSubmitIndex` | `s32` | Index der Übergabeschleife im vier Plätze großen Upload-Ring. Die Freigabeschleife endet an dieser Grenze. **Bestätigt.** | `gAllocateRenderBuffers` (Initialisierung), `gSubmitRenderFrame` (Lesen und Schreiben) |
| `D_8009F628` | `gSpuUploadCompleted` | `s32 / volatile int` | Abschlussflag für den synchronen SPU-Upload einer Soundbank. Der Aufrufer setzt es auf 0 und wartet auf den Wert 1 aus dem Callback. **Bestätigt.** | `gUploadSoundBankToSpu` (Lesen und Rücksetzen), `gSignalSpuUploadComplete` (Schreiben) |
| `D_8009F998` | `gSpuTransferWriteIndex` | `int` | Schreibindex des SPU-Transferrings mit neun Slots. Er läuft nach Eintrag 8 auf 0 zurück. **Bestätigt.** | `gEnqueueSpuTransfer` (Schreiben), `gCompleteSpuTransfer` (Lesen) |
| `D_8009F9AC` | `gSavedSpuTransferCallback` | `s32` | Gesicherter vorheriger SPU-Transfercallback, der beim Abbau der Warteschlange wieder installiert wird. **Bestätigt.** | `800211FC.c` (Schreiben), `gShutdownSpuTransferQueue` (Lesen) |
| `D_8009FA14` | `gSavedSpuTransferMode` | `s32` | Gesicherter vorheriger SPU-Transfermodus für die Wiederherstellung beim Abbau der Warteschlange. **Bestätigt.** | `800211FC.c` (Schreiben), `gShutdownSpuTransferQueue` (Lesen) |
| `D_8009FA34` | `gSpuTransferActive` | `s32 / int` | Aktivkennzeichen der SPU-Transferwarteschlange. Es wird beim Start gesetzt und nach dem letzten Abschluss gelöscht. **Bestätigt.** | `gStartSpuTransfer`, `gCompleteSpuTransfer` (Schreiben), `8002129C.c` (Lesen) |
| `D_8009FAD0` | `gSpuTransferDispatching` | `s32 / int` | Sperrwort gegen einen erneuten Eintritt in den SPU-Transferabschluss während eines Callbacks. **Bestätigt.** | `gCompleteSpuTransfer` (Lesen und Schreiben) |
| `D_800A0520` | `gVramUploadQueue` | `u8[]` | Beginn der 20 Upload-Einträge mit einer Schrittweite von 16 Bytes. Wort +0 liefert die Bilddaten, Wort +4 einen Eintrag für die Freigabe. **Wahrscheinlich.** | `gSubmitRenderFrame` (Lesen) |
| `D_800A0528` | `gVramUploadRectFields` | `u8[]` | Rechteckfeld bei +8 jedes 16-Byte-Eintrags in `gVramUploadQueue`. Die Adresse geht als erstes Argument an `gLoadImage`. **Wahrscheinlich.** | `gSubmitRenderFrame` (Lesen) |
| `D_800A07AC` | `gSoundBankSlots` | `s32` | Adresswort der Tabelle der Soundbank-Zeiger, Feld +4 von `gSoundSystem`. **Bestätigt.** | `gInitSoundStorage` (Schreiben), `gReleaseSoundBankSpuHandle` (Lesen) |
| `D_800A07B0` | `gSoundChannelCount` | `s32` | Anzahl der Soundkanal-Einträge, Feld +8 von `gSoundSystem`. **Bestätigt.** | `gInitSoundStorage` (Schreiben), `80021F3C.c` (Lesen über die Struktur) |

Nicht belegte RAM-Symbole behalten ihre Retail-Namen. Die Kommentare der betreffenden Units nennen die offenen Zuordnungen.

## Wie diese Datei weitergeführt wird

Am besten **eine neue Variable nach der anderen** ergänzen und die bestehende Gliederung beibehalten. Ein neuer Eintrag braucht zunächst nur:

1. Den ursprünglichen Namen `D_XXXXXXXX`, die RAM-Adresse und – falls belegt – den Initialwert.
2. Einen verständlichen C-Namen und einen kurzen Satz, wofür die Variable benutzt wird.
3. Ein oder zwei Funktionen, die diese Bedeutung belegen.
4. Den Vermerk **„bestätigt"**, **„wahrscheinlich"** oder **„noch offen"**, falls Zuordnung oder Typ unsicher sind.

Belege kommen zuerst aus der `@par Globals`-Zeile der Funktionsdoku, dann aus Schreib- und Lesestelle im Code. Keine Namen erfinden: bleibt die Rolle unklar, behält die Variable ihren `D_XXXXXXXX`-Namen.
