// Categories: Gameplay > Entity Update | Game Engine > Map Occupancy
#include "game/types.h"

struct Unk3BObject {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 unk21[3];
    u32 flags;
};

extern u8 *gSlotSpawnDescs[] __asm__("D_800A0C70");
extern s32 gQuarterIndex(s32 value, s32 range) __asm__("func_8003B65C");
void gFlagQuartileChange(struct Unk3BObject *object, s32 value) __asm__("func_8003BA50");


/**
 * @brief Flags an entity when its counter crosses a quartile boundary.
 *
 * Compares the quartile index of a previous counter value against object->unk1C
 * relative to object->unk1E and sets flag 0x4000 if they differ and a table entry exists.
 *
 * @param object Entity block carrying unk1C (+0x1C), unk1E (+0x1E), tableIndex (+0x20), flags (+0x24).
 * @param value Previous counter value before the caller's update.
 *
 * @par Globals
 * D_800A0C70 - table indexed by object->tableIndex; checked for a non-null entry.
 *
 * @par Control flow
 * Guards on object->unk1C >= 0, computes quartiles of value and unk1C via func_8003B65C,
 * and sets bit 0x4000 in object->flags when D_800A0C70[tableIndex] != 0 and quartiles differ.
 *
 * @par Calls
 * func_8003B65C - maps a value against object->unk1E into quartile index 0..3 (called twice).
 *
 * @par Side effects
 * May OR 0x4000 into object->flags.
 *
 * @par Callers
 * Confirmed: func_8002A940, func_8002AA1C, func_8002FC90, func_800324E0, func_8003526C, func_800395A8, func_800398EC, func_80058210, func_80058D78, func_80060A0C.
 *
 * @par Environment path
 * Sets flag 0x4000 in an entity's +0x24 word when the counter at +0x1C,
 * measured against the maximum at +0x1E, lies in a different quarter
 * (func_8003B65C returns 0..3 for <= 1/4, < 1/2, < 3/4, else) than the
 * previous value passed in, provided the counter is non-negative and the
 * entity's slot byte +0x20 has a spawn descriptor in D_800A0C70. The flag is
 * consumed by func_8004CE04 (8004CE04.c:59-61), which clears it and calls
 * func_8003BAE4 to spawn the slot's types for the new quarter. Callers are
 * the entity update and hit paths that change +0x1C (gUpdateHoverEnemy,
 * func_8002A940, func_8002AA1C, func_8002FC90, func_80058210, func_80060A0C
 * and others).
 */
void gFlagQuartileChange(struct Unk3BObject *object, s32 value)
{
    s32 previous;
    s32 current;

    if (object->unk1C >= 0) {
        previous = gQuarterIndex(value, object->unk1E);
        current = gQuarterIndex(object->unk1C, object->unk1E);
        if (gSlotSpawnDescs[object->tableIndex] != 0 && current != previous) {
            object->flags |= 0x4000;
        }
    }
}
