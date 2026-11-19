// Categories: Platform > CD-ROM | Platform > Interrupts | Platform > DMA
#include "game/types.h"

struct State {
    s32 count;              /* 0x00 */
    void *buffer;           /* 0x04 */
    volatile s32 unk8;      /* 0x08 */
    s32 mode;               /* 0x0C */
    volatile s32 command;   /* 0x10 */
    volatile s32 unk14;     /* 0x14 */
    s32 unk18;              /* 0x18 */
    u32 time;               /* 0x1C */
    volatile s32 unk20;     /* 0x20 */
    s32 busy;               /* 0x24 */
    s32 callback;           /* 0x28 */
    volatile s32 callback2; /* 0x2C */
    volatile s32 callback3; /* 0x30 */
    volatile s32 unk34;     /* 0x34 */
};

extern struct State gCdAsyncReadState __asm__("D_8009CBD0");
extern s32 gCdUseDmaFlag __asm__("D_8009C814");
extern u32 gCdReadCallback __asm__("D_8009C810");

extern void gCdPauseCallback(void) __asm__("func_800908A0");
extern s32 gSetCdSyncCallback(s32) __asm__("func_8008E194");
extern s32 gSetCdReadyCallback(s32) __asm__("func_8008E1A8");
extern s32 gSetCdDmaCallback(s32) __asm__("func_8008E5D8");
extern s32 gCdControlF(s32, s32) __asm__("func_8008E2F8");
void gCdDmaCompleteCb(void) __asm__("func_80090B48");


/**
 * @brief CD DMA3 sector-transfer completion callback advancing the read state.
 *
 * Updates D_8009CBD0 for each completed sector (advancing buffer pointer unk8 by
 * command*4, decrementing remaining count unk14, incrementing sector cursor unk20),
 * and once unk14 reaches zero restores callbacks, pauses the drive, and notifies D_8009C810.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009CBD0 - CD read state; D_8009C814 - mode flag bit 0 selects DMA3 hook restore; D_8009C810 - user completion callback.
 *
 * @par Control flow
 * Updates unk8/unk14/unk20 and returns immediately if unk14 != 0; otherwise restores callbacks, issues pause command 9, sets busy = 1, and invokes D_8009C810(2, state->unk34).
 *
 * @par Calls
 * func_8008E1A8 - restores callback2; func_8008E5D8 - restores callback3 if D_8009C814 & 1; func_8008E194 - sets func_800908A0; func_8008E2F8 - sends CdlPause (cmd 9); indirect call to D_8009C810.
 *
 * @par Side effects
 * Mutates D_8009CBD0, restores CD/DMA callbacks, sets busy = 1, and invokes D_8009C810.
 *
 * @par Callers
 * Confirmed: no direct calls; address taken in func_800908D4 (0x80090960) and func_80090C18 (0x80090DB8) and registered as DMA3 callback via func_8008E5D8.
 *
 * @par Role and path
 * Serves as the CD-ROM DMA3 sector-transfer completion callback registered
 * via func_8008E5D8 in func_800908D4 and func_80090C18. For each sector it
 * advances D_8009CBD0.unk8 by command * 4, decrements unk14, and increments
 * unk20; when unk14 reaches zero, it restores callbacks via
 * func_8008E1A8/func_8008E5D8/func_8008E194, issues CdlPause via
 * func_8008E2F8(9, 0), sets busy = 1, and invokes D_8009C810(2,
 * state->unk34).
 */
void gCdDmaCompleteCb(void)
{
    register struct State *state __asm__("$16");

    state = &gCdAsyncReadState;
    state->unk8 += state->command * 4;
    state->unk14--;
    state->unk20++;
    if (state->unk14 != 0) {
        return;
    }
    gSetCdReadyCallback(state->callback2);
    if (gCdUseDmaFlag & 1) {
        gSetCdDmaCallback(state->callback3);
    }
    gSetCdSyncCallback((s32)gCdPauseCallback);
    gCdControlF(9, 0);
    state->busy = 1;
    if (gCdReadCallback != 0) {
        ((void (*)(s32, s32))gCdReadCallback)(2, state->unk34);
    }
}
