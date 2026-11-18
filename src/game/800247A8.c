// Categories: Input > Configuration
#include "game/types.h"

struct Unk247A8 {
    u32 unk0;
    s32 unk4;
};

extern struct Unk247A8 gActionRecords[2] __asm__("D_800A0AA0");
extern struct Unk247A8 D_800A0AB0[2];
extern struct Unk247A8 D_800A0AC0[2];
extern struct Unk247A8 D_800A0AD0[2];
extern struct Unk247A8 D_800A0AE0[2];
extern struct Unk247A8 D_800A0AF0[2];
extern struct Unk247A8 gPanelPhaseTable[2] __asm__("D_800A0B00");
extern struct Unk247A8 gPanelChoiceTable[2] __asm__("D_800A0B10");
extern struct Unk247A8 gBackActionRecord[2] __asm__("D_800A0B20");

extern u8 gPresetIndexPerPort[] __asm__("D_8009FA98");

extern s32 D_8009F660;
extern s32 gSinglePlayerEntity __asm__("D_8009F9EC");
extern s32 gFrameTickBase __asm__("D_8009FA30");
extern u16 D_8009F65E;
extern u16 gTeamFlag __asm__("D_8009F65C");
extern u16 gPlayerFadeTimers __asm__("D_8009FA44");
extern s32 D_8009F664;

/**
 *  @brief Initializes shared values and two sets of mask records.
 *
 *  Sets fixed masks and record values, choosing four masks per set according
 *  to bit 1 of the corresponding configuration byte.
 *  @par Globals
 *  D_8009F660, D_8009F9EC, D_8009FA30, D_8009F65E, D_8009F65C,
 *  D_8009FA44, D_8009FA46, and D_8009F664 receive initial values.
 *  D_8009FA98 supplies configuration bits; D_800A0AA0 through D_800A0B20
 *  are the nine two-entry record arrays initialized by the function.
 *  @par Control flow
 *  Loops over two entries and branches on each configuration byte's bit 1.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the listed scalar values and both entries of each record array.
 *  @par Callers
 *  Confirmed: func_80024E3C.
 */
void func_800247A8(void)
{
    s32 i;

    D_8009F660 = 0x40000;
    gSinglePlayerEntity = 0;
    gFrameTickBase = 0;
    D_8009F65E = 0;
    gTeamFlag = 0;
    *(u16 *)0x8009FA46 = 0;
    gPlayerFadeTimers = 0;
    D_8009F664 = 2;

    for (i = 0; i < 2; i++) {
        gActionRecords[i].unk0 = 0x40000;
        gActionRecords[i].unk4 = 2;
        D_800A0AB0[i].unk0 = 0x40000000;
        D_800A0AB0[i].unk4 = 2;
        D_800A0AC0[i].unk0 = 0x80000000;
        D_800A0AC0[i].unk4 = 2;
        D_800A0AD0[i].unk0 = 0x10000000;
        D_800A0AD0[i].unk4 = 2;
        D_800A0AE0[i].unk0 = 0x20000000;
        D_800A0AE0[i].unk4 = 2;
        if (gPresetIndexPerPort[i] & 2) {
            D_800A0AF0[i].unk0 = 0x1000000;
            gPanelPhaseTable[i].unk0 = 0x8000000;
            gPanelChoiceTable[i].unk0 = 0x4000000;
            gBackActionRecord[i].unk0 = 0x400000;
        } else {
            D_800A0AF0[i].unk0 = 0x4000000;
            gPanelPhaseTable[i].unk0 = 0x400000;
            gPanelChoiceTable[i].unk0 = 0x1000000;
            gBackActionRecord[i].unk0 = 0x8000000;
        }
        D_800A0AF0[i].unk4 = 2;
        gPanelPhaseTable[i].unk4 = 2;
        gPanelChoiceTable[i].unk4 = 2;
        gBackActionRecord[i].unk4 = 2;
    }
}
