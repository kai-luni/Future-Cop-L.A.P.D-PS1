# Globale RAM-Variablen in Future Cop: L.A.P.D.

**Stand: 17. November 2026 · US-Version (`SLUS_007.39`)**

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
| `D_8009F9FC` | `gFrameId` | `s32` | Frame-Kennung für Zeitvergleiche; wird auf 5 gesetzt und bei Bedarf aus der Zeitbasis berechnet. **Wahrscheinlich.** | `func_800259D8`, `func_80025B7C` |
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
| `D_8009F4BC` | `gDecodeJob` | `s32` | Kennung des laufenden Dekodierauftrags (Rechteck-Paar, Index, Ziel, Flag). **Wahrscheinlich** (Doku: „decode job: rects[2], index, rect, dest, flag"). | `func_80085074` |
| `D_8009F4C0` | `gStripScaleEnable` | `s32` | Schalter für horizontales Skalieren eines Streifens; nur bei `== 1` und gesetztem `D_8009F4C4` aktiv. **Bestätigt.** | `func_80085074` |

## Spielzustand und Warteschlangen

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009FA00` | `gScreenState` | `s32` | Aktueller Bildschirmzustand der Hauptschleife (z. B. 1 = „loading", 13 = HUD-Phase). **Bestätigt.** | `func_80024E3C`, `func_8001EFF8` |
| `D_8009F9A8` | `gPendingScreenState` | `s32` | Nächster Bildschirmzustand; spiegelt `unk10` der Hauptschleife, solange er ungleich 0 ist, läuft die Schleife weiter. **Bestätigt.** | `func_80076A2C` |
| `D_8009FA60` | `gPaletteCycleCounter` | `s32` | Zähler für den Palettenwechsel; das HUD liest `(>> 4) & 7` daraus. **Bestätigt.** | `func_8001EFF8` |
| `D_8009FA20` | `gFrameCallbackEnabled` | `u8` | Schalter für den Pro-Frame-Rücksprung; 0 bedeutet „läuft noch nicht". **Bestätigt** (Doku: „per-frame-callback enabled flag"). | `func_8001E4E4`, `func_8001E540` |
| `D_800A0BC0` | `gRingBuffer` | `Queue298EC` | Ringpuffer mit Länge, Basisindex und acht 8-Byte-Slots für Kommandos. **Bestätigt.** | `func_800298EC` |
| `D_800A0B70` | `gStreamBufferList` | `State` | Kopf der Liste von Stream-/Asset-Puffern (`head` bei +0x10); neue Knoten werden hinten angehängt. **Bestätigt.** | `func_800288BC` |
| `D_800A0A00` | `gObjectSlots` | `Unk800A0A00[8]` | Acht Objekt-Slots; `unk00`/`unk01` = -1 markiert einen freien Slot. **Bestätigt.** | `func_8002333C` |

## Spielobjekte, Grafik und Zufall

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F9E4` | `gCamera` | `struct Camera *` | Die aktive Kamera; die Funktionen setzen Position, Blickpunkt, Rotation und die daraus gebaute Matrix. **Bestätigt.** | `func_800177AC`, `func_80017860` |
| `D_8009AA38` | `gBaseMatrix` | `struct Matrix3x3` | Grundmatrix, aus der Instanzen ihre Transformationsmatrix kopieren. **Wahrscheinlich.** | `func_80013BD8` |
| `D_800A0300` | `gBoneMatrices` | `struct Matrix[]` | Matrix je Knochen-/Segmentindex; Eintrag `i-1` ist der Elternknoten von `i`. **Wahrscheinlich.** | `func_80013CB8` |
| `D_8009F588` | `gTransformRecordList` | `void *` | Kopf der Liste der Transform-Sätze; wird beim Init auf 0 gesetzt und beim Durchlaufen verkettet. **Bestätigt.** | `func_8001774C`, `func_80017994` |
| `D_8009F58C` | `gTransformSerialCounter` | `s32` | Fortlaufende ID für Transform-Sätze (`node->id = gTransformSerialCounter++`). **Bestätigt.** | `func_80017994` |
| `D_8009F214` | `gRandSeed` | `s32` | Zustand des Zufallszahlengenerators (Multiplikation mit 16807, Park-Miller). **Bestätigt.** | `func_80019720` |
| `D_8009F5B8` | `gResourceList` | `struct Resource *` | Kopf der einfach verketteten Ressourcenliste; wird in mehreren Funktionen durchlaufen und am Ende freigegeben. **Bestätigt.** | `func_80019AA8`, `func_800198DC` |
| `D_800A07A8` | `gSoundSystem` | `struct SoundSystem` | Globaler Zustand des Klangsystems mit Bank-Liste und Anzahl. **Bestätigt.** | `func_80021508` |
| `D_8009F92C` | `gSaveProfileImage` | `u8 *` | 0x678 Byte großes Profil-/Spielstandabbild; Titel und Profilfelder werden an festen Offsets (+4, +0x20C) hineingeschrieben. **Bestätigt.** | `func_80081204`, `func_800822CC` |

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

<!-- OFFEN: D_8009FA58 (Zwei-Slot-Array der Spieler-Entities) bleibt vorerst
     unbenannt. In src/game/80027510.c wird dasselbe Symbol doppelt deklariert
     (einmal als Union `D_8009FA58`, einmal als Array `D_8009FA58_arr`); eine
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
| `D_8009FAA4` | `gDiscShadeByte` | `u32` | Byte, dessen `>> 2` als hohe Farbe der Disc dient, wenn das Info-Block-Feld `+0x324` positiv ist. **Bestätigt.** | `func_800597C0` |
| `D_8009AF3C` | `gDiscPartIndexTable` | `u8[]` | Byte-Tabelle der Modellteil-Indizes, von denen zwei für die Discs genutzt werden. **Bestätigt.** | `func_800597C0` |

`D_8009F36D` (Rampen-Byte, `u8`-Definition innerhalb der Unit) und `D_8009F36C` (nur in Kommentaren erwähnt, kein eigenes Symbol) bleiben unverändert.

## Anforderungsliste des Spawn-Pfads

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F668` | `gRequestListHead` | `s32` | Kopf der doppelt verketteten Anforderungsliste, an die `func_80024EC8` neue Knoten anhängt. **Bestätigt.** | `func_80024EC8` |
| `D_8009F66C` | `gRequestListTail` | `s32` | Ende derselben Liste; neue Knoten werden hinten eingefügt. **Bestätigt.** | `func_80024EC8` |
| `D_8009F9F4` | `gLastSpawnSlot` | `s32` | Slot, den der Spawn-Pfad zuletzt belegt hat und später zurückmeldet. **Wahrscheinlich.** | `func_80024EC8` |

## 3D-Pipeline und Renderpuffer

Diese Gruppe bildet den Kern der 3D-Ausgabe: Transformationspuffer, Clip-Flags, Lichtvektoren und die Pools, aus denen die Polygon- und Paketdaten kommen. Die Belege stammen aus den Renderern `func_80013A14`, `func_80013F08`, `func_8001B2C8` und `func_8001B4A8`.

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009FA1C` | `gLevelVertexBlock` | `void *` | 0x400-Byte-Block für Vertizes, beim Level-Setup mit Tag 2 alloziert. **Bestätigt.** | `func_80013A14` |
| `D_8009F578` | `gPartVertexBuffer` | `void *` / `Vertex *` | 0x800-Byte-Puffer mit den transformierten Vertizes eines Bauteils; der Polygon-Emitter liest hier. **Bestätigt.** | `func_80013A14`, `func_80015B68` |
| `D_8009F57C` | `gClipFlagBuffer` | `void *` / `u8 *` | Ein Clip-Flag-Byte je Vertex; der Wert `0xC0` bedeutet sichtbar. **Bestätigt.** | `func_80015B68` |
| `D_8009F580` | `gDepthTagWord` | `s16` | Gemeinsames 16-Bit-Wort für Tiefe und Tag; zwei Bytes werden hineingepackt. **Bestätigt.** | `func_80013AA0`, `func_80013A14` |
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
| `D_8009AFE8` | `gVertexIndexTable` | `u32[]` | Vier-Byte-Tabelle der Vertex-Indizes. **Bestätigt.** | `func_800648E8` |
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
| `D_8009F38C` | `gModelNodeList` | `struct Unk734B0 *` | Kopf der Modell-/Szenengraph-Liste; wird beim Missionsstart geleert. **Bestätigt.** | `func_800734B0` |
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

## Diagnose

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DDEC` | `gLogPrintf` | `void (*)(char *, ...)` | Funktionszeiger auf die zentrale Log-Routine; im Retail mit `func_8008E000` vorbelegt und im ganzen Code als Ausgabe verwendet. **Bestätigt.** | `func_80092910`, `func_800930B0` |

Alle hier genannten Variablen behalten ihren ursprünglichen Typ; die betroffenen Units sind nach der Umbenennung weiterhin byte-identisch.

## Wie diese Datei weitergeführt wird

Am besten **eine neue Variable nach der anderen** ergänzen und die bestehende Gliederung beibehalten. Ein neuer Eintrag braucht zunächst nur:

1. Den ursprünglichen Namen `D_XXXXXXXX`, die RAM-Adresse und – falls belegt – den Initialwert.
2. Einen verständlichen C-Namen und einen kurzen Satz, wofür die Variable benutzt wird.
3. Ein oder zwei Funktionen, die diese Bedeutung belegen.
4. Den Vermerk **„bestätigt"**, **„wahrscheinlich"** oder **„noch offen"**, falls Zuordnung oder Typ unsicher sind.

Belege kommen zuerst aus der `@par Globals`-Zeile der Funktionsdoku, dann aus Schreib- und Lesestelle im Code. Keine Namen erfinden: bleibt die Rolle unklar, behält die Variable ihren `D_XXXXXXXX`-Namen.
