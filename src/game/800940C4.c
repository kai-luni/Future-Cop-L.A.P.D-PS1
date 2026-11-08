#include "game/types.h"

s32 func_800940C4(u8 *p)
{
    s32 v[4];
    s32 r;

    if (p == 0) {
        r = 0;
    } else {
        v[0] = p[0] >> 3;
        v[2] = (-*(s16 *)(p + 4) & 0xFF) >> 3;
        v[1] = p[2] >> 3;
        v[3] = (-*(s16 *)(p + 6) & 0xFF) >> 3;
        r = 0xE2000000 | (v[1] << 15) | (v[0] << 10) | (v[3] << 5) | v[2];
    }
    return r;
}
