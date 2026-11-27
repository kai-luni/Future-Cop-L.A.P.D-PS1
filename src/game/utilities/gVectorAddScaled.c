// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

extern int gGuidedProjectileUpdate();

union Product1ACA4 { long long value; u32 word[2]; };

/**
 * @brief Scaled vector addition: out = a + b * scale in 16.16 fixed point
 *        (code-identical twin of func_8001AC28).
 *
 * @param a Base vector (three s32).
 * @param b Direction vector (three s32).
 * @param scale 16.16 factor.
 * @param out Result vector (may alias a).
 * @return Nothing.
 *
 * @par Confirmed
 * Per component the middle 32 bits of b[i] * scale ((mfhi << 16) |
 * (mflo >> 16)) are added to a[i]. The retail binary carries two copies
 * of this routine at different addresses.
 *
 * @par Likely role
 * The position integrator of the projectile and debris paths
 * (func_80036BD4, func_8004BEE8, func_800324E0): the aliasing-friendly
 * form advances positions in place each frame.
 *
 * @par Callers
 * func_800324E0, gGuidedProjectileUpdate, func_80036BD4, func_800371FC, func_8004AFC0,
 * func_8004BEE8.
 */
void gVectorAddScaled(s32 *a, s32 *b, s32 scale, s32 *out) __asm__("func_8001ACA4");

void gVectorAddScaled(s32 *a, s32 *b, s32 scale, s32 *out)
{
    register union Product1ACA4 wide __asm__("$8");
    register u32 sum __asm__("$2");
    register u32 mul __asm__("$3");

    mul = b[0];
    sum = a[0];
    __asm__ volatile("mult %2, %3\n\tmflo %0\n\tmfhi %1"
                     : "=r"(wide.word[0]), "=r"(wide.word[1])
                     : "r"(mul), "r"(scale));
    wide.word[1] <<= 16;
    wide.word[0] >>= 16;
    mul = wide.word[1] | wide.word[0];
    out[0] = sum + mul;

    mul = b[1];
    sum = a[1];
    __asm__ volatile("mult %2, %3\n\tmflo %0\n\tmfhi %1"
                     : "=r"(wide.word[0]), "=r"(wide.word[1])
                     : "r"(mul), "r"(scale));
    wide.word[1] <<= 16;
    wide.word[0] >>= 16;
    mul = wide.word[1] | wide.word[0];
    out[1] = sum + mul;

    mul = b[2];
    sum = a[2];
    __asm__ volatile("mult %2, %3\n\tmflo %0\n\tmfhi %1"
                     : "=r"(wide.word[0]), "=r"(wide.word[1])
                     : "r"(mul), "r"(scale));
    wide.word[1] <<= 16;
    wide.word[0] >>= 16;
    mul = wide.word[1] | wide.word[0];
    out[2] = sum + mul;
}
