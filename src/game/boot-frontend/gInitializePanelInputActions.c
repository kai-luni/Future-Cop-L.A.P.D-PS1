// Categories: Input > Configuration
#include "game/types.h"

struct Unk247A8 {
    u32 unk0;
    s32 unk4;
};

extern struct Unk247A8 gMapOpenPanelAction[2] __asm__("D_800A0AA0");
extern struct Unk247A8 gOptUpAction[2] __asm__("D_800A0AB0");
extern struct Unk247A8 gOptDownAction[2] __asm__("D_800A0AC0");
extern struct Unk247A8 gOptDecreaseAction[2] __asm__("D_800A0AD0");
extern struct Unk247A8 gOptIncreaseAction[2] __asm__("D_800A0AE0");
extern struct Unk247A8 gMapDigit2Action[2] __asm__("D_800A0AF0");
extern struct Unk247A8 gMapDigit3Action[2] __asm__("D_800A0B00");
extern struct Unk247A8 gMapDigit4Action[2] __asm__("D_800A0B10");
extern struct Unk247A8 gBackActionRecord[2] __asm__("D_800A0B20");

extern u8 gPresetIndexPerPort[] __asm__("D_8009FA98");

extern s32 D_8009F660;
extern s32 gSinglePlayerEntity __asm__("D_8009F9EC");
extern s32 gFrameTickBase __asm__("D_8009FA30");
extern u16 gSecondTeamRetireTimer __asm__("D_8009F65E");
extern u16 gTeamFlag __asm__("D_8009F65C");
extern u16 gPlayerFadeTimers __asm__("D_8009FA44");
extern s32 D_8009F664;

void gInitializePanelInputActions(void) __asm__("func_800247A8");

/**
 * @brief Resets per-player transition state and initializes the two ports' panel action
 * records.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clears the entity pointer, frame base and both pairs of transition halfwords. Initializes
 * nine eight-byte action records per port with phase 2, selecting four masks according to bit 1
 * of that port's preset byte.
 *
 * @par Globals
 * Reads gPresetIndexPerPort. Initializes gMapOpenPanelAction, gOptUpAction, gOptDownAction,
 * gOptDecreaseAction, gOptIncreaseAction, gMapDigit2Action, gMapDigit3Action, gMapDigit4Action
 * and gBackActionRecord. Resets gSinglePlayerEntity, gFrameTickBase, gTeamFlag,
 * gSecondTeamRetireTimer and both gPlayerFadeTimers halfwords, and writes D_8009F660 and
 * D_8009F664.
 *
 * @par Callers
 * Direct calls from gStartModuleByName (func_80024E3C).
 *
 * @par Role and path
 * Module startup resets these records before the map and options panels decode button edges.
 *
 * @par Unknown
 * D_8009F660 and D_8009F664 are unresolved beyond their initialization values.
 */
void gInitializePanelInputActions(void)
{
    s32 i;

    D_8009F660 = 0x40000;
    gSinglePlayerEntity = 0;
    gFrameTickBase = 0;
    gSecondTeamRetireTimer = 0;
    gTeamFlag = 0;
    *(u16 *)0x8009FA46 = 0;
    gPlayerFadeTimers = 0;
    D_8009F664 = 2;

    for (i = 0; i < 2; i++) {
        gMapOpenPanelAction[i].unk0 = 0x40000;
        gMapOpenPanelAction[i].unk4 = 2;
        gOptUpAction[i].unk0 = 0x40000000;
        gOptUpAction[i].unk4 = 2;
        gOptDownAction[i].unk0 = 0x80000000;
        gOptDownAction[i].unk4 = 2;
        gOptDecreaseAction[i].unk0 = 0x10000000;
        gOptDecreaseAction[i].unk4 = 2;
        gOptIncreaseAction[i].unk0 = 0x20000000;
        gOptIncreaseAction[i].unk4 = 2;
        if (gPresetIndexPerPort[i] & 2) {
            gMapDigit2Action[i].unk0 = 0x1000000;
            gMapDigit3Action[i].unk0 = 0x8000000;
            gMapDigit4Action[i].unk0 = 0x4000000;
            gBackActionRecord[i].unk0 = 0x400000;
        } else {
            gMapDigit2Action[i].unk0 = 0x4000000;
            gMapDigit3Action[i].unk0 = 0x400000;
            gMapDigit4Action[i].unk0 = 0x1000000;
            gBackActionRecord[i].unk0 = 0x8000000;
        }
        gMapDigit2Action[i].unk4 = 2;
        gMapDigit3Action[i].unk4 = 2;
        gMapDigit4Action[i].unk4 = 2;
        gBackActionRecord[i].unk4 = 2;
    }
}
