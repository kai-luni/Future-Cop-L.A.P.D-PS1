// Categories: Utilities > Mathematics > Vector | Utilities > Mathematics > Trigonometry
#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern void gSinCos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");

void gRotateVector2Q16(u32 angle, s32 *vec) __asm__("func_8001A578");

/**
 * @brief Rotates two fixed-point components in place by the specified angle.
 *
 * @param angle Angle in the 4096-step turn used by gSinCos.
 * @param vec Two writable signed 16.16 components.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls gSinCos to get sine and cosine words. Uses four signed products
 * shifted individually by 16 and stores x*cos - y*sin and x*sin + y*cos.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_800460D4, func_80053514.
 *
 * @par Role and path
 * The stereo pan and moving-platform paths share the two-component rotation
 * helper.
 *
 * @par Unknown
 * The coordinates' axes depend on the caller.
 */
void gRotateVector2Q16(u32 angle, s32 *vec)
{
    s32 sine;
    s32 cosine;
    register s32 p0 __asm__("$3");
    register s32 b __asm__("$2");
    register s32 a __asm__("$4");
    register s32 x __asm__("$5");

    gSinCos(angle, &sine, &cosine);

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
