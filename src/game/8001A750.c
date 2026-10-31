#include "game/types.h"

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
