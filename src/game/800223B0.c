#include "game/types.h"

struct State { u8 pad[0x70]; s32 scale; };
extern struct State D_800A07A8;

void func_800223B0(u8 *a0, s32 a1, s32 a2, s32 *a3, s32 *a4)
{
    s32 values[2];
    s32 product = ((a2 & 0x7f) - 0x40) * a1;
    s32 pan = product >> 6;
    values[0] = a1 - (pan & (-pan >> 31));
    values[1] = a1 + (pan & (product >> 31));
    if (a2 & 0x80)
        values[0] = -values[0];
    if (a0) {
        values[0] = (values[0] * a0[1]) >> 7;
        values[1] = (values[1] * a0[2]) >> 7;
    }
    *a3 = values[0] * D_800A07A8.scale;
    *a4 = values[1] * D_800A07A8.scale;
}
