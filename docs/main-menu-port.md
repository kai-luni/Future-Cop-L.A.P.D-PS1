# Main-menu selection and music notes

The initial seven menu records are `gMainMenuItems` (`D_8009BBF8`, 0x1c
bytes each) in `asm/us/main.s`, installed by `gSetupFrontEndMenus`
(`func_80077AD4`), called by `func_800780DC`.
`gUpdateMainMenuUnlocks` (`func_8007A168`, `src/game/gpu-2d-hud/gUpdateMainMenuUnlocks.c`) enables items 2 and 5 only
when the relevant progress or flags are set. They have no text ID before
unlocking. The initial enabled indices are 0, 1, 3, 4 and 6.

`gSelectEnabledMenuItem` (`func_800787D8`, `src/game/boot-frontend/gSelectEnabledMenuItem.c`) scans from a candidate,
wraps at either end, and selects the first enabled record in the requested
player/group. If no matching item is found, it leaves the previous selection
unchanged; direction zero tests only the initial candidate. `tests/host/menu_navigation_test.sh` compiles this C
function and checks the vectors in the sibling .NET repo under QEMU; the same
vectors drive the .NET test. The QEMU harness is not PS1 hardware.

The FE.MIS `MenuTune1_sw14.` stream begins at 0x96000 on the US retail disc.
Its 54 `VAGM` blocks each contain two 0x2fc0-byte ADPCM channels; padding
`FILL` chunks appear between blocks. `func_800244A4` requests 0x37b4 (14260)
samples per second. A stereo decode of the concatenated channel blocks yields
1,155,168 samples per channel (about 81 seconds). This is a stream-format
observation and a software decode, not parity with SPU output.
