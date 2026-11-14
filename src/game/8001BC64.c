#include "game/types.h"

void func_8001BC64(u32 color, s32 scale, s32 *out)
{
    s32 r, g, b;
    r = (s32)((color >> 16) & 255) * scale;
    g = (s32)((color >> 8) & 255) * scale;
    b = (s32)(color & 255) * scale;
    out[0] = r;
    out[1] = g;
    out[2] = b;
}
