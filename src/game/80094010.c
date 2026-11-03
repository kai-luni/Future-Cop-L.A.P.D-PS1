#include "game/types.h"

extern s16 D_8009DDF4;
extern s16 D_8009DDF6;

s32 func_80094010(s32 x, s32 y)
{
    register s32 cx __asm__("$2");
    register s32 py __asm__("$3");
    s32 px;
    u16 maxw;
    u16 maxh;

    if ((s16)x >= 0) {
        maxw = *(u16 *)&D_8009DDF4;
        if ((s16)x > D_8009DDF4 - 1) {
            cx = maxw - 1;
        } else {
            cx = x;
        }
    } else {
        cx = 0;
    }
    x = cx;

    if ((s16)y >= 0) {
        maxh = *(u16 *)&D_8009DDF6;
        if ((s16)y > D_8009DDF6 - 1) {
            y = maxh - 1;
        }
    } else {
        y = 0;
    }

    py = (y & 0x3FF) << 10;
    px = (x & 0x3FF) | 0xE4000000;
    return py | px;
}
