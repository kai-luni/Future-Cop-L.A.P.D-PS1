// Categories: Resources > Save Data | Platform > Memory Card
#include "game/types.h"

struct Ctx {
    s32 unk000;
    u8 unk004[0x8F0];
    s32 unk8F4;
    s32 unk8F8;
    s32 unk8FC;
    s32 unk900;
    s32 unk904;
    s32 unk908;
    s32 unk90C;
    s32 unk910;
    u8 unk914[0x80];
    s32 unk994;
    s32 unk998;
    s32 unk99C;
    s32 unk9A0;
    s32 unk9A4;
    s32 unk9A8;
    s32 unk9AC;
    u8 unk9B0[0x1200];
    s32 unk1BB0;
};

extern volatile struct Ctx *gOperationState __asm__("D_8009AB08");

extern void *gFillMemory(void *destination, int value, unsigned int count) __asm__("func_8001E4C0");
extern void gAcquireMcCallback(void) __asm__("func_80020530");
extern void gAcquireMcEvents(void) __asm__("func_80020120");
extern void func_8008D640(s32 value);
void gResetMcManager(void) __asm__("func_800206D0");


/**
 * @brief Resets the memory-card manager to idle.
 *
 * Zeroes the command and result words, the slot counters and the busy flag,
 * sets the state fields +0x9A0 and +0x9AC to 3, the result word +0x900 to -2,
 * clears the 0x80-byte name frame (+0x914) and the 0x1200-byte format image
 * (+0x9B0). Then it takes the driver lock (func_80020530), opens the eight BIOS
 * card events (func_80020120) and restores the channel selected in the first
 * word (func_8008D640).
 *
 * @par Globals
 * D_8009AB08 - the manager context; nothing else.
 *
 * @par Callers
 * func_8007FA88 (arming a profile transfer).
 *
 * @par Role and path
 * Resets the memory-card manager in gOperationState (D_8009AB08) to idle,
 * zeroing command/status fields and clearing the 0x80-byte name buffer and
 * 0x1200-byte format buffer via func_8001E4C0, then acquires the frame
 * callback via func_80020530, opens BIOS card events via func_80020120, and
 * resets the channel via func_8008D640. Called by func_8007FA88.
 */
void gResetMcManager(void)
{
    s32 t = gOperationState->unk000;

    gOperationState->unk8F4 = 0;
    gOperationState->unk8F8 = 0;
    gOperationState->unk910 = 0;
    gOperationState->unk8FC = t;
    gOperationState->unk900 = -2;
    gOperationState->unk904 = 0;
    gOperationState->unk908 = 1;
    gOperationState->unk90C = 0;
    gOperationState->unk994 = 0;
    gOperationState->unk998 = 0;
    gOperationState->unk9A0 = 3;
    gOperationState->unk9AC = 3;
    gOperationState->unk9A8 = 1;
    gOperationState->unk9A4 = 0;
    gOperationState->unk1BB0 = 0;

    gFillMemory(gOperationState->unk914, 0, 0x80);
    gFillMemory(gOperationState->unk9B0, 0, 0x1200);
    gAcquireMcCallback();
    gAcquireMcEvents();
    func_8008D640(gOperationState->unk8FC);
}
