// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

/**
 * @brief Ground-plane distance approximation: max(|dx|, |dz|) +
 *        min(|dx|, |dz|) / 2, ignoring y.
 *
 * @param a, b Point triples.
 * @return The octagonal norm in the x/z plane.
 *
 * @par Confirmed
 * sum = |dx| + |dz|, min extracted via the sign-mask trick, result =
 * sum - (min >> 1). No y term.
 *
 * @par Likely role
 * The horizontal range gate of the flyer tick func_800324E0, the spawn
 * setup func_80036E6C and the radial blast func_800410BC: ground distance
 * is what matters for targeting and blast falloff, the approximation
 * avoids a square root.
 *
 * @par Callers
 * func_800324E0, func_80036E6C, func_800410BC.
 */
s32 func_8001A750(s32 *a, s32 *b)
{
    s32 x;
    s32 z;
    s32 d;
    s32 min;
    s32 sum;

    x = a[0] - b[0];
    if (x < 0) {
        x = -x;
    }
    z = a[2] - b[2];
    if (z < 0) {
        z = -z;
    }
    sum = x + z;
    d = x - z;
    min = ((d >> 31) & d) + z;
    return sum - (min >> 1);
}
