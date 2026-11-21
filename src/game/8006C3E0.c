// Categories: Gameplay > Target Selection | Gameplay > Player
#include "game/types.h"

struct Unk8006C3E0Other {
    u8 unk00[0x8];
    s32 unk08;
    u8 unk0C[0x18];
    u32 unk24;
};

struct Unk8006C3E0Inner {
    u8 unk00[0x68];
    s32 unk68;
    struct Unk8006C3E0Other *unk6C;
    u8 unk70[0x8];
    s32 unk78[3];
};

struct Unk8006C3E0Outer {
    u8 unk00[0xAC];
    struct Unk8006C3E0Inner *unkAC;
};

extern s32 gCheckTargetRangeAndCone(struct Unk8006C3E0Outer *arg0, struct Unk8006C3E0Other *arg1, s32 arg2, s32 arg3) __asm__("func_8004E5F0");
void gUpdatePlayerTargetLock(struct Unk8006C3E0Outer *arg0, s32 arg1) __asm__("func_8006C3E0");


/**
 * @brief Decrements weapon cooldown timers and validates the player's locked target.
 *
 * Reads arg0->unkAC (+0xAC), ticks down its three s32 timers in unk78[0..2] clamped
 * at zero, and clears the target lock unk6C if the target is flagged dead/inactive,
 * changed its unk08 ID, or fails the range-and-facing check.
 *
 * @param arg0 Player entity block whose +0xAC pointer holds Unk8006C3E0Inner.
 * @param arg1 Frame tick delta subtracted from each positive unk78[0..2] counter.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Loops i = 0..2 decrementing positive inner->unk78[i] by arg1 with zero clamp; then if inner->unk6C != 0, clears unk6C on (unk24 & 0x20001), unk08 != unk68, or func_8004E5F0 < 0.
 *
 * @par Calls
 * func_8004E5F0 - validates planar distance (< 0x18000) and facing cone (threshold 0xE804) against current target.
 *
 * @par Side effects
 * Updates inner->unk78[0..2] and may clear inner->unk6C.
 *
 * @par Callers
 * Confirmed: func_800588D4 (0x80058BB8, player entity tick).
 */
void gUpdatePlayerTargetLock(struct Unk8006C3E0Outer *arg0, s32 arg1)
{
    struct Unk8006C3E0Inner *inner;
    struct Unk8006C3E0Other *other;
    s32 i;
    s32 value;

    inner = arg0->unkAC;
    for (i = 0; i < 3; i++) {
        value = inner->unk78[i];
        if (value > 0) {
            value -= arg1;
            inner->unk78[i] = value;
            if (value < 0) {
                inner->unk78[i] = 0;
            }
        }
    }

    other = inner->unk6C;
    if (other != 0) {
        if ((other->unk24 & 0x20001) || other->unk08 != inner->unk68) {
            inner->unk6C = 0;
        } else if (gCheckTargetRangeAndCone(arg0, other, 0x18000, 0xE804) < 0) {
            inner->unk6C = 0;
        }
    }
}
