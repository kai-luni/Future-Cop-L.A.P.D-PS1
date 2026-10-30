#include "game/types.h"

extern s32 *D_8009F590;
extern s32 D_8009F594;

s32 func_8001814C(s32 value)
{
    s32 high = D_8009F594 - 1;
    s32 *table = D_8009F590;
    s32 low = 0;
    s32 mid;

    while (high - low >= 2) {
        mid = (low + high) / 2;
        if (value < table[mid]) {
            high = mid;
        } else {
            low = mid;
        }
    }
    if (value >= table[high]) {
        return high;
    }
    return low;
}
