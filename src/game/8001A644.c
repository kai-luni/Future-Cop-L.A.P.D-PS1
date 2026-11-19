// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

extern s32 gApproxVectorLength(s32 *vector) __asm__("func_80019F54");
extern s32 gGteLeadingZeros(u32 value) __asm__("func_80095C14");

s32 gScaleVector3ToBit15(s32 *vector) __asm__("func_8001A644");

/**
 * @brief Rescales a three-component vector relative to the leading bit of its
 * length estimate.
 *
 * @param vector Three signed components updated in place.
 * @return The length estimate before rescaling.
 *
 * @par Confirmed mechanics
 * Gets a length estimate from gApproxVectorLength. For nonzero input computes
 * 31 minus its GTE leading-zero count, then shifts the vector left or
 * arithmetically right so the estimate's leading bit would reach bit 15.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gUpdateHoverEnemy.
 *
 * @par Role and path
 * The flyer update uses the rescaled vector in its direction calculations.
 *
 * @par Unknown
 * The input domain determines whether arithmetic overflow or loss of precision
 * matters.
 */
s32 gScaleVector3ToBit15(s32 *vector)
{
    s32 result;
    s32 shift;

    result = gApproxVectorLength(vector);
    shift = result ? 0x1F - gGteLeadingZeros(result) : 0;
    if (shift >= 0x10) {
        s32 s, x, y, z;
        s = shift - 0xF;
        x = vector[0];
        y = vector[1];
        z = vector[2];
        vector[0] = x >> s;
        vector[1] = y >> s;
        vector[2] = z >> s;
    } else {
        s32 s, x, y, z;
        s = 0xF - shift;
        x = vector[0];
        y = vector[1];
        z = vector[2];
        vector[0] = x << s;
        vector[1] = y << s;
        vector[2] = z << s;
    }
    return result;
}
