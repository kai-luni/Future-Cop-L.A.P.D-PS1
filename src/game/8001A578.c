// Categories: Utilities > Mathematics > Vector | Utilities > Mathematics > Trigonometry
#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

/**
 * @brief Rotates a 2D fixed-point vector in place by a 12-bit angle.
 *
 * Obtains Q16 sine and cosine values from func_8001846C and applies a 2D rotation
 * matrix to vec[0] and vec[1] using fixed-point 16.16 FMUL multiplication.
 *
 * @param angle 12-bit rotation angle (0..4095 = full turn, masked by func_8001846C).
 * @param vec Two-element s32 vector [x, y] updated in place to (x*cos - y*sin, x*sin + y*cos).
 *
 * @par Globals
 * None directly; callee func_8001846C reads sine table D_8009A234.
 *
 * @par Control flow
 * Straight-line sequence: sin/cos lookup followed by four FMUL operations updating vec[0] and vec[1].
 *
 * @par Calls
 * func_8001846C - resolves Q16 sine and cosine from the 4096-step lookup table D_8009A234.
 *
 * @par Side effects
 * Overwrites vec[0] and vec[1] with rotated coordinates.
 *
 * @par Callers
 * Confirmed: func_800460D4 (0x80046100, 0x8004611C, stereo pan vector rotation), func_80053514 (0x800535F4, moving-platform offset rotation).
 */
void func_8001A578(u32 angle, s32 *vec)
{
    s32 sine;
    s32 cosine;
    register s32 p0 __asm__("$3");
    register s32 b __asm__("$2");
    register s32 a __asm__("$4");
    register s32 x __asm__("$5");

    func_8001846C(angle, &sine, &cosine);

    x = vec[0];
    b = cosine;
    FMUL(p0, b, x);
    a = sine;
    b = vec[1];
    FMUL(b, a, b);
    p0 -= b;
    vec[0] = p0;

    b = sine;
    FMUL(p0, b, x);
    a = cosine;
    b = vec[1];
    FMUL(b, a, b);
    p0 += b;
    vec[1] = p0;
}
