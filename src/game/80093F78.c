#include "game/types.h"

extern u16 D_8009DDF4;
extern u16 D_8009DDF6;

s32 func_80093F78(s16 x, s16 y)
{
    register s32 v1 __asm__("$3");
    s32 x_tagged;

    x = (x < 0) ? 0 : (((s16)D_8009DDF4 - 1 < x) ? D_8009DDF4 - 1 : x);
    y = (y < 0) ? 0 : (((s16)D_8009DDF6 - 1 < y) ? D_8009DDF6 - 1 : y);

    v1 = (y & 0x3FF) << 10;
    x_tagged = (x & 0x3FF) | 0xE3000000;
    return v1 | x_tagged;
}
