# Single-Player Precinct Assault Front-End Path

## Overview

The single-player Precinct Assault front-end path transitions from the main menu item dispatch into mode configuration, player setup, the 1P Precinct Assault hub, mission selection, loadout selection, and finally mission module launch. It starts in the per-frame menu loop `func_80078DD0` (`0x80078DD0`) when the user confirms entry 3 of `gMainMenuItems` (`D_8009BBF8`), invoking `gSelectPrecinctAssault1P` (`func_8007A4CC`). After stepping through setup (`D_800A37A8`), hub (`D_800A38C8`), mission select (`D_800A3838`), and loadout (`D_800A3628`), `gFinalizeMissionLoadout` (`func_8007E3EC`) writes the six equipment bytes into `gGameSession` (`D_8009FA94`) and sets `gScreenState` (`D_8009FA00`) to `5`. That exit state leaves the UI frame loop `func_80027EE4` (`0x80027EE4`), maps `gGameSession->unk10` to `2` in `gFinishScreenModule` (`func_80025B7C`), and causes `gGameMain` (`func_80076A2C`) to call `gRunScreenModule(gFrontEndModuleName, 0)` to launch the level.

## Hop-by-hop path

1. **Main menu per-frame dispatch (`0x80078DD0`, `func_80078DD0`)**: Called each frame by `func_80027EE4` (`0x80027EE4`) while `gScreenState` (`D_8009FA00`) is `4`. After `gUpdateTitleScreenTimeout` (`func_800778E4` at `0x800778E4`) activates `gMenuDescriptor` (`D_800A3808`), `func_80078DD0` polls controllers `0`/`1`, draws via `gDrawModeHeadingPage` (`func_8007A2C4` at `0x8007A2C4`), and on confirm of entry `3` calls `item->handler(3, 0, 0, 0, 3)` -> `gSelectPrecinctAssault1P` (`func_8007A4CC`).
2. **1P Precinct Assault mode select (`0x8007A4CC`, `gSelectPrecinctAssault1P`)**: When `D_800A35CC == 0`, sets `gGameSession->players` (`+0x0C`) = `1`, `split` (`+0x5C`) = `1`, `D_800A377D[0] = 1`, `D_8009F459 = 0`, and calls `gSetupPrecinctAssaultMissionList` (`func_80077708` at `0x80077708`). That binds `gMissionListTablePtr` (`D_8009F884`) = `gFrontEndListTable` (`D_8009B72C`), `gMissionListMode` (`D_8009F889`) = `1`, `gMissionListMinIndex` (`D_8009F881`) = `0`, `gSelectedMissionIndex` (`D_8009F880`) = `0`, `gMissionListMaxIndex` (`D_8009F882`) = `gUnlockFlag + 3`, and `gMissionListEntryCount` (`D_8009F888`) = `gUnlockFlag + 4` (with `gUnlockFlag` / `D_8009F3C2` updated by `gUpdatePrecinctUnlockFlag` / `func_80077FFC` when `D_8009F3C0 != 0`). Sets `gDeferredTransition` (`D_8009F8C8`) = `5`, queues `gMultiBrfMovieName` (`D_80012094`) once if `gLayoutPointerFlagA` (`D_8009F3E6`) == `0`, and sets `D_800A368B = 9`.
3. **Deferred transition & briefing movie (`0x80078DD0`, `func_80078DD0` -> `0x80078650`, `func_80078650`)**: At the end of `func_80078DD0`, `gDeferredTransition == 5` plays `gPendingRowCapture` via `func_80082DDC` if non-null, then either stores `&D_800A37A8` into `D_8009B8B8[0]` and pushes `D_800A3688` (`func_80078650(&D_800A3688, 1)`) or pushes `D_800A37A8` directly (`func_80078650(&D_800A37A8, 1)`). Clears `gDeferredTransition = -1` and `gPendingRowCapture = 0`.
4. **Player setup (`D_800A37A8` -> `0x8007B004`, `gInitMissionSelectState` -> `0x8007A5F0`, `gHandlePlayerJoinSelection`)**: `func_80078650` activates `D_800A37A8` and runs `gInitMissionSelectState`, copying `gGameSession->unk08`/`unk98` to `gPlayerSelection` (`D_8009F8F0`)/`D_8009F8F4`, clearing `D_8009F8E0`/`D_8009F8E4`, and setting `D_800A37AB[0] = 9`. Per-frame UI uses `gDrawMenuCommandsRow` (`func_8007B17C`), `gMenuReadyToAdvancePredicate` (`func_8007B0D4`), and `gStepPlayersOptionPane` (`func_8007A7A0`, caller dispatch unproven). On confirm, `gHandlePlayerJoinSelection` writes `gPlayerSelection`/`D_8009F8F4` back to `gGameSession` and pushes the 1P Precinct Assault hub `D_800A38C8` (`func_80078650(&D_800A38C8, 1)`).
5. **1P Precinct Assault hub (`D_800A38C8` -> `0x8007B3F4`, `gEnterGameHubMenu` -> `0x8007BCF4`, `gDrawGameHubMenu`)**: `gEnterGameHubMenu` sets `D_800A38C8.unk24 = &gMenuDescriptor` (`D_800A3808`) and selects item `3` via `gSelectEnabledMenuItem(3, 1, 0)` when `gRowClearFlag` (`D_8009FA10`) != `0`. `gDrawGameHubMenu` renders via `gDrawProfileSlotList` (`func_8007B460`). Handoff from `D_800A38C8` (`D_8009BE28`) to level select `gScreenDescriptor` (`D_800A3838`): exact item record in `D_8009BE28` is unproven.
6. **Level select (`D_800A3838` -> `0x8007B854`, `gHandleProfileListItem`)**: `gHandleProfileListItem` steps `gSelectedMissionIndex` (`D_8009F880`) or `gSelectedRow` (`D_8009F87E`). On confirm (`arg0 == 3`) with `split == 1` and `players == 1`, it promotes `gPlayerMenuDepth[0]` (`D_8009F8C0`) to `2` if `SELECTED.unk1F != 0`, stores the level index in `gGameSession->unk4C`, pushes 1P loadout `D_800A3628` (`func_80078650(D_800A3628, 1)`), sets `gDeferredTransition = 2`, copies `SELECTED.unk0` to `gFrontEndModuleName` (`D_8009B300`), sets `gPendingModuleNameFlag` (`D_8009F3EC`) to `D_8009F488` if equal to `D_8009F444` (`"lax1"`) else `0`, and sets `gModuleLoadState` (`D_8009F3F0`).
7. **Loadout (`D_800A3628` -> `0x8007D408`, `gInitSelectionIndices` -> `0x8007E3EC`, `gFinalizeMissionLoadout`)**: `gInitSelectionIndices` configures `gLoadoutTable` (`D_8009C0A8[0..2]`), clamps `D_8009F924`/`D_8009F926`/`D_8009F928`, and sets `D_800A362B[0] = 7`; `gDrawLoadoutScreen` (`func_8007DD08`) draws the three rows. On confirm, `gFinalizeMissionLoadout` calls `gClampLoadoutSelections(1)` (`func_8007D2DC`), writes the six equipment bytes to `gGameSession[10..15]` (`+0x28..+0x3C`), sets `gFrontEndReturnMode` (`D_8009F45A`) = `2`, clears `gRowClearFlag = 0`, and sets `gScreenState` (`D_8009FA00`) = `5`.
8. **Game start (`0x80027EE4` -> `0x80025B7C` -> `0x80076A2C` -> `0x800287F4`)**: `gScreenState = 5` exits `func_80027EE4`; `gFinishScreenModule` (`func_80025B7C`) maps state `5` via `jtbl_80010DDC` to `gGameSession->unk10 = 2` and unloads front-end subsystems (`gUnloadFrontEndSubsystems`). `gGameMain` (`func_80076A2C`) sees `gPendingScreenState = 2` (`!= 0`) and loops to `gRunScreenModule(gFrontEndModuleName, 0)` (`func_800287F4`), which calls `gStartModuleByName` (`func_80024E3C`) and enters the gameplay loop `func_80028618` (`0x80028618`).

## Function table

| address | retail symbol | current C name | one-line role |
|---|---|---|---|
| `0x800776B8` | `func_800776B8` | `gSetupCrimeWarMissionList` | Binds 8-entry Crime War list `D_8009B724` and mode `0` |
| `0x80077708` | `func_80077708` | `gSetupPrecinctAssaultMissionList` | Binds 5-entry Precinct Assault list `D_8009B72C` and mode `1` |
| `0x80077FFC` | `func_80077FFC` | `gUpdatePrecinctUnlockFlag` | Sets `D_8009F3C2 = 1` when `D_8009B72C[0..3]` states equal `10` |
| `0x80079FE0` | `func_80079FE0` | `gPlayCrimeWarMissionMovies` | Plays Crime War mission movies from `D_8009B724` plus credits |
| `0x8007A0A4` | `func_8007A0A4` | `gPlayPrecinctMissionMovies` | Plays Precinct Assault mission movies from `D_8009B72C` plus credits |
| `0x8007A168` | `func_8007A168` | `gUpdateMainMenuUnlocks` | Enables main-menu movie reel items 2 and 5 when unlocked |
| `0x8007A2C4` | `func_8007A2C4` | `gDrawModeHeadingPage` | Draws three mode-heading strings and queues two display IDs |
| `0x8007A3AC` | `func_8007A3AC` | `gSelectCrimeWar1P` | Main-menu entry 0 handler: configures 1P Crime War and `cwrules.wve` |
| `0x8007A450` | `func_8007A450` | `gSelectCrimeWar2P` | Main-menu entry 1 handler: configures 2P Crime War and `cwrules.wve` |
| `0x8007A4CC` | `func_8007A4CC` | `gSelectPrecinctAssault1P` | Main-menu entry 3 handler: configures 1P Precinct Assault and `MultiBrf.wve` |
| `0x8007A56C` | `func_8007A56C` | `gSelectPrecinctAssault2P` | Main-menu entry 4 handler: configures 2P Precinct Assault and `MultiBrf.wve` |
| `0x8007A5F0` | `func_8007A5F0` | `gHandlePlayerJoinSelection` | Setup-screen item callback: updates preview scale and pushes hub |
| `0x8007A7A0` | `func_8007A7A0` | `gStepPlayersOptionPane` | Steps per-player option in `D_8009F8F0` and draws label from `D_8009BEEC` |
| `0x8007B004` | `func_8007B004` | `gInitMissionSelectState` | Setup-screen `unk10` init: copies `D_8009FA94` fields and sets `D_800A37AB` |
| `0x8007B0D4` | `func_8007B0D4` | `gMenuReadyToAdvancePredicate` | Setup-screen `unk1C` predicate: returns `2` when ready to advance |
| `0x8007B17C` | `func_8007B17C` | `gDrawMenuCommandsRow` | Setup-screen `unk18` draw: draws header and queues prompt IDs |
| `0x8007B3F4` | `func_8007B3F4` | `gEnterGameHubMenu` | Hub `unk10` init: links parent `D_800A3808` and advances cursor if flagged |
| `0x8007B460` | `func_8007B460` | `gDrawProfileSlotList` | Draws mission/slot rows, completion markers, and progress bars |
| `0x8007B854` | `func_8007B854` | `gHandleProfileListItem` | Level-select item handler: steps index, pushes `D_800A3628`, sets module name |
| `0x8007BCF4` | `func_8007BCF4` | `gDrawGameHubMenu` | Hub `unk18` draw: calls `func_8007B460` and queues hub prompt IDs |
| `0x8007F648` | `func_8007F648` | `gRequestPauseMenu` | Sets `D_8009F3E8 = 1` on action `3` and restores screen via `func_8007F5D0` |
| `0x8007F678` | `func_8007F678` | `gSetMenuCursorToItem2` | `D_800A3838.unk10` init: sets `*D_8009F468 = 2` and returns `2` |
| `0x80080038` | `func_80080038` | `gDrawProfileDetailScreen` | Draws profile detail/save/load screen for `D_800A36E8` |
| `0x80080984` | `func_80080984` | `gApplySavedAudioSettings` | Restores audio volumes, device enables, and controller presets |

## Global table

| `D_` symbol | current C alias | meaning | retail initial value |
|---|---|---|---|
| `D_80012088` | `gCwRulesMovieName` | Crime War rules movie path | `"cwrules.wve"` (`.rdata`, unproven in `asm/us/main.s`) |
| `D_80012094` | `gMultiBrfMovieName` | Precinct Assault briefing movie path | `"MultiBrf.wve"` (`.rdata`, unproven in `asm/us/main.s`) |
| `D_8009B300` | `gFrontEndModuleName` | Active/selected module name buffer | unproven |
| `D_8009B4E4` | `gWideTextTable` | 36-byte mission/entry record table | unproven |
| `D_8009B724` | `gMissionListTable` | Crime War 8-entry mission index list | `{9, 11, 5, 1, 13, 4, 2, 0}` |
| `D_8009B72C` | `gFrontEndListTable` | Precinct Assault 5-entry mission index list | `{3, 7, 10, 15, 6}` |
| `D_8009B730` | `gFrontEndEntrySequence` | Attract-demo entry sequence (level, type, players, value) | `06 00 00 00 \| 09 00 01 00 \| 03 01 01 00 \| 05 00 02 00 \| 07 01 02 00` |
| `D_8009BB66` | `gMenuPreviewScaleP1` | P1 setup item preview scale (`100`/`110`) | unproven |
| `D_8009BBBA` | `gMenuPreviewScaleP2` | P2 setup item preview scale (`100`/`110`) | unproven |
| `D_8009BBF8` | `gMainMenuItems` | 7-entry main menu item table | unproven (table layout in Retail data anchors) |
| `D_8009F3C0` | `gFrontEndListFlag` | Cheat/unlock-all flag | `0` |
| `D_8009F3C2` | `gUnlockFlag` | Precinct Assault 5th-map unlock flag | `0` |
| `D_8009F3E6` | `gLayoutPointerFlagA` | One-shot `MultiBrf.wve` shown flag | `0` |
| `D_8009F3F0` | `gModuleLoadState` | Loading screen index | `1` |
| `D_8009F458` | `gCrimeWarProgress` | Crime War mission progress byte | `0` |
| `D_8009F459` | — | Precinct Assault progress byte | `0` |
| `D_8009F45A` | `gFrontEndReturnMode` | Return mode (`1`=CW, `2`=PA) | `0` |
| `D_8009F468` | `gFontBankTable` | Active menu descriptor pointer | `0` |
| `D_8009F880` | `gSelectedMissionIndex` | Selected index in active mission list | BSS |
| `D_8009F881` | `gMissionListMinIndex` | Minimum selectable mission index | BSS |
| `D_8009F882` | `gMissionListMaxIndex` | Maximum selectable mission index | BSS |
| `D_8009F884` | `gMissionListTablePtr` | Pointer to active mission index list | BSS |
| `D_8009F888` | `gMissionListEntryCount` | Active mission list entry count | BSS |
| `D_8009F889` | `gMissionListMode` | List mode (`0`=CW, `1`=PA) | BSS |
| `D_8009F8C4` | `gPendingRowCapture` | Queued movie path for transition | BSS |
| `D_8009F8C8` | `gDeferredTransition` | Pending menu transition id (`-1` idle) | BSS |
| `D_8009FA00` | `gScreenState` | Screen loop state (`4`=UI, `5`=exit to game) | BSS |
| `D_8009FA10` | `gRowClearFlag` | Progression/hub-return flag | BSS |
| `D_8009FA94` | `gGameSession` | Pointer to `0xF4`-byte session state block | BSS |

## Retail data anchors

- **`gMainMenuItems` (`D_8009BBF8`)**: 7 records x `0x1C` bytes (`+0x00` x, `+0x01` y, `+0x04` handler, `+0x0E` enabled, `+0x0F` player, `+0x16` text id `u16`, `+0x18` colour `u16`). Handlers: `0` -> `gSelectCrimeWar1P` (`func_8007A3AC`), `1` -> `gSelectCrimeWar2P` (`func_8007A450`), `2` -> `gPlayCrimeWarMissionMovies` (`func_80079FE0`, disabled at boot), `3` -> `gSelectPrecinctAssault1P` (`func_8007A4CC`), `4` -> `gSelectPrecinctAssault2P` (`func_8007A56C`), `5` -> `gPlayPrecinctMissionMovies` (`func_8007A0A4`, disabled at boot), `6` -> `NULL`, next `D_800A36E8` (load-game menu).
- **Entry 3 (1P Precinct Assault)**: text id `0x3E8`, `y = 124`, handler `gSelectPrecinctAssault1P` (`func_8007A4CC`).
- **Mission lists**: `gMissionListTable` (`D_8009B724`) = `{ 9, 11, 5, 1, 13, 4, 2, 0 }` (Crime War, 8 entries); `gFrontEndListTable` (`D_8009B72C`) = `{ 3, 7, 10, 15, 6 }` (Precinct Assault, 5 entries).
- **Movie name strings**: `gCwRulesMovieName` (`D_80012088`) = `"cwrules.wve"`; `gMultiBrfMovieName` (`D_80012094`) = `"MultiBrf.wve"`.

## Test coverage

The host test ([precinct_path_harness.c](../tests/host/precinct_path_harness.c), run via [precinct_path_test.sh](../tests/host/precinct_path_test.sh)) compiles the unchanged retail units `800776B8.c`, `80077708.c`, and `80077FFC.c` with GCC 2.7.2 / `maspsx` and runs them under `qemu-mipsel`. It proves that `gSetupCrimeWarMissionList` and `gSetupPrecinctAssaultMissionList` bind the retail tables, clear cursor indices, set `gMissionListMode`/`gMissionListMaxIndex`/`gMissionListEntryCount`, and that `gUpdatePrecinctUnlockFlag` and the `gFrontEndListFlag` cheat path behave as specified. It does not exercise controller dispatch, descriptor transitions, movie playback, profile I/O, loadout clamping, or module loading.

## Open questions

- Exact item record in `D_8009BE28` (hub `D_800A38C8`) that pushes `gScreenDescriptor` (`D_800A3838`) for level select.
- Exact caller/table entry dispatching `gStepPlayersOptionPane` (`func_8007A7A0`).
- Whether `gFrontEndModuleName` (`D_8009B300`) already carries `.mis` in `SELECTED.unk0` or receives it elsewhere on the `gGameSession->unk10 == 2` game-launch path (since `gGameMain` only appends `".mis"` in `case 0` when `gPendingModuleNameFlag` is set).
- Downstream consumer of `gScreenState` (`D_8009FA00`) and `gGameSession` fields inside the loaded level module (`func_80024A24` / `func_80028618`).
