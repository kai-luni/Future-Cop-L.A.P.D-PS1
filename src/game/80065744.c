#include "game/types.h"

extern s32 D_8009FAA4;

s32 func_80065744(s32 arg0)
{
    s32 v;

    if (arg0 == 0) {
        return 0;
    }
    if (arg0 >= 0x21) {
        arg0 -= 0x20;
        v = (arg0 * D_8009FAA4) >> 10;
        return (v & 1) ? 0x1C : 0;
    }
    if (arg0 > 0) {
        v = (arg0 * D_8009FAA4) >> 6;
        return v % 28;
    }
    v = ((-arg0) * D_8009FAA4) >> 6;
    v = v % 28;
    return 27 - v;
}
