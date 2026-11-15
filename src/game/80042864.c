// Categories: Graphics > Effects > Particles | Game Engine > Frame
#include "game/types.h"

struct Unk42864 {
    u8 unk00[0x15];
    u8 total;
    u8 first;
    u8 second;
};

/**
 * @brief Drains a three-stage budget (second, first, total) by an amount,
 *        cascading the remainder to the next stage.
 *
 * @param object Budget record; +0x15 total, +0x16 first, +0x17 second.
 * @param amount Amount to drain.
 * @return Nothing.
 *
 * @par Confirmed
 * A zero total aborts. Each non-zero stage absorbs up to its value; the
 * remainder passes on (second -> first -> total), and the total clamps at
 * 0 instead of going negative.
 *
 * @par Likely role
 * The life/energy drain of the segment objects behind func_800437E8/
 * func_80043CA8: the three bytes are likely fade phases that consume in
 * order before the segment's main life decreases.
 *
 * @par Unknown
 * Which phase each byte represents.
 *
 * @par Callers
 * func_800437E8, func_80043CA8.
 */
void func_80042864(struct Unk42864 *object, s32 amount)
{
    if (object->total == 0) {
        return;
    }
    if (object->second != 0) {
        if (object->second >= amount) {
            object->second -= amount;
            return;
        }
        amount -= object->second;
        object->second = 0;
    }
    if (object->first != 0) {
        if (object->first >= amount) {
            object->first -= amount;
            return;
        }
        amount -= object->first;
        object->first = 0;
    }
    if (object->total != 0) {
        if (object->total < amount) {
            object->total = 0;
        } else {
            object->total -= amount;
        }
    }
}
