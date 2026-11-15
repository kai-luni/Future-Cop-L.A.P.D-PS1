// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

/**
 * @brief Fast approximate distance between two points: max + (mid+min)/4.
 *
 * @param a, b Point triples (three s32 each).
 * @return The octagonal norm of the difference.
 *
 * @par Confirmed
 * Per-axis absolute differences are XOR-swap sorted so x = max; the result
 * is x + ((y + z) >> 2). The s32 sibling of func_8001AF08 (which works on
 * s16 and scales by 16).
 *
 * @par Likely role
 * The range gate of the combat code: the beam timer func_8005B3D4 and the
 * aim solver func_8005E2A0 compare against it, the base handler's hit path
 * func_8002AB8C uses it for reach checks — cheap enough for per-frame
 * distance tests without a square root.
 *
 * @par Callers
 * func_8002AB8C, func_80035B7C, func_80036AB4, func_8005B3D4, func_8005E2A0.
 */
s32 func_8001A4E8(s32 *a, s32 *b)
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
