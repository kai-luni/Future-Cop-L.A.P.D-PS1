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

## Steuerung, Skript-VM, Panels und CD-Treiber: die große Runde

Hundert weitere Globals in **einem** Commit. Drei Kandidaten kippten beim Byte-Vergleich (`D_8009BEF4`, `D_8009C810`, `D_8009F614`) und bleiben stehen. Außerdem sind vier Namen aus der vorigen Runde nachgetragen, die in der Doku fehlten: `gDeferredList` (`D_8009F708`), `gDeferredPool` (`D_8009F70C`), `gPendingMessageMask` (`D_8009F7A8`) und `gRegistryTable` (`D_8009F800`).

### Objekt- und Renderhelfer

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F348` | `gPlayerAvatarSprite` | — | Sprite je Spieler, gezeichnet bei y 74, sobald `player+0xAE` positiv ist. |
| `D_8009F374` | `gPendingMotion` | — | Startet eine Bewegung, wenn gesetzt; sonst erhält das Objekt einen Callback. |
| `D_8009F388` | `gColourWord` | — | Farbwort: `(Wert >> 8) \| 0x3A000000`, Farbe ist `field40 >> 8`. |
| `D_8009F45C` | `gAtlasTable` | — | Atlas mit `count`; der Index muss darin liegen. |
| `D_8009F460` | `gSpriteAtlasBlock` | — | Wird vor dem Neuaufbau freigegeben; bei Typ ungleich 2 wird ein Flag gesetzt. |
| `D_8009F468` | `gFontBankTable` | — | Schriftbank-Tabelle; ändert sich `[8]`, wechselt `func_80048E28` die Bank. |
| `D_8009F4E0` | `gLoopAbortFlag` | — | Bricht die Schleife früh ab; bleibt 0, greift der Fallback. |
| `D_8009F508` | `gReleaseDoneFlag` | — | Wird im Completion-Callback gesetzt. |
| `D_8009F5BC` | `gListTailFlag` | — | Bei ungleich 0 wird hinter dem Ende angehängt, sonst Kopf und Ende gesetzt. |
| `D_8009FABC` | `gCharTable` | `u8[]` | 0x100-Byte-Zeichentabelle; der Zeiger rückt um 16 weiter. |
| `D_8009F284` | `gGlobalTick` | — | Globaler Tick, gegen den die Frame-Helfer messen. |
| `D_8009FA30` | `gFrameTickBase` | — | Wartet mindestens 5 Ticks seit diesem Wert und aktualisiert das Frame-Delta. |
| `D_8009FA64` | `gCameraNearBase` | — | Erhält `(Wert << 8) / 608` für die Nahgrenze. |

### Skript-VM, Listen und Tabellen

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F2CC` | `gLabelRecord` | — | Datensatz mit drei Labelwerten (`unk04/unk08/unk0C`). |
| `D_8009F2F4` | `gBannerString` | — | Banner-String für `func_80049100(…, 0x41, 0x14, 1)`. |
| `D_8009F30C` | `gNodeCList` | — | Liste der NodeC-Einträge; passende werden entlinkt und freigegeben. |
| `D_8009F310` | `gSavedChunkBase` | — | Zwischengespeicherte Basis, wird auf 0 zurückgesetzt und über die Chunks iteriert. |
| `D_8009F318` | `gMessage10Counter` | — | Zähler der Meldung 10. |
| `D_8009F31C` | `gCellAbortFlag` | — | Wird gesetzt, solange „keine Zelle" gilt. |
| `D_8009F320` | `gNodeListHead` | — | Kopf der Knotenliste, Durchlauf mit vorab gesichertem `next`. |
| `D_8009F328` | `gConeTestFlag` | — | Nur bei ungleich 0 läuft der Kegeltest. |
| `D_8009F329` | `gBandTestFlag` | — | Bit 0 aktiviert den Bandtest. |
| `D_8009FA48` | `gChainedNodes` | — | Vier Knoten der Schrittweite 0x6014, angehängt beim Aufbau. |
| `D_8009FA40` | `gUnitListHead` | — | Kopf der Einheitenliste, über `next` bis null durchlaufen. |
| `D_8009F9D8` | `gByteTable` | `u8[]` | Byte-Tabelle, in die das Ergebnis indiziert; drei Bytes werden gepackt. |
| `D_8009F9F8` | `gOpcodeTable` | — | 32 Einträge; ein negativer Wert führt `func_80019668()` modulo. |
| `D_8009FA08` | `gTableBuffer` | — | Basisadresse des allokierten Tabellenpuffers. |
| `D_8009F9D0` | `gPool1Size` | — | Erster Pool: Größe `a0`, Anzahl `a1`. |
| `D_8009FAAC` | `gPool2Size` | — | Zweiter Pool: Größe `a2`, Anzahl `a3`. |
| `D_8009FA4C` | `gPool3Size` | — | Dritter Pool: Größe `a4`, Anzahl `a5`. |
| `D_8009F800` | `gRegistryTable` | — | 0x80-Slot-Registertabelle (Nachtrag). |
| `D_8009F708` | `gDeferredList` | — | Einfach verkettete Liste `{next, object}` (Nachtrag). |
| `D_8009F70C` | `gDeferredPool` | — | Pool aus 256 Acht-Byte-Knoten (Nachtrag). |
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
| `D_8009F680` | `gMenuModeFlag` | — | Wird auf 1 gesetzt, wenn Menümodus 0 oder `unk312` gesetzt ist. |
| `D_8009F698` | `gResourceSubChunkPtr` | — | Veröffentlichter `0x74544F53`-Subchunk-Zeiger einer `0x43746F73`-Ressource. |
| `D_8009F69C` | `gRegisterGuard` | — | Einzige Wächterprüfung auf 0, danach geradliniger Rumpf. |
| `D_8009FA6C` | `gPanelSelection` | — | Auswahl im Panel; Code 0 verlässt es mit Sound 0x3E und `-1`. |
| `D_8009FA80` | `gSlotIndexLimit` | — | Vergleichswert, bevor der Index weiterläuft. |
| `D_8009FA88` | `gListTailBranch` | — | Einzelne Verzweigung: Anhängen am Ende oder Kopf-Initialisierung. |
| `D_8009FAB8` | `gHudMode2` | — | Bei Wert 2 wird das HUD aktiviert. |
| `D_8009FAC0` | `gPanelSoundIds` | — | Sound-Ids 0x2F/0x30 des Panels, danach mischt `func_80025F58` neu. |
| `D_8009FAC8` | `gFrameSwitch` | — | Frame-Umschalter; bei 0 kehrt der Aufruf sofort zurück. |
| `D_8009FAD8` | `gHookInstallFlag` | — | Wird genullt, danach installiert `func_8001E87C` die Geräte-Hooks. |
| `D_8009F870` | `gIdleDeadline` | — | Leerlauf-Frist, um 0x2328 Ticks vorgestellt. |
| `D_8009F874` | `gOverlayDelay` | — | Wird auf `-1` gesetzt, nachdem die Fläche gesetzt wurde. |
| `D_8009F904` | `gSnapshotResult` | — | Momentaufnahme, danach zwei `func_8001EB2C`-Aufrufe und Vergleich mit 1. |
| `D_8009F918` | `gTestResultFlag` | — | Bei erfolgreichem Test auf 1 gesetzt. |
| `D_8009F91C` | `gSnapshotPointer` | — | Wird freigegeben, wenn er ungleich 0 ist. |
| `D_8009F920` | `gPreviewTrackId` | — | Track-Id, für die die Vorschau-Voice gestartet wurde. |
| `D_8009F92A` | `gStatusTextId` | — | Status-Text-Id (0, solange nichts läuft). |
| `D_8009F944` | `gMissionClearFlag` | — | Wird genullt und sonst nirgends geschrieben. |
| `D_8009F968` | `gLayerModeBits` | — | Bit 0 wählt die schmale Variante (`0x27`). |
| `D_8009F9A0` | `gMissionWorkingSet` | — | Wird genullt, bevor `func_80017C68(0x168, …)` die Pools anlegt. |
| `D_8009F9B0` | `gStreamPreemptFlag` | — | Markiert beim Verdrängen abgebaute Stream-Voices. |
| `D_8009F9CC` | `gDisplaySetupToggle` | — | Wechselt zwischen 0 und 1 nach dem Display-Aufbau. |
| `D_800A0AA0` | `gActionRecords` | — | Fünf Acht-Byte-Aktionsdatensätze (Tastenmaske, Phase). |
| `D_800A0B00` | `gPanelPhaseTable` | — | Phasenfeld je Slot, an `func_800260B8` übergeben. |
| `D_800A0B10` | `gPanelChoiceTable` | — | Zweite Auswahltabelle, liefert Auswahl 4. |
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
| `D_800A2630` | `gRampVariantTable` | — | Verlaufsvariante des k==6-Zweigs. |
| `D_800A2931` | `gSlfxFlagByte` | `u8` | SLFX-Flag, beim Zerlegen der Karten-Chunks geschrieben. |
| `D_800A2940` | `gCellDataOutput` | — | Nimmt Ausgabezeiger und Metadaten der Zelldaten auf. |
| `D_800A3190` | `gScenePositionPairs` | — | Positionspaare, die `func_80066C9C` für die Szene schreibt. |
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

**Nicht umbenannt:** `D_8009BEF4`, `D_8009C810`, `D_8009F614` — Byte-Vergleich kippt nach dem Rename.

## Textformatierung, Audio-Pegel, Missions- und Dateitabellen

Fünfzig weitere Globals in **einem** Commit. Fünf Kandidaten aus der CD-/Verzeichnisgruppe kippten beim Byte-Vergleich (`D_8009F4D8`, `D_800A445C`, `D_800A4480`, `D_800A4484`, `D_800A3E80`) und bleiben stehen.

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
| `D_8009F938` | `gBucketArray` | `Bucket *` | Eimer-Array-Zeiger, über `func_80083A10` indiziert. |
| `D_8009F93C` | `gMissionGridWidth` | `int` | Rasterbreite in Zellen. |
| `D_8009F940` | `gMissionGridHeight` | `int` | Rasterhöhe in Zellen. |
| `D_8009F950` | `gWorkAreaBlock` | `void *` | Zielblock samt Arbeitsbereich beim Allokieren. |
| `D_8009F970` | `gQueueBlock` | `Queue *` | Warteschlangenblock: Einträge ab `+0x08`, Index und Zähler nach 257 Wörtern. |
| `D_8009F974` | `gStreamDescriptors` | `Ent88F00 *` | Ziel für die zu kopierenden Stream-Deskriptoren. |

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
| `D_8009C814` | `gCardRetryCount` | `s32` | Fehlversuchszähler der Karteninitialisierung; nach dem fünften greift der Inline-Fehlerpfad. |
| `D_800A0C70` | `gOccupancyColumns` | `s32[]` | Belegungs-Spaltenbyte je Region und Slot. |

## Effekt-Renderer, Lichtraster, Frontend und Pad-Treiber

Fünfzig weitere Globals in **einem** Commit. Zwei Kandidaten fielen beim Byte-Vergleich durch (`D_8009F79C`, `D_8009F7C0`) und bleiben stehen; `D_8009EA00` erwies sich als reines Inline-Asm-Symbol ohne C-Deklaration und wurde durch `D_8009EEE0` ersetzt.

### Effekte, Objekte und Skript-VM

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_800A2950` | `gWorldClipRect` | `s32[4]` | Welt-Rechteck für x/z mit 0x800 Rand, gegen das Positionen geprüft werden. |
| `D_800A6578` | `gMapGrid` | `u32[]` | 256-Spalten-Zellenraster, dessen Zellwort per Zeigerrechnung adressiert wird. |
| `D_800A7040` | `gSpawnRecordMap` | `s32[2][256]` | Spawn-Datensatz je Karte; beim Eintragen wird das Raster-Byte gelöscht. |
| `D_800A8DC0` | `gPlayerCollisionCandidates` | `Entry[]` | Kandidatenliste für Spieler-Kollisionen gegen den Gegenspieler. |
| `D_800A8EA8` | `gPendingEntryList` | `Request[]` | Ausstehende Einträge; beim Verwerfen wird der Completion-Callback (`+0x0C`) genullt. |
| `D_800A8F94` | `gEffectPoolCapacity` | `s32` | Kapazität des Effektpools; bei Erreichen schrumpft die Grenze von 0x38000 auf 0x20000. |
| `D_800A8F98` | `gEffectListHead` | `Node *[]` | Listenkopf der Effektkette, Zähler steht zwei Langwörter daneben. |
| `D_800A2290` | `gSideStepTable` | `u8[][8]` | Tabelle `[Seite][Schritt]` für den Verfolger-Kamerapfad. |
| `D_800A22F0` | `gDomeEffectBlock` | `Block *[4]` | Angehängter Effektblock (0x1B0 Bytes) mit Kuppelnetz und zwölf Bewegungs-Einträgen. |
| `D_800A2300` | `gEffectKeyTable` | `Entry[]` | Schlüssel-Tabelle; das Feld `unk02` landet in `obj->unk20`. |
| `D_800A3538` | `gObjectPool` | `Pool` | Objektpool, den `func_8006EDC4` verwaltet. |
| `D_800A353C` | `gObjectListHead` | `PoolNode *` | Kopf der Objektliste, je Eintrag einmal aufgerufen. |
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
| `D_8009F7C4` | `gControlMaskWord` | `u32` | Zweites Steuerwort des Layout-Maskenpfads. |

### Frontend-Zustände

| Global (RAM) | C-Name | Datentyp | Bedeutung |
|---|---|---|---|
| `D_8009F3E8` | `gPauseFlag` | `u8` | Pause-Flag, wird beim Start von Sound 0x49 gelöscht. |
| `D_8009F3EC` | `gPendingModuleNameFlag` | `s32` | Zeigt an, dass ein Modulname kopiert und um `".mis"` ergänzt werden soll. |
| `D_8009F3F0` | `gModuleLoadState` | `s8` | Zustandsnummer des Modul-Ladepfads (nach dem Kopieren auf 5 gesetzt). |
| `D_8009F464` | `gStillRunningState` | `s32` | Wird auf 2 geschrieben, solange `func_8007F8CC` „läuft noch" meldet. |
| `D_8009F496` | `gPendingFlag` | `u8` | Ausstehendes Flag, das `func_8007FACC` konsumiert. |
| `D_8009F4D4` | `gInitGuardFlag` | `u8` | Wächter gegen einen zweiten Durchlauf der einmaligen Arbeit. |
| `D_8009F4DC` | `gChunkSkipFlag` | `s32` | Bit 0 bewirkt, dass Chunks ab 0x424C Bytes übersprungen werden. |
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
| `D_8009CBD0` | `gSectorBufferPtr` | `struct State` | Pufferzeiger, der je abgeschlossenem Sektor um `unk8` weiterläuft. |
| `D_8009CC88` | `gFrameClockAnchor` | `char[]` | Anker der Frame-Uhr; die Initialisierung liefert diese Adresse oder null. |
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
| `D_8009F26C` | `gChannelEnableFlag` | `u8` | Aktivierungs-Byte; nur bei ungleich 0 läuft der Slot-Durchlauf. |
| `D_8009F640` | `gStreamStatePair` | `s32[2]` | Zwei Zustandswörter, beim Aufsetzen genullt. |
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
| `D_8009FA84` | `gModuleLoadedFlag` | `s32` | Nur bei ungleich 0 läuft der Modul-Ladepfad. |
| `D_8009C1CC` | `gSpecialistModules[5]` | `s32[5]` | Die fünf Spezialisten-Module, sonst `"PA\"`. |
| `D_8009F678` | `gSlotVoiceHandle` | `s32[]` | Voice-Handle je Slot; wird gestoppt, wenn der Track wechselt. |
| `D_8009FA78` | `gSlotTrackId` | `s32[]` | Track-Id je Slot für denselben Vergleich. |
| `D_8009ABE8` | `gItemCodeTable` | `u8[]` | Sechs Item-Codes der Auswahl im Panel. |
| `D_800A0A80` | `gAssetHandle` | `s32[3]` | Handle, Resource und Flag eines Assets samt Zustand. |
| `D_800A0A88` | `gPlayerCountRecord` | `int` | Spielerzahl-Datensatz; nur der Wert 1 zählt als Einzelspieler. |
| `D_8009F688` | `gMissionParam` | `s32` | Parameter für `func_80059BFC(flag, …)`, Flag 2 bei `unk0C == 2`. |

**Nicht umbenannt:** `D_8009F61C` (Anfangs-Statuswort des CD-Lesers) und `D_8009F618` (Klammer-Flag um den Callback-Aufruf) — bei beiden kippt der Byte-Vergleich nach dem Rename, sie bleiben mit Retail-Namen stehen.

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
| `D_8009E0A0` | `gRatioTable` | `s16[]` | Verhältnis-Tabelle, in die das Ergebnis einer Divisionsschutz-Prüfung indiziert. | `80095C30.c:57` |
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
| `D_800A2970` | `gClipRect` | `Grid` | Rechteck `(x, y, Breite, Höhe)`, auf dem der Durchlauf arbeitet. | `80062614.c:19` |
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
| `D_8009FAA0` | `gScreenUvBase` | `u32` | UV-Basiswort für die drei Bildschirm-Vertizes (`<< 16 | 0x101`). |
| `D_800A3548` | `gHeaderPrimitiveRecord` | `u8[]` | Kopf-Primitiv, das aus den Clip-Flag-Bytes des Modells gefüllt wird. |
| `D_8009B234` | `gModelTextureTable` | `Texture[]` | Textur-Einträge je Modell; das Feld `+0x72` (s8) wählt den Index, `< 0` bedeutet keine Textur. |
| `D_800A6F38` | `gTileDefTable` | `u8[]` | 100-Einträge-Tabelle der Tile-Definitionen; `func_80065CA8(tile, index, x, y)` zeichnet daraus. |
| `D_8009F80C` | `gTileMapTable` | `Other *` | Tile-Karte; `->cells` wird auf 0..53 geklemmt und in `gTileDefTable` nachgeschlagen. |
| `D_8009F808` | `gTileBlockTable` | `int *` | Block-Tabelle: `block = [((slot >> 2) & 0x3FFF)]`. |
| `D_8009B1A4` | `gTileShapeTable` | `s32[]` | Formcode je Tile: `>> 3` wählt im Switch die Vertex-Menge `v[0..7]` (zwei Varianten). |
| `D_8009F83C` | `gTileColourWord` | `u16` | Farbwort, das aus den Farb-Bytes des Tile-Records gesetzt wird. |
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
