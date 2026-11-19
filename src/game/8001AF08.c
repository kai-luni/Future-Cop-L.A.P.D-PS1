// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

s32 gApproximateLengthS16Scaled16(s16 *vector) __asm__("func_8001AF08");

/**
 * @brief Estimates the length of three signed halfword components and scales
 * the approximation by 16.
 *
 * @param vector Pointer to three signed halfwords.
 * @return Sixteen times the weighted length estimate.
 *
 * @par Confirmed mechanics
 * Loads three signed halfwords and takes their absolute values. Selects the
 * maximum by swaps and returns (max << 4) + ((other two summed) << 2).
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gSpawnMeshFractureDebris.
 *
 * @par Role and path
 * The gameplay update at gSpawnMeshFractureDebris uses this short-vector
 * estimate.
 *
 * @par Unknown
 * The meaning of the approximation in the caller depends on the source vector.
 */
s32 gApproximateLengthS16Scaled16(s16 *vector)
{
    s32 a;
    s32 b;
    s32 c;

    a = vector[0];
    if (a < 0) {
        a = -a;
    }
    b = vector[1];
    if (b < 0) {
        b = -b;
    }
    c = vector[2];
    if (c < 0) {
        c = -c;
    }
    if (a < b) {
        a ^= b;
        b ^= a;
        a ^= b;
    }
    if (a < c) {
        a ^= c;
        c ^= a;
        a ^= c;
    }
    b += c;
    return (a << 4) + (b << 2);
}
