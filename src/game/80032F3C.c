// Categories: Gameplay > Entity Script | AI > Evasion
#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk32F3C {
    u8 unk00[0x16];
    u8 unk16;
    u8 unk17[0x61];
    struct Vec3 unk78;
    u8 unk84[0x58];
    struct Vec3 unkDC;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    s32 unkF4;
    u8 unkF8[2];
    u8 unkFA;
};

extern u32 func_80019668(void);
extern void gVectorScaleInPlace(s32 arg0, void *arg1) __asm__("func_8001AB78");
extern s32 func_80032180(u32 arg0, void *arg1, s32 arg2, s32 arg3);
extern s32 gUpdateHoverEnemy(s32 arg0, void *arg1, s32 arg2) __asm__("func_800324E0");
extern s32 gEntityBaseHandler(u32 message, void *object, s32 arg2, s32 arg3) __asm__("func_8002AD58");
s32 gHandleTumblingFlyer(u32 arg0, struct Unk32F3C *obj, s32 arg2, s32 arg3) __asm__("func_80032F3C");


/**
 * @brief Message handler of a flyer variant: delegates spawn/tick and adds
 *        a randomised evade maneuver on message 4.
 *
 * @param arg0 Message number.
 * @param obj Flyer block; +0x16 the type byte, +0x78 the heading,
 *        +0xDC the evade velocity, +0xE8..+0xF0 angular rates, +0xFA state.
 * @param arg2, arg3 Forwarded payloads.
 * @return Case 1: 0x134. Case 4: 0. Other cases: the sub-handler results.
 *
 * @par Confirmed
 * Case 2 -> func_80032180 (spawn), case 6 -> func_800324E0 (tick). Case 4:
 * state 5 falls through to base, state 3 returns 0; otherwise type 0x25 or
 * a 10/16 random chance sets state 3, copies the heading to the evade
 * velocity, scales it by unkF4 (func_8001AB78) and draws three random
 * angular rates in -0x800..0x7FF. Type 0x25 without the chance sets state 5.
 *
 * @par Likely role
 * The evade response of the flyer class (behind func_800357C0): on a hit
 * or trigger the unit breaks off with a random spin, the two states gate
 * re-triggering.
 *
 * @par Unknown
 * What type 0x25 marks and what message 4 signals.
 *
 * @par Callers
 * func_800357C0.
 */
s32 gHandleTumblingFlyer(u32 arg0, struct Unk32F3C *obj, s32 arg2, s32 arg3)
{
    register s32 c25 __asm__("$18");
    register s32 c3 __asm__("$19");
    register s32 c5 __asm__("$21");

    switch (arg0) {
    case 1:
        return 0x134;
    case 2:
        return func_80032180(2, obj, arg2, arg3);
    case 6:
        return gUpdateHoverEnemy(6, obj, arg2);
    case 4:
        c5 = 5;
        if (obj->unkFA == c5) {
            goto dflt;
        }
        c3 = 3;
        if (obj->unkFA == c3) {
            goto ret0;
        }
        c25 = 0x25;
        if (obj->unk16 == c25 || (func_80019668() & 0xF) < 0xA) {
            obj->unkFA = c3;
            obj->unkDC = obj->unk78;
            gVectorScaleInPlace(obj->unkF4, &obj->unkDC);
            obj->unkE8 = (func_80019668() & 0xFFF) - 0x800;
            obj->unkEC = (func_80019668() & 0xFFF) - 0x800;
            obj->unkF0 = (func_80019668() & 0xFFF) - 0x800;
            goto ret0;
        }
        if (obj->unk16 == c25) {
            obj->unkFA = c5;
        ret0:
            return 0;
        }
        break;
    }
dflt:
    return gEntityBaseHandler(arg0, obj, arg2, arg3);
}
