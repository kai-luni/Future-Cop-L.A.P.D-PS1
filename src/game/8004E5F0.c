// Categories: AI > Target Selection | Physics > Collision
#include "game/types.h"

typedef struct Entity {
    u8 unk00[0x16];
    u8 unk16;
    u8 unk17[0x39];
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    u8 unk60[0x18];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
    u8 unk88[0x18];
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
} Entity;

#define FMUL(a, b) ({ s32 r_; __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
#define FMULU(a, b) ({ s32 r_; __asm__("nop\n\tnop\n\tmultu %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })

extern s32 gSqrt12(s32 value) __asm__("func_80095900");
s32 gCheckTargetRangeAndCone(Entity *src, Entity *tgt, s32 maxDist, s32 threshold) __asm__("func_8004E5F0");


/**
 * @brief Range-and-facing score used by the AI target acquisition.
 *
 * Rejects a candidate outside the maxDist box in x/z, computes the fixed-point
 * planar distance (square root of the FMUL squares, shifted left 2), then
 * optionally tests whether the candidate lies in the seeker's facing cone by
 * normalizing the dot product of the seeker heading with the displacement.
 *
 * @param src Seeker entity; heading at +0x78/+0x80, or +0xA0/+0xA8 when the
 *        type byte +0x16 is 1; position at +0x50/+0x58.
 * @param tgt Candidate entity, position at +0x50/+0x58.
 * @param maxDist Half-width of the rejection box and exclusive distance cap.
 * @param threshold Facing threshold; 0xFFFF0000 disables the cone test.
 * @return The fixed-point distance when accepted, -1 when rejected.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Four early-out box tests, then the distance cap. The cone test is skipped
 * for threshold <= 0xFFFF0000 or dist < 9. Four sign branches compute the
 * signed projection num * 0x80000000 / (dist >> 1) via FMULU and compare it
 * against threshold; distances below 9 are accepted without a direction.
 *
 * @par Calls
 * func_80095900 - integer square root of dx*dx + dz*dz.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8004E80C (nearest and line-of-sight search, modes 1 and 3),
 * func_8004F330, func_80060238, func_8006C224, func_8006C3E0 (direct callers).
 */
s32 gCheckTargetRangeAndCone(Entity *src, Entity *tgt, s32 maxDist, s32 threshold)
{
    s32 dx;
    s32 dz;
    s32 len;
    s32 dist;
    s32 vx;
    register s32 vy __asm__("$3");
    s32 num;
    s32 den;
    s32 val;

    dx = tgt->unk50 - src->unk50;
    if (maxDist < dx) {
        return -1;
    }
    if (dx < -maxDist) {
        return -1;
    }
    dz = tgt->unk58 - src->unk58;
    if (maxDist < dz) {
        return -1;
    }
    if (dz < -maxDist) {
        return -1;
    }

    len = gSqrt12(FMUL(dx, dx) + FMUL(dz, dz));
    dist = len << 2;
    if (maxDist <= dist) {
        return -1;
    }
    if ((s32)0xFFFF0000 >= threshold) {
        return dist;
    }
    if (dist < 9) {
        return dist;
    }

    if (src->unk16 == 1) {
        vx = src->unkA0;
        vy = src->unkA8;
    } else {
        vx = src->unk78;
        vy = src->unk80;
    }
    num = FMUL(vx, dx) + FMUL(vy, dz);

    if (num > 0) {
        if (dist > 0) {
            den = dist >> 1;
            
#line 900
            val = FMULU(num, 0x80000000u / (u32)den);
        } else {
            den = -dist;
            den >>= 1;
            
#line 901
            val = -FMULU(num, 0x80000000u / (u32)den);
        }
    } else {
        if (dist > 0) {
            den = dist >> 1;
            num = -num;
            
#line 901
            val = -FMULU(num, 0x80000000u / (u32)den);
        } else {
            num = -num;
            den = -dist;
            den >>= 1;
            
#line 900
            val = FMULU(num, 0x80000000u / (u32)den);
        }
    }

    if (threshold <= val) {
        return dist;
    }
    return -1;
}
