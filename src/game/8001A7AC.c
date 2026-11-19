// Categories: Utilities > Mathematics > Fixed Point
#include "game/types.h"

extern int gSolveLobVector();

extern s32 gSquaredLength2Q16(s32 arg0) __asm__("func_8001A030");
extern s32 gSqrt12(s32 arg0) __asm__("func_80095900");
extern void gScaleVector2Q16(u32 arg0, s32 arg1) __asm__("func_8001ABE0");

s32 gNormalizeVector2Q16(s32 arg0) __asm__("func_8001A7AC");

/**
 * @brief Returns the length of a two-component fixed-point vector and
 * normalizes it in place above the minimum length.
 *
 * @param arg0 Address of a two-word fixed-point vector.
 * @return Pre-normalization length estimate, equal to the square-root result
 *     shifted left by two.
 *
 * @par Confirmed mechanics
 * Calls gSquaredLength2Q16 and gSqrt12 and shifts the latter result left by
 * two. If it is at least 2, divides unsigned 0x80000000 by half that length
 * and passes the quotient and input address to gScaleVector2Q16.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80051EB8,
 * gTickHoverPhysics, func_80054590, func_800549F0, gSolveLobVector.
 *
 * @par Role and path
 * The motion and lead-direction paths use the resulting direction and original
 * length.
 *
 * @par Unknown
 * The reconstructed parameter is a word that the retail routine uses as an
 * address.
 */
s32 gNormalizeVector2Q16(s32 arg0)
{
    s32 s0;

    s0 = gSqrt12(gSquaredLength2Q16(arg0)) << 2;
    if (s0 >= 2) {
        gScaleVector2Q16(0x80000000u / (u32)(s0 >> 1), arg0);
    }
    return s0;
}
