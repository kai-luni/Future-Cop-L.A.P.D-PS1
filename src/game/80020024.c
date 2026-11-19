// Categories: Game Engine > State
#include "game/types.h"

struct Ctx {
    s32 unk000;
    u8 unk004[0x8C8];
    void (*unk8CC)(void);
    u8 unk8D0[0x24];
    s32 unk8F4;
    s32 unk8F8;
    s32 unk8FC;
    s32 unk900;
    s32 unk904;
    u8 unk908[0x9C];
    s32 unk9A4;
    u8 unk9A8[0x1208];
    u32 unk1BB0;
};

extern volatile struct Ctx *gOperationState __asm__("D_8009AB08");

extern void func_8008D640(s32 value);
void gMcWatchdogTick(void) __asm__("func_80020024");


/**
 *  @brief Advances a context counter and invokes its callback.
 *
 *  On reaching the counter threshold, resets several context fields and
 *  passes another field to a follow-up operation before the callback.
 *  @par Globals
 *  D_8009AB08 points to the volatile context containing the counter and callback.
 *  @par Control flow
 *  Tests the counter's pre-increment value against 0x3D; always invokes unk8CC.
 *  @par Calls
 *  func_8008D640 receives unk8FC on threshold; unk8CC is called indirectly.
 *  @par Side effects
 *  Increments or resets the counter and may reset other context fields.
 *  The direct and indirect calls may have additional effects.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Per-frame memory-card watchdog callback installed by func_80020530:
 * increments unk1BB0 in gOperationState (D_8009AB08) and, if it reaches 0x3D,
 * resets unk8F8/unk8F4/unk904, sets unk900 to -3 and unk9A4 to 1, and calls
 * func_8008D640(unk8FC), then always invokes the chained callback unk8CC.
 */
void gMcWatchdogTick(void)
{
    if (gOperationState->unk1BB0++ >= 0x3D) {
        gOperationState->unk1BB0 = 0;
        gOperationState->unk8F8 = 0;
        gOperationState->unk8F4 = 0;
        gOperationState->unk904 = 0;
        gOperationState->unk900 = -3;
        gOperationState->unk9A4 = 1;
        func_8008D640(gOperationState->unk8FC);
    }
    gOperationState->unk8CC();
}
