// Categories: Graphics > Sprite/Asset Subsystem | Resources > Memory Management
#include "game/types.h"

typedef struct {
    s8 unk00;
    s8 unk01;
    u8 unk02;
    u8 pad03;
    s32 unk04;
    s32 unk08;
    u8 pad0C[4];
} Unk800A0A00;

extern Unk800A0A00 gObjectSlots[8] __asm__("D_800A0A00");
extern s32 gCshdNodeSlots[2] __asm__("D_8009F640");
extern s32 gAssetHandle[3] __asm__("D_800A0A80");
extern s32 gStreamCommandState __asm__("D_8009F648");
extern s32 gSpuBufferPtr __asm__("D_8009F278");
extern s32 gVolatileBase __asm__("D_8009F27C");
extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern u8 gResourceGroupTableA[] __asm__("D_800A8E48");
extern u8 gSoundChannelStorage[] __asm__("D_800AB0E8");
extern u8 gFlushSoundChannelsIfCvkActive[] __asm__("D_80023314");

extern void gShutdownCvkSystem(void) __asm__("func_80023478");
extern void gResetCvkSubsystem(void) __asm__("func_80021B50");
extern s32 func_80021C20(s32 first, ...);
extern void gInitSpuTransferHook(void) __asm__("func_800211FC");
extern s32 gSetScreenModeHook(s32 callback) __asm__("func_8001F0FC");
extern s32 gSpuAllocAt(s32 arg0, s32 arg1) __asm__("func_80098458");
extern s32 gAllocSpuRamBlock(s32 arg0) __asm__("func_80097B28");
extern void gInitSoundState(void) __asm__("func_80044F8C");
u8 gInitCvkStreamSubsystem(void) __asm__("func_8002333C");


/**
 * @brief Brings up the sprite/asset subsystem: resource group, eight object
 *        slots and two work buffers.
 *
 * @return D_8009F26C - 1 when the two work buffers were obtained (the subsystem
 *         is running), 0 when the resource-group call failed and no buffers
 *         were taken.
 *
 * @par Confirmed flow
 * 1. func_80023478() tears down a previous instance if one is running.
 * 2. func_80021B50(), then func_80021C20(1, 0x1002001, 4, 8, 5, 0x18, 8,
 *    D_800A8E48, 9, D_800AB0E8, 0) - a varargs "declare resource group" call
 *    with two (id, table) pairs (4/8 and 5/0x18) and two tables, terminated by
 *    0. func_800211FC() finishes the declaration.
 * 3. func_8001F0FC(D_80023314) registers a per-frame callback.
 * 4. The eight slots D_800A0A00 are initialised: unk00/unk01 = -1 ("free"),
 *    unk02/unk04/unk08 = 0. D_8009F640[0..1] are cleared.
 * 5. Only when func_80021C20 returned 0: D_8009F27C = func_80098458(0x1010,
 *    0x11E80) and D_8009F278 = func_80097B28(0xC000) - two buffers, one of them
 *    0x11E80 bytes with a 0x1010-byte header area - and D_8009F26C = 1.
 * 6. D_800A0A80[0..2] and D_8009F648 are cleared, func_80044F8C() runs a final
 *    set-up step, and D_8009F26C is returned.
 *
 * @par Globals
 * D_800A0A00 (8 object slots), D_8009F640 (2 words), D_800A0A80 (3 words),
 * D_8009F26C (running flag), D_8009F278/D_8009F27C (work buffers),
 * D_8009F648, D_800A8E48/D_800AB0E8 (resource tables).
 *
 * @par Callers
 * func_80076EB0 (boot); func_80023478 undoes it.
 *
 * @par Environment path
 * Brings up the CVK sprite/asset subsystem by resetting tables via
 * func_80021B50, registering the frame callback via func_8001F0FC,
 * initializing audio via func_80044F8C, and invoking SPU/stream init helpers
 * func_800211FC, func_80021C20, func_80023478, func_80097B28, and
 * func_80098458.
 */
u8 gInitCvkStreamSubsystem(void)
{
    s32 result;
    u32 i;

    gShutdownCvkSystem();
    gResetCvkSubsystem();
    result = func_80021C20(1, 0x1002001, 4, 8, 5, 0x18, 8, gResourceGroupTableA, 9, gSoundChannelStorage, 0);
    gInitSpuTransferHook();
    gSetScreenModeHook((s32)gFlushSoundChannelsIfCvkActive);
    for (i = 0; i < 8; i++) {
        gObjectSlots[i].unk00 = -1;
        gObjectSlots[i].unk01 = -1;
        gObjectSlots[i].unk04 = 0;
        gObjectSlots[i].unk08 = 0;
        gObjectSlots[i].unk02 = 0;
    }
    i = 0;
    {
        register s32 *p __asm__("$3");
        p = gCshdNodeSlots;
        do {
            *p = 0;
            p++;
            i++;
        } while (i < 2);
    }
    if (result == 0) {
        gVolatileBase = gSpuAllocAt(0x1010, 0x11E80);
        gSpuBufferPtr = gAllocSpuRamBlock(0xC000);
        gCvkSystemActive = 1;
    }
    gAssetHandle[2] = 0;
    gAssetHandle[0] = 0;
    gAssetHandle[1] = 0;
    gStreamCommandState = 0;
    gInitSoundState();
    return gCvkSystemActive;
}
