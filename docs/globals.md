# Globale Hardware-Zeiger in Future Cop: L.A.P.D.

**Stand: 16. November 2026 · US-Version (`SLUS_007.39`) · Dokumentation aus Novas Analyse zusammengefasst**

## Worum geht es hier?

Diese Datei erklärt **49 globale Variablen**, die sich im Originalprogramm befinden und als Inhalt eine Adresse eines PlayStation-Hardwareregisters tragen. Es sind **49 verschiedene Variablen**, aber nur **37 verschiedene Hardware-Zieladressen**: Manche Programmteile besitzen einen eigenen Zeiger auf dasselbe Register.

Die Datei dokumentiert **nur diese bisher untersuchten Hardware-Zeiger**, nicht alle globalen Variablen des Spiels. Sie hilft beim Verstehen des C-Codes und beim Nachschlagen, welche Funktion mit welchem Gerät spricht. Der Abschnitt „Spielzustand, Treiber und Warteschlangen" enthält zusätzlich 33 normale Daten-Globals (Ringindizes, Treiberzustände, Rückrufe), die bereits im C-Code lesbare Namen tragen.

### Das wichtigste Prinzip

Ein Beispiel ist `D_8009DEC8`: An der RAM-Adresse `0x8009DEC8` liegt die Zahl `0x1F801814`. Diese Zahl ist die Adresse des GPU-Steuer- und Statusregisters. **Die globale Variable ist also nicht selbst das GPU-Register.**

```c
/* Lesbarer Name, ursprüngliches Assembler-Symbol bleibt erhalten. */
extern volatile u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");

u32 status = *gGpuGp1PortPtr;     /* GPU-Status aus der Hardware lesen */
*gGpuGp1PortPtr = 0x04000000;   /* GPU-Steuerbefehl an die Hardware senden */
```

Das einfache Lesen der **Zeigervariablen** liefert lediglich die Adresse des Gerätes. Das `*` davor greift auf das eigentliche Hardware-Register zu. Gerade diese Unterscheidung ist für die Decompilation wichtig.

## Übersicht nach Aufgabenbereich

Die **RAM-Adresse** bezeichnet die Stelle, an der Future Cop den Zeiger speichert. **Geräteadresse** ist der 32-Bit-Wert, der im Original-Executable an dieser Stelle steht. Der **C-Name** ist der verständliche Name aus Novas Dokumentation; das ursprüngliche `D_...`-Symbol bleibt für den Assembler erhalten. Eine **Beispielfunktion** zeigt, wo der Zugriff nachvollzogen werden kann.

### Grafik und GPU

Diese Einträge betreffen die Grafikausgabe und den Datentransfer zur GPU. GP0 überträgt Grafikbefehle und Daten; über GP1 werden Steuersignale geschickt und der GPU-Status gelesen.

| Global (RAM) | C-Name | Geräteadresse | Verwendung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DEC4` | `gGpuGp0PortPtr` | `0x1F801810` | GPU-Befehle und Bilddaten (GP0/GPUREAD) | `func_8009446C` |
| `D_8009DEC8` | `gGpuGp1PortPtr` | `0x1F801814` | GPU-Steuerbefehle und Status (GP1/GPUSTAT) | `func_80095350` |
| `D_8009CC18` | `gGpuGp1StatusPtr` | `0x1F801814` | Zusätzlicher Zeiger für GPU-Statusabfragen | `func_80091270` |
| `D_8009DECC` | `gDma2MemAddrPtr` | `0x1F8010A0` | Speicheradresse eines GPU-DMA-Transfers | `func_8009446C` |
| `D_8009DED0` | `gDma2BlockCtrlPtr` | `0x1F8010A4` | Blockgröße und -anzahl für GPU-DMA | `func_800946A8` |
| `D_8009DED4` | `gDma2ChannelCtrlPtr` | `0x1F8010A8` | GPU-DMA starten und überwachen | `func_800949A0` |

### DMA-Steuerung und gemeinsame Register

DMA ist die direkte Übertragung von Daten zwischen Speicher und Geräten. Mehrere verschiedene Globals zeigen hier absichtlich auf dasselbe Steuerregister; sie werden von unterschiedlichen Programmteilen verwendet.

| Global (RAM) | C-Name | Geräteadresse | Verwendung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DD1C` | `gDmaControlPtr` | `0x1F8010F0` | Zentrale Freigabe und Prioritäten der DMA-Kanäle (DPCR) | `func_80091614` |
| `D_8009DD60` | `gDmaInterruptCtrlPtr` | `0x1F8010F4` | DMA-Interrupts aktivieren und quittieren (DICR) | `func_80091D0C` |
| `D_8009DD84` | `gDmaChannelRegBasePtr` | `0x1F801080` | Startadresse des DMA-Registerblocks | `func_80091D0C` |
| `D_8009DEE4` | `gDmaControlGpuPtr` | `0x1F8010F0` | DPCR-Zugriff aus dem Grafikbereich | `func_80094F4C` |
| `D_8009CBA4` | `gDmaControlCdromPtr` | `0x1F8010F0` | DPCR-Zugriff aus dem CD-ROM-Bereich | `func_8008FC30` |
| `D_8009E9F8` | `gDmaControlMdecPtr` | `0x1F8010F0` | DPCR-Zugriff aus dem MDEC-Bereich | `func_80096240` |
| `D_8009EEB4` | `gSpuDelayCtrlPtr` | `0x1F8010F0` | DPCR-Zugriff aus dem SPU-Treiber | `func_80096EB8` |

### Interrupts und Zeitmessung

Interrupts melden Ereignisse wie Bildwechsel oder Geräteantworten. Mit der Maske legt das Programm fest, welche Meldungen es verarbeitet.

| Global (RAM) | C-Name | Geräteadresse | Verwendung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009C788` | `gInterruptStatusPtr` | `0x1F801070` | Interruptstatus I_STAT lesen und quittieren | `func_8008A5BC` |
| `D_8009C808` | `gInterruptStatusStatePtr` | `0x1F801070` | Weiterer Blick auf I_STAT als Struktur | `func_8008DA7C` |
| `D_8009DD14` | `gInterruptStatusHalfPtr` | `0x1F801070` | 16-Bit-Zugriff auf I_STAT | `func_800916EC` |
| `D_8009DD18` | `gInterruptMaskPtr` | `0x1F801074` | Interruptmaske I_MASK lesen oder setzen | `func_800915FC` |
| `D_8009CC1C` | `gTimer1CounterPtr` | `0x1F801110` | Zählerstand des Timers 1 | `func_80091270` |
| `D_8009DD54` | `gTimer1ModePtr` | `0x1F801114` | Betriebsart von Timer 1 | `func_80091BA0` |

### Controller und Speicherkarten (SIO0)

SIO0 ist die serielle Schnittstelle für Controller und Speicherkarten. Mehrere Globals zeigen auf dieselbe Registerbasis, aber der Code nutzt unterschiedliche Offsets und Zugriffsarten.

| Global (RAM) | C-Name | Geräteadresse | Verwendung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009C78C` | `gSio0RegBasePtr` | `0x1F801040` | Basis des SIO0-Registerblocks | `func_8008A9A8` |
| `D_8009C7A4` | `gSio0RxDataPtr` | `0x1F801040` | Datenbyte vom SIO0-Empfang lesen | `func_8008BA34` |
| `D_8009C7C0` | `gSio0CtrlPtr` | `0x1F801040` | SIO0-Steuerregister über Basis + 0xA | `func_8008BF78` |
| `D_8009C804` | `gSio0CtrlClearPtr` | `0x1F801040` | SIO0-Steuerregister zurücksetzen | `func_8008DA14` |

### CD-ROM und CD-DMA

Hier steuert das Spiel das Laufwerk, übergibt Befehle und Parameter und überträgt gelesene Sektordaten über DMA-Kanal 3.

| Global (RAM) | C-Name | Geräteadresse | Verwendung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009CB6C` | `gCdromIndexStatusPtr` | `0x1F801800` | CD-ROM-Registerbank wählen, Status lesen | `func_8008E7A0` |
| `D_8009CB70` | `gCdromCommandPtr` | `0x1F801801` | CD-Befehl senden bzw. Antwort lesen | `func_8008F244` |
| `D_8009CB74` | `gCdromParameterPtr` | `0x1F801802` | Parameter und Interrupt-Freigabe, bankabhängig | `func_8008F244` |
| `D_8009CB78` | `gCdromRequestStatusPtr` | `0x1F801803` | Anforderung und Interruptstatus, bankabhängig | `func_8008E7A0` |
| `D_8009CB7C` | `gComAccessDelayPtr` | `0x1F801020` | Gemeinsame Wartezeit der Gerätezugriffe | `func_8008F8E8` |
| `D_8009CBA0` | `gCdromAccessDelayPtr` | `0x1F801018` | Zugriffsverzögerung des CD-ROM einstellen | `func_8008FC30` |
| `D_8009CBA8` | `gDma3MemAddrPtr` | `0x1F8010B0` | Speicheradresse eines CD-DMA-Transfers | `func_8008FC30` |
| `D_8009CBAC` | `gDma3BlockCtrlPtr` | `0x1F8010B4` | Blocksteuerung für CD-DMA | `func_8008FD30` |
| `D_8009CBB0` | `gDma3ChannelCtrlPtr` | `0x1F8010B8` | CD-DMA starten und auf Abschluss warten | `func_8008FC30` |

### Video-Dekodierung (MDEC)

MDEC ist der Hardware-Dekoder für Videodaten. Die beiden zugehörigen DMA-Kanäle bewegen Daten zwischen Hauptspeicher und Dekoder.

| Global (RAM) | C-Name | Geräteadresse | Verwendung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009E9F0` | `gMdecCommandPtr` | `0x1F801820` | MDEC-Befehls- und Parameteranschluss | `func_80096240` |
| `D_8009E9F4` | `gMdecControlStatusPtr` | `0x1F801824` | MDEC-Steuer- und Statusregister | `func_80096150` |
| `D_8009E9C0` | `gDma0MemAddrPtr` | `0x1F801080` | Speicheradresse für DMA-Kanal 0 | `func_80096240` |
| `D_8009E9C4` | `gDma0BlockCtrlPtr` | `0x1F801084` | Blocksteuerung für DMA-Kanal 0 | `func_80096240` |
| `D_8009E9C8` | `gDma0ChannelCtrlPtr` | `0x1F801088` | DMA-Kanal 0 starten oder zurücksetzen | `func_80096150` |
| `D_8009E9CC` | `gDma1MemAddrPtr` | `0x1F801090` | Speicheradresse für DMA-Kanal 1 | `func_800962D0` |
| `D_8009E9D0` | `gDma1BlockCtrlPtr` | `0x1F801094` | Blocksteuerung für DMA-Kanal 1 | `func_800962D0` |
| `D_8009E9D4` | `gDma1ChannelCtrlPtr` | `0x1F801098` | DMA-Kanal 1 starten oder zurücksetzen | `func_80096150` |

### Ton (SPU) und Ton-DMA

Die SPU erzeugt und verarbeitet den Ton. Ein größerer Registerbereich steuert unter anderem Stimmen und Lautstärken; DMA-Kanal 4 bewegt Tondaten.

| Global (RAM) | C-Name | Geräteadresse | Verwendung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009EEA4` | `gSpuRegBasePtr` | `0x1F801C00` | Basis des SPU-Registerblocks | `func_80096EB8` |
| `D_8009CB80` | `gSpuRegsAltPtr` | `0x1F801C00` | Zweiter Zeiger auf die SPU-Registerbasis | `func_8008F7AC` |
| `D_8009EEA8` | `gDma4MemAddrPtr` | `0x1F8010C0` | Speicheradresse für SPU-DMA | `func_800973B4` |
| `D_8009EEAC` | `gDma4BlockCtrlPtr` | `0x1F8010C4` | Blocksteuerung für SPU-DMA | `func_800973B4` |
| `D_8009EEB0` | `gDma4ChannelCtrlPtr` | `0x1F8010C8` | SPU-DMA starten | `func_800973B4` |
| `D_8009EEB8` | `gSpuAccessDelayPtr` | `0x1F801014` | Zugriffsverzögerung der SPU einstellen | `func_80097940` |

### DMA-Kanal 6: Ordnungstabellen

Dieser DMA-Kanal dient dem Aufbau beziehungsweise Zurücksetzen von Ordnungstabellen, die für die Grafikverarbeitung benötigt werden (OTC = Ordering Table Clear).

| Global (RAM) | C-Name | Geräteadresse | Verwendung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DED8` | `gDma6MemAddrPtr` | `0x1F8010E0` | Startadresse für OTC-DMA | `func_8009415C` |
| `D_8009DEDC` | `gDma6BlockCtrlPtr` | `0x1F8010E4` | Anzahl der OTC-Einträge | `func_8009415C` |
| `D_8009DEE0` | `gDma6ChannelCtrlPtr` | `0x1F8010E8` | OTC-DMA starten und abschließen | `func_8009415C` |

## Spielzustand, Treiber und Warteschlangen

Diese Einträge sind **keine** Hardware-Zeiger, sondern Datenvariablen im RAM. Für sie wurde geprüft, welche Rolle sie im Code haben; die neuen Namen stehen auch im C-Code und sind über `__asm__("D_XXXXXXXX")` an das Retail-Symbol gebunden. Der Datentyp blieb jeweils unverändert, und das Byte-Matching der betroffenen Units wurde vor und nach jeder Umbenennung verglichen.

Konfidenz: **bestätigt** = Schreiben und Lesen belegen die Rolle eindeutig; **wahrscheinlich** = Rolle aus dem Code ableitbar, einzelne Details offen.

### Zeitmessung und Interrupts

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DD50` | `gVSyncTickCounter` | `volatile s32` | Zählt die VSync-Interrupts; der VBlank-Handler erhöht ihn, die Frame-Wartefunktion pollt ihn und bricht mit „VSync: timeout" ab. **Bestätigt.** | `func_80091BF8`, `func_800913E8` |
| `D_8009CC20` | `gTimerBaselineTick` | `volatile u32` | Letzte stabile Probe des Timers 1; verstrichene Ticks = `(now - gTimerBaselineTick) & 0xFFFF`, danach neu gesampelt. **Bestätigt.** | `func_80091270` |
| `D_8009DD20` | `gIrqStuckCounter` | `int` | Zählt Dispatch-Runden, in denen Status und Maske gesetzt bleiben; ab `0x800` Log und Status löschen, sonst zurücksetzen. **Rolle bestätigt.** | `func_800916EC` |
| `D_8009DEF0` | `gDispatchSavedIrqMask` | `u32` | Sichert die Interruptmaske über die GPU-Dispatch-Sektion und schreibt sie danach zurück. **Bestätigt.** | `func_80094A3C` |
| `D_8009DEF8` | `gSavedIrqMask` | `u32` | Dasselbe für den Transferpfad: Maske sichern, Arbeit erledigen, Maske zurück. **Bestätigt.** | `func_80094F4C`, `func_8009509C` |
| `D_8009DD10` | `gIrqCallbackTable` | `struct CallbackTable *` | Zeiger auf die Tabelle mit dem IRQ-Rücksprung, der beim Betreten/Verlassen der Handler gerufen wird. **Wahrscheinlich** (Inhalt der Tabelle nicht vollständig analysiert). | `func_80091480`, `func_800914B0` |

### GPU-Warteschlange

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DEE8` | `gGpuJobHead` | `volatile s32` | Schreibindex des 64 Einträge großen GPU-Auftragsrings; voll, wenn `((head + 1) & 0x3F) == tail`. **Bestätigt.** | `func_80094A3C`, `func_8009509C` |
| `D_8009DEEC` | `gGpuJobTail` | `volatile s32` | Leseindex desselben Rings; `count = (head - tail) & 0x3F` ist die Zahl wartender Aufträge. **Bestätigt.** | `func_8009509C`, `func_80094F4C` |

### Ton (SPU-Treiber)

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009EEBC` | `gSpuTransferAddr` | `u16` | Aktuelle SPU-RAM-Adresse für den FIFO-Transfer; wird in das Transferadressregister geschrieben. **Bestätigt.** | `func_80097138`, `func_80098EE8` |
| `D_8009EECC` | `gSpuAddrShift` | `s32` | Schiebewert zwischen SPU-Adresse und Byte-Offset (`<< gSpuAddrShift`), im Retail 3. **Bestätigt.** | `func_80097808`, `func_80098EE8` |
| `D_8009EEC8` | `gSpuAlignEnable` | `s32` | Schalter für die Adressausrichtung, im Retail 2. **Wahrscheinlich.** | `func_80096EB8`, `func_80097808` |
| `D_8009EED0` | `gSpuAlignBase` | `s32` | Basiswert der Ausrichtung, im Retail 8. **Wahrscheinlich.** | `func_80097808` |
| `D_8009EED4` | `gSpuAlignMask` | `s32` | Maske der Ausrichtung (`addr &= ~mask`), im Retail 7. **Wahrscheinlich.** | `func_80097808` |
| `D_8009EEE4` | `gSpuInitData` | `u8[]` | 0x10-Byte-Block, der beim Kaltstart nach SPU-RAM `0x200` übertragen wird. **Wahrscheinlich.** | `func_80096EB8` |

### Controller (SIO0)

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009C760` | `gPadPresentFlag` | `s32` | 1, sobald ein Controller erkannt wurde; steuert, ob die Ports abgefragt werden. **Bestätigt.** | `func_8008A290`, `func_8008A43C` |
| `D_8009C768` | `gPadSlotIndex` | `int` | Laufender Port-Index beim Abfragen der Slots (Einträge liegen bei `Basis + Index * 240`). **Bestätigt.** | `func_8008A43C`, `func_8008A6CC` |
| `D_800A6FF8` | `gPadActuators` | `Slot[]` | Ein Eintrag pro Port für den Aktuator-Zustand (Rumble-Motoren). **Wahrscheinlich.** | `func_8001E9AC`, `func_8001E87C` |

### CD-ROM-Treiber

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009CB84` | `gCdStateBytes` | `u8[]` | Drei Zustandsbytes des Treibers; nach einem Reset steht `[0] = 2`, während einer Antwort werden sie fortgeschrieben. **Bestätigt.** | `func_8008F6D8`, `func_8008E7A0` |
| `D_8009C8B4` | `gCdStatusByte` | `u8` | Letztes Statusbyte einer CD-Antwort; wird unter anderem auf Bit `0x10` geprüft. **Bestätigt.** | `func_8008E7A0`, `func_8008E010` |
| `D_8009C8B0` | `gCdLogLevel` | `int` | Größer 0 schaltet die CD-Protokollierung ein; der Setter gibt den vorherigen Wert zurück. **Bestätigt.** | `func_8008E0D8`, `func_8008E7A0` |
| `D_8009C8BC` | `gCdResponseCount` | `int` | Zähler der bearbeiteten CD-Antworten, wird pro Antwort erhöht. **Bestätigt.** | `func_8008E7A0`, `func_8008FF20` |
| `D_8009CBC4` | `gCdSeenResponseCount` | `s32` | Zuletzt verarbeiteter Stand von `gCdResponseCount`; eine Abweichung löst das Neulesen des Zustands aus. **Bestätigt.** | `func_8008FF20` |

### Diagnose

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009DDEC` | `gLogPrintf` | `void (*)(char *, ...)` | Funktionszeiger auf die zentrale Log-Routine; im Retail mit `func_8008E000` vorbelegt und im ganzen Code als Ausgabe verwendet. **Bestätigt.** | `func_80092910`, `func_800930B0` |

### Spielobjekte, Grafik und Zufall

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009F9E4` | `gCamera` | `struct Camera *` | Die aktive Kamera; die Funktionen setzen Position, Blickpunkt, Rotation und die daraus gebaute Matrix. **Bestätigt.** | `func_800177AC`, `func_80017860` |
| `D_8009AA38` | `gBaseMatrix` | `struct Matrix3x3` | Grundmatrix, aus der Instanzen ihre Transformationsmatrix kopieren. **Wahrscheinlich.** | `func_80013BD8` |
| `D_800A0300` | `gBoneMatrices` | `struct Matrix[]` | Matrix je Knochen-/Segmentindex; Eintrag `i-1` ist der Elternknoten von `i`. **Wahrscheinlich.** | `func_80013CB8` |
| `D_8009F214` | `gRandSeed` | `s32` | Zustand des Zufallszahlengenerators (Multiplikation mit 16807, Park-Miller). **Bestätigt.** | `func_80019720` |
| `D_8009F5B8` | `gResourceList` | `struct Resource *` | Kopf der einfach verketteten Ressourcenliste; wird in mehreren Funktionen durchlaufen und am Ende freigegeben. **Bestätigt.** | `func_80019AA8`, `func_800198DC` |
| `D_800A07A8` | `gSoundSystem` | `struct SoundSystem` | Globaler Zustand des Klangsystems mit Bank-Liste und Anzahl. **Bestätigt.** | `func_80021508` |

### Spieler und Match

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_8009FA0C` | `gPlayerCount` | `s32` / `int` | Anzahl der Spieler im laufenden Match (1 = Einzelspieler, 2 = Zweispieler); viele Pfade verzweigen darauf. **Bestätigt.** Die Funktionsdoku in `src/game/8002D1A0.c` nennt sie „player count". | `func_8002D1A0`, `func_80027510` |
| `D_8009F9EC` | `gSinglePlayerEntity` | `struct Ent *` / `Game *` | Zeiger auf die Spielfigur im Einzelspielermodus; wird nur bei `gPlayerCount == 1` ausgewertet und über die Entity-Id verglichen. **Bestätigt.** | `func_8002D1A0`, `func_80027510` |

<!-- OFFEN: D_8009FA58 (Zwei-Slot-Array der Spieler-Entities) bleibt vorerst
     unbenannt. In src/game/80027510.c wird dasselbe Symbol doppelt deklariert
     (einmal als Union `D_8009FA58`, einmal als Array `D_8009FA58_arr`); eine
     Umbenennung verändert dort die erzeugten Bytes. Erst die Doppeldeklaration
     auflösen, dann benennen. -->

### Controller-Puffer

| Global (RAM) | C-Name | Datentyp | Bedeutung | Beispielfunktion |
|---|---|---|---|---|
| `D_800AAC50` | `gPadMappingTable` | `struct PadMapping[]` | Tastenzuordnung je Port. **Bestätigt.** | `func_8001E588` |
| `D_800A78F0` | `gPadBuffers` | `struct PadBuffer[]` | Empfangspuffer je Port, aus dem die Pad-Bits gelesen werden. **Wahrscheinlich.** | `func_8001E7B8`, `func_8001E9AC` |

Alle hier genannten Variablen behalten ihren ursprünglichen Typ; die betroffenen Units sind nach der Umbenennung weiterhin byte-identisch.

## Vier Beispiele etwas genauer

**1. Die GPU steuern (`D_8009DEC8`).** Der Initialwert `0x1F801814` ist im Retail-Executable belegt: Datei-Offset `0x8E6C8`, Bytes `14 18 80 1F`. Die Funktion `func_80094144` lädt zunächst diesen Zeiger und liest danach GPUSTAT. In `func_80095350` werden dagegen GP1-Steuerbefehle ausgegeben. Laut Novas Prüfung nutzen 18 Funktionen diesen globalen Zeiger.

**2. Einen Interrupt quittieren (`D_8009C788`).** Der Zeiger enthält `0x1F801070` (I_STAT). `func_8008A5BC` schreibt `0xFFFFFFFE` an das Register, um den betreffenden Interrupt zu quittieren. In der Controller-Verarbeitung werden außerdem Statusbits abgefragt. Unterschiedliche Funktionen greifen teils als 16-Bit-, teils als 32-Bit-Wert darauf zu.

**3. Eine Bildübertragung vorbereiten (`D_8009DECC`, `D_8009DED0`, `D_8009DED4`).** Diese drei Variablen zeigen auf die Register von DMA-Kanal 2: Zuerst wird die Speicheradresse festgelegt, dann die Blocksteuerung und zuletzt der Kanal gestartet. Solche zusammengehörigen Zeiger erkennt man besonders gut an den gemeinsam aufgerufenen Funktionen `func_8009446C` und `func_800946A8`.

**4. Den Ton vorbereiten (`D_8009EEA4`).** Der Wert `0x1F801C00` verweist auf den Beginn des SPU-Registerblocks. Novas Einzelanalyse nennt 14 Funktionen, darunter `func_80096EB8` für die Einrichtung und `func_80097138` für Übertragungen zum Tonspeicher. Die vielen Offsets hinter diesem Zeiger sind Hardware-Register, nicht automatisch Felder einer Spielstruktur.

## Wichtige Erkenntnisse für die C-Decompilation

**Verständliche Namen sind bereits möglich.** Nova hat dokumentiert, dass die C-Dateien lesbare Namen wie `gGpuGp1PortPtr` verwenden und sie über `__asm__("D_8009DEC8")` an die ursprünglichen Retail-Symbole binden. Laut den beigefügten Einzelberichten wurde das Byte-Matching der betroffenen Units nach der Umbenennung erneut geprüft. Diese Zusammenfassung führt keine eigene Build-Prüfung aus.

**Noch keine C-Definitionen anlegen.** Die vier Bytes eines Globals liegen bisher im Retail-Payload, das in der vorhandenen Build-Struktur als zusammenhängender Assemblerblock (`asm/us/main.s`) eingebunden ist. Eine zusätzliche Definition wie `volatile u32 *gGpuGp1PortPtr = ...;` würde die Variable nicht automatisch an `0x8009DEC8` platzieren. Für echte C-Datendefinitionen müssten zunächst Datensegmente und Linker-Platzierung kontrolliert umgestellt und anschließend Byte für Byte geprüft werden.

**Die Typen nicht pauschal vereinheitlichen.** Fachlich ist bei Hardware-Registern meist ein `volatile`-Ziel richtig. Im bestehenden Matching verwenden einzelne Funktionen trotzdem unterschiedliche Deklarationen. Nova hat beim GPU-Zeiger gemessen, dass eine einheitliche Deklaration mehrere Units verändert. Deshalb die schon passenden Typen nicht ungeprüft ersetzen.

**Gleiche Geräteadresse bedeutet nicht gleiche globale Variable.** `D_8009DEC8` und `D_8009CC18` zeigen beide auf `0x1F801814`. Auch fünf verschiedene Globals zeigen auf `0x1F8010F0` (DMA-Steuerregister DPCR). Das sind unterschiedliche Speicherstellen im Spiel, die nicht einfach zusammengelegt werden dürfen.

**Hardware ist nicht dasselbe wie Spiellogik.** Diese Datei erklärt vor allem den Kontakt zur PlayStation-Hardware. Spielerpositionen, Gegnerzustände, Kamera und ähnliche globale Spielobjekte sind damit noch nicht dokumentiert.

## Wie Nova diese Datei weiterführen kann

Am besten **eine neue Variable nach der anderen** ergänzen und die bestehende Gliederung beibehalten. Ein neuer Eintrag braucht zunächst nur:

1. Den ursprünglichen Namen `D_XXXXXXXX`, die RAM-Adresse und – falls aus dem Original-Executable belegt – den Initialwert.
2. Einen verständlichen C-Namen und einen kurzen Satz, wofür die Variable benutzt wird.
3. Ein oder zwei Funktionen, die diese Bedeutung belegen.
4. Den Vermerk **„bestätigt“**, **„wahrscheinlich“** oder **„noch offen“**, falls Zuordnung oder Typ unsicher sind.

Keine weiteren 49 Einzeldateien nötig. Ausführliche Assembler-Ausschnitte oder experimentelle Analysedaten nur dann aufnehmen, wenn sie einen zweifelhaften Befund wirklich erklären. Bestehende C-Dateien und Matching-Profile sollen durch die Pflege dieser Dokumentation nicht verändert werden.

## Herkunft und Prüfstand

Grundlage sind die **49 Einzelberichte** und die `README.md` aus dem übergebenen Archiv `globals.zip` vom 16. November 2026. Die Adressen, C-Namen, Geräte-Zuordnungen und genannten Funktionsbeispiele sind daraus übernommen und nach Aufgabenbereichen zusammengefasst. Die Originaldateien `SLUS_007.39`, `main.s` und die aktuellen C-Builds wurden für diese Zusammenfassung **nicht erneut ausgeführt oder unabhängig geprüft**. Die dokumentierten Matching-Ergebnisse sind somit Novas Prüfstand, keine neue Verifikation durch diese Datei.
