// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

extern int gGuidedProjectileUpdate();
extern int gSolveLeadDirection();

s32 gApproximateDistance3(s32 *a, s32 *b) __asm__("func_8001A4E8");

/**
 * @brief Estimates the distance between two three-component points.
 *
 * @param a First point with three signed components.
 * @param b Second point with three signed components.
 * @return Largest absolute delta plus the other two deltas summed and shifted
 *     right by two.
 *
 * @par Confirmed mechanics
 * Subtracts corresponding words and takes their magnitudes. Finds the largest
 * with two conditional swaps and adds one quarter of the sum of the other two.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80025D6C, func_8002AB8C,
 * gGuidedProjectileUpdate, func_80036AB4, func_8005B3D4, gSolveLeadDirection.
 *
 * @par Role and path
 * Gameplay callers use this inexpensive distance estimate for spatial tests.
 *
 * @par Unknown
 * The caller determines the world-unit scale and any acceptance threshold.
 */
s32 gApproximateDistance3(s32 *a, s32 *b)
{
    s32 x;
    s32 y;
    s32 z;

    x = a[0] - b[0];
    if (x < 0) {
        x = -x;
    }
    y = a[1] - b[1];
    if (y < 0) {
        y = -y;
    }
    z = a[2] - b[2];
    if (z < 0) {
        z = -z;
    }
    if (x < y) {
        x ^= y;
        y ^= x;
        x ^= y;
    }
    if (x < z) {
        x ^= z;
        z ^= x;
        x ^= z;
    }
    y += z;
    return x + (y >> 2);
}
