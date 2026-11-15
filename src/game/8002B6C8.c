// Categories: AI > Targeting | Gameplay > Entity Script
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk8002B6C8 {
    u8 unk00[0x5C];
    s32 x;
    s32 y;
    s32 z;
    u8 unk68[0xF4];
    s16 unk15C;
    u8 unk15E;
    u8 unk15F[5];
    u8 unk164;
};

extern s32 func_80095C30(s32 x, s32 z);
extern void func_8002B588(struct Unk8002B6C8 *object, s32 angle, s32 arg2);
extern void func_8002B0EC(struct Unk8002B6C8 *object, s32 arg1);

/**
 * @brief Turns an actor toward another object when the bearing error is
 *        large enough, otherwise syncs its animation state.
 *
 * @param object Actor; +0x5C the position, +0x15C the current heading,
 *        +0x15E the animation state, +0x164 the desired state.
 * @param other Target object (position at +0x5C).
 * @param arg2 Forwarded to the turn helper.
 * @return Nothing.
 *
 * @par Confirmed
 * delta = other - object; angle = func_80095C30(delta.x, delta.z) & 0xFFF
 * (the 12-bit ground bearing). When |angle - heading| > 0x40 the actor
 * turns via func_8002B588(object, angle, arg2); otherwise a mismatch
 * between +0x15E and +0x164 triggers func_8002B0EC(object, unk164).
 *
 * @par Likely role
 * The face-the-target step of the attack dispatcher func_8002B784: the
 * 0x40 dead zone (about 11 degrees) stops turn jitter, the animation sync
 * switches between idle and attack poses once aligned.
 *
 * @par Callers
 * func_8002B784.
 */
void func_8002B6C8(struct Unk8002B6C8 *object, struct Unk8002B6C8 *other, s32 arg2)
{
    struct Vec3s32 delta;
    s32 angle;
    s32 diff;

    delta.x = other->x - object->x;
    delta.y = other->y - object->y;
    delta.z = other->z - object->z;
    angle = func_80095C30(delta.x, delta.z) & 0xFFF;
    diff = angle - object->unk15C;
    if ((((diff >> 31) + diff) ^ (diff >> 31)) > 0x40) {
        func_8002B588(object, angle, arg2);
    } else if (object->unk15E != object->unk164) {
        func_8002B0EC(object, object->unk164);
    }
}
