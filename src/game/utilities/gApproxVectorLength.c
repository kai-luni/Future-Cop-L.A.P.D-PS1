// Categories: Utilities > Mathematics > Vector Length | Graphics > Culling
#include "game/types.h"
s32 gApproxVectorLength(s32 *vector) __asm__("func_80019F54");


/**
 * @brief Cheap 3D distance estimate: max + (sum of the others) / 4.
 *
 * @param vector Pointer to three 32-bit components.
 * @return An upper bound of the vector length, within a few percent of the true
 *         value - the octagonal ("taxicab with a bias") metric.
 *
 * @par Confirmed
 * The three components are made absolute, sorted with two conditional swaps
 * (a >= b >= c), then b + c are added and the result is a + ((b + c) >> 2).
 *
 * @par Compare
 * func_8001A100 computes the real length with two square-root steps; this one
 * is the cheap variant for range tests where a few percent do not matter.
 *
 * @par Callers
 * func_8001A644, func_8003E984, func_80070730 and func_80075A84.
 *
 * @par Environment path
 * Cheap 3D length estimate: the absolute components are sorted and the result
 * is max + (mid + min) / 4, with no multiply or root. Callers are
 * func_8001A644, func_8003E984, gComputeEffectEndPoint (func_80070730) and
 * gRunPolygonPass (func_80075A84), which use it for range checks. It is
 * shared distance math, not specific to the camera or level construction.
 */
s32 gApproxVectorLength(s32 *vector)
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
    return a + (b >> 2);
}
