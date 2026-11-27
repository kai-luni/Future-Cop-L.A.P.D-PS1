// Categories: Gameplay > Entity Lifecycle | Game Engine > Object Pool
#include "game/types.h"

struct Unk3CA80 {
    u8 unk00[4];
    s16 unk04;
    u8 unk06;
    s8 unk07;
    u8 unk08[7];
    u8 flags;
    u8 unk10[4];
    s16 unk14;
    u8 unk16[0x0E];
    void (*callback)(s32 message, struct Unk3CA80 *object, s32 arg2, s32 arg3);
};
void gRetireSpawnSlot(struct Unk3CA80 *object) __asm__("func_8003CA80");


/**
 * @brief Sends teardown message 7 to an object's callback and marks its slot retired.
 *
 * Confirmed: invokes object->callback(7, object, 0, 0) (+0x24), resets unk07 (+0x07) and unk04 (+0x04) to -1, zeroes unk14 (+0x14) and unk06 (+0x06), and sets bit 0x02 in flags (+0x0F). Invoked during owner cleanup (func_8003B6A4, func_8003FB90) and timer teardown (func_8003CAE4).
 *
 * @param object Slot/entity record (+0x04, +0x06, +0x07, +0x0F flags, +0x14, +0x24 callback).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line indirect callback invocation followed by five field writes; no branches or loops.
 *
 * @par Calls
 * Indirect call object->callback(7, object, 0, 0) via pointer at +0x24.
 *
 * @par Side effects
 * Executes message 7 handler and writes unk07 = -1, unk14 = 0, unk06 = 0, unk04 = -1, and flags (+0x0F) |= 2.
 *
 * @par Callers
 * Confirmed: func_8003B6A4 (0x8003B738), func_8003CAE4 (0x8003CD4C, 0x8003CD80), func_8003FB90 (0x8003FC44) / Likely: none / Unknown: none.
 */
void gRetireSpawnSlot(struct Unk3CA80 *object)
{
    s32 zero;

    zero = 0;
    object->callback(7, object, zero, zero);
    object->unk07 = -1;
    object->unk14 = 0;
    object->unk06 = 0;
    object->unk04 = -1;
    object->flags |= 2;
}
