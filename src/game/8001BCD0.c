#include "game/types.h"

void func_8001BCD0(u32 color, s32 scale, s32 *out)
{
    s32 r, g, b;

    r = ((color >> 16) & 0xff) * scale;
    g = ((color >> 8) & 0xff) * scale;
    b = (color & 0xff) * scale;
    out[0] += r;
    out[1] += g;
    out[2] += b;
}
