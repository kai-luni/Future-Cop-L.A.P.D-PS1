// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

/**
 * @brief Fast approximate magnitude of an s16 vector: max*16 + (mid+min)*4.
 *
 * @param vector Three s16 components.
 * @return The octagonal norm approximation, scaled by 16.
 *
 * @par Confirmed
 * Absolute values are sorted with XOR swaps so a = max; the result is
 * (a << 4) + ((b + c) << 2). No square root, no tables.
 *
 * @par Likely role
 * The cheap length estimate of the polygon fracture func_8004B5F8 (fragment
 * sizing): the max + quarter-sum formula approximates the Euclidean norm
 * within a few percent, the << 4 keeps 16.16 scale.
 *
 * @par Callers
 * func_8004B5F8.
 */
s32 func_8001AF08(s16 *vector)
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
