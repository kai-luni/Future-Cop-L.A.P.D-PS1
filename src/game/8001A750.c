// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

s32 gApproximateDistanceXzHalfMin(s32 *a, s32 *b) __asm__("func_8001A750");

/**
 * @brief Estimates horizontal distance by adding the larger absolute
 * displacement to half the smaller, rounded up.
 *
 * @param a First point with X at +0 and Z at +8.
 * @param b Second point with the same layout.
 * @return max(abs(dx), abs(dz)) + ceil(min(abs(dx), abs(dz)) / 2).
 *
 * @par Confirmed mechanics
 * Uses signed negation for negative deltas at +0 and +8. Selects the lesser
 * magnitude with the sign mask of their difference, shifts it right once, and
 * subtracts it from the sum.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gUpdateHoverEnemy,
 * gHandleProjectileMsg, func_800410BC, func_8006320C.
 *
 * @par Role and path
 * Several gameplay callers use this low-cost distance metric for horizontal
 * comparisons.
 *
 * @par Unknown
 * The routine does not establish the unit scale of X or Z.
 */
s32 gApproximateDistanceXzHalfMin(s32 *a, s32 *b)
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
