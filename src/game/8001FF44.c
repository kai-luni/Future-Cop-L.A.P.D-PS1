// Categories: Platform > State
#include "game/types.h"

struct Ctx {
    u8 pad0[0x8F4];
    s32 unk8F4;
    s32 unk8F8;
    s32 unk8FC;
    s32 unk900;
    s32 unk904;
    u8 pad908[0x9A4 - 0x908];
    s32 unk9A4;
    s32 unk9A8;
    u8 pad9AC[0x1BB0 - 0x9AC];
    s32 unk1BB0;
};

extern volatile struct Ctx *gOperationState __asm__("D_8009AB08");
extern void func_8008D640(s32);
s32 gMcErrorCallbackB(void) __asm__("func_8001FF44");


/**
 *  @brief Updates a context state and passes its stored value to a helper.
 *
 *  The routine marks the context, then either decrements a positive counter
 *  in state zero or resets several state fields to their fallback values.
 *  @return Always 0.
 *  @par Globals
 *  D_8009AB08 points to the volatile context read and modified by this routine.
 *  @par Control flow
 *  State zero decrements a positive unk9A8; otherwise it resets the state
 *  fields and sets unk9A8 to 1. Other states reset the fields without
 *  changing unk9A8, then all paths call the helper.
 *  @par Calls
 *  func_8008D640 receives the context's unk8FC value.
 *  @par Side effects
 *  Writes context markers and, depending on state, the counter and reset fields.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Handles error or timeout cleanup in gOperationState (D_8009AB08) via a jump
 * table: clears unk1BB0, sets unk9A4 to 1, decrements unk9A8 when unk8F4 is 0
 * and unk9A8 > 0 or else sets unk900 to -3 and resets unk8F8/unk8F4/unk904,
 * and calls func_8008D640(unk8FC).
 */
s32 gMcErrorCallbackB(void)
{
    static void *const table[] = { &&case0, &&fallback, &&fallback, &&fallback, &&fallback, &&fallback, &&fallback };
    register s32 count __asm__("$2");
    register volatile struct Ctx *ctx __asm__("$3");
    register volatile struct Ctx *result __asm__("$2");
    gOperationState->unk1BB0 = 0;
    gOperationState->unk9A4 = 1;
    if ((u32)gOperationState->unk8F4 < 7)
        goto *table[gOperationState->unk8F4];
    goto fallback;
case0:
    ctx = gOperationState;
    if (ctx->unk9A8 > 0) {
        count = ctx->unk9A8 - 1;
    } else {
        __asm__ volatile("" ::: "memory");
        ctx->unk900 = -3;
        __asm__ volatile("" ::: "$2", "memory");
        ctx->unk8F8 = 0;
        ctx->unk8F4 = 0;
        ctx->unk904 = 0;
        count = 1;
    }
    ctx->unk9A8 = count;
    __asm__ volatile("" ::: "memory");
    result = gOperationState;
    goto done;
fallback:
    result = gOperationState;
    result->unk900 = -3;
    result->unk8F8 = 0;
    result->unk8F4 = 0;
    result->unk904 = 0;
done:
    func_8008D640(result->unk8FC);
    return 0;
}
