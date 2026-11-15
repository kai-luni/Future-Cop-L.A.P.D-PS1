#include "game/types.h"

u32 func_80099968(s32 a0, u32 a1)
{
    s32 prev;
    s32 cur;
    u32 x;
    s32 steps;
    s32 frac;
    s32 i;

    prev = a0 << 12;
    steps = a1 >> 5;
    frac = a1 & 0x1F;
    x = 0x103B;
    cur = a0 * x;

    for (i = 0; i < steps; i++) {
        prev = a0 * x;
        x = 4155 * x;
        x >>= 12;
        cur = a0 * x;
    }

    return (prev + (((u32)(cur - prev) >> 5) * frac)) >> 12;
}
