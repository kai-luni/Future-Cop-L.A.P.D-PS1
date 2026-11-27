// Categories: Utilities > Mathematics > Vector Length | Graphics > Culling
#include "game/types.h"

s32 gApproximateLength2(s32 *vector) __asm__("func_8001A074");

/**
 * @brief Approximates the length of a two-component vector using two piecewise
 * linear formulas.
 *
 * @param vector Two signed component words.
 * @return Approximate length in the input scale.
 *
 * @par Confirmed mechanics
 * Takes the component magnitudes and swaps them so the greater one comes
 * first. If the smaller exceeds half the larger, replaces the pair by (75
 * times larger + 106 times smaller) >> 7 and the difference. Returns the first
 * term plus (second >> 2) minus (second >> 6).
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gVectorLength3D,
 * gApproximateDistanceXzOctants, func_800458E8, func_80053730.
 *
 * @par Role and path
 * The two-component estimate supports engine distance and direction
 * calculations.
 *
 * @par Unknown
 * The routine contains no explicit error bound.
 */
s32 gApproximateLength2(s32 *vector)
{
    s32 a;
    s32 b;
    s32 sum;

    a = vector[0];
    if (a < 0) {
        a = -a;
    }
    b = vector[1];
    if (b < 0) {
        b = -b;
    }
    if (a < b) {
        a ^= b;
        b ^= a;
        a ^= b;
    }
    sum = a + b;
    if ((a >> 1) < b) {
        sum = (sum << 6) + (sum << 3) + (sum << 1) + a + (b << 5);
        b = a - b;
        a = sum >> 7;
    }
    return a + (b >> 2) - (b >> 6);
}
