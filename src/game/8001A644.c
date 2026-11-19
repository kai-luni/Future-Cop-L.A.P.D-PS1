// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

extern s32 gApproxVectorLength(s32 *vector) __asm__("func_80019F54");
extern s32 gGteLeadingZeros(u32 value) __asm__("func_80095C14");

/**
 * @brief Rescales a vector so its largest component fits the 16-bit range,
 *        returning the pre-scale magnitude word.
 *
 * @param vector Three s32 components, rescaled in place.
 * @return The result of func_80019F54 (the magnitude/max word).
 *
 * @par Confirmed
 * shift = 0x1F - LZC(result) (0 when result == 0). shift >= 0x10 shifts
 * the components right by shift - 0xF, otherwise left by 0xF - shift —
 * normalising the leading bit to position 15.
 *
 * @par Likely role
 * The range guard of the flyer tick func_800324E0 before handing vectors
 * to the s16-based transform/angle helpers: keeps large world coordinates
 * inside the fixed-point domain without losing the ratio.
 *
 * @par Unknown
 * Whether func_80019F54 returns the max component or a norm.
 *
 * @par Callers
 * func_800324E0.
 */
s32 func_8001A644(s32 *vector)
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
