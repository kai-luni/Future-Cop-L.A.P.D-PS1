// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

union Product1AC28 { long long value; u32 word[2]; };

/**
 * @brief Scaled vector addition: out = a + b * scale, three components in
 *        16.16 fixed point.
 *
 * @param a Base vector (three s32).
 * @param b Direction vector (three s32).
 * @param scale 16.16 factor.
 * @param out Result vector.
 * @return Nothing.
 *
 * @par Confirmed
 * Per component the 64-bit product b[i] * scale is formed via mult, the
 * middle 32 bits ((mfhi << 16) | (mflo >> 16)) added to a[i].
 *
 * @par Likely role
 * The position integrator of the attack/mover handlers (func_8002E630,
 * func_8002EED8) and the camera rig: advances a position along a direction
 * by a frame-scaled step, the same arithmetic as func_8001ACA4.
 *
 * @par Callers
 * func_8002E630, func_8002EED8, func_80052550, func_800569EC, func_80057E40.
 */
void func_8001AC28(s32 *a, s32 *b, s32 scale, s32 *out)
{
    register union Product1AC28 wide __asm__("$8");
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
