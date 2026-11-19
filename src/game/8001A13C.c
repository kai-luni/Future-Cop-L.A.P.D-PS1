// Categories: Utilities > Mathematics > Vector | Physics > Collision
#include "game/types.h"

extern int gGuidedProjectileUpdate();

extern s32 gApproximateLength2(s32 *vector) __asm__("func_8001A074");

s32 gApproximateDistanceXzOctants(s32 *a, s32 *b) __asm__("func_8001A13C");

/**
 * @brief Estimates the XZ distance between two points with the piecewise
 * length helper.
 *
 * @param a First point with X at +0 and Z at +8.
 * @param b Second point with the same layout.
 * @return Approximate length of the XZ displacement.
 *
 * @par Confirmed mechanics
 * Subtracts the words at +8 and +0 into two adjacent stack slots, forwards
 * their address to gApproximateLength2, and returns its result.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gGuidedProjectileUpdate,
 * func_800513D0.
 *
 * @par Role and path
 * Entity and resource paths use this distance estimate in the horizontal
 * plane.
 *
 * @par Unknown
 * The call sites determine how the estimate is used.
 */
s32 gApproximateDistanceXzOctants(s32 *a, s32 *b)
{
    s32 delta[2];

    delta[0] = a[2] - b[2];
    delta[1] = a[0] - b[0];
    return gApproximateLength2(delta);
}
