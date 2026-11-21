// Categories: Gameplay > Spawning | Game Engine > Object Pool
#include "game/types.h"

extern void gHandleType5Entity(s32 arg0) __asm__("func_800382B4");
extern void gInitAttackSchedulerSlot(u8 *in, u8 *out) __asm__("func_80060668");
extern u8 *func_80039418(u8 *object, s32 arg1, s16 arg2, s32 arg3, u8 arg4, u8 arg5, s32 arg6);
s32 gInitType28Entity(s32 arg0, u8 *arg1, u8 *arg2) __asm__("func_800397AC");


/**
 * @brief Attaches up to four child objects to a parent from its spawn table.
 *
 * func_800382B4(2) runs first (mode selection for the spawn path). For each of
 * the four slots the presence word at base + 4 + offset + 0x20 is tested; when
 * it is non-zero func_80039418 creates the child with the parent's x/y
 * position, the slot index, the per-slot flags and scale bytes and the
 * presence word. The handle is stored at out + 0x158 and the created object is
 * marked with flag 0x10. From the second slot on, and only while bit 4 of the
 * per-slot byte is clear, the parent's 0xA0 block is also copied into the
 * child's 0x44 area. The slot counter is stored at arg1[0x73] and the 0xA0
 * word is cleared.
 *
 * @param arg0 Unused argument slot of the spawn call.
 * @param arg1 Parent object; flags at 0x4C, position 0x48/0x4A, payload 0xA0.
 * @param arg2 Spawn table; pointer at +4, object base at +8.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Counted loop over four slots with two conditional side calls.
 *
 * @par Calls
 * func_800382B4 - spawn mode setup. func_80039418 - child constructor.
 * func_80060668 - copies the parent's state block into the child.
 *
 * @par Side effects
 * Creates up to four objects and writes the parent's slot count.
 *
 * @par Callers
 * Likely: func_800398EC (sibling in the same block).
 */
s32 gInitType28Entity(s32 arg0, u8 *arg1, u8 *arg2)
{
    s32 i;
    register u8 *out __asm__("$19");
    register s32 offset __asm__("$18");
    u8 *base;
    u8 *obj;
    s32 step;
    u8 *result;

    gHandleType5Entity(2);
    i = 0;
    out = arg1;
    offset = i;
    base = arg2;
    obj = *(u8 **)(base + 8);
    step = 0x44;
    for (; i < 4; i++, step += 0x10) {
        if (*(u32 *)(*(u8 **)(base + 4) + offset + 0x20) != 0) {
            if (i > 0 && (obj[i + 0x4C] & 4) == 0) {
                gInitAttackSchedulerSlot(obj + step, arg1 + 0xA0);
            }
            result = func_80039418(arg1, *(s16 *)(obj + 0x48) << 8, *(s16 *)(obj + 0x4A),
                                   i, obj[i + 0x4C], obj[i + 0x50],
                                   *(u32 *)(*(u8 **)(base + 4) + offset + 0x20));
            *(u32 *)(out + 0x158) = (u32)result;
            *(u32 *)(result + 0x24) |= 0x10;
        } else {
            *(u32 *)(out + 0x158) = 0;
        }
        out += 4;
        offset += 8;
    }
    arg1[0x73] = i;
    *(u32 *)(arg1 + 0xA0) = 0;
    return 0;
}
