#include "game/types.h"

extern u32 func_8001BC44(u32 r, u32 g, u32 b);

u32 func_8001BD70(s16 *input)
{
    register s16 *p asm("$6") = input;
    u32 r, g, b;

    r = p[1];
    if (r >> 8) r = 255;
    g = p[3];
    if (g >> 8) g = 255;
    b = p[5];
    if (b >> 8) b = 255;
    return func_8001BC44((u8)r, (u8)g, (u8)b);
}
