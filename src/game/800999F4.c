#include "game/types.h"

u16 func_800999F4(u16 a0, u16 a1, u16 a2, u16 a3)
{
    s32 start;
    s32 end;
    s32 diff;
    s32 ad;
    s32 q;
    s32 r;
    s32 sh;
    u16 m;
    u32 mm;
    u32 x;
    s32 prev;
    s32 cur;
    u32 ar;
    s32 steps;
    s32 frac;
    s32 i;
    u32 t;

    start = (a0 << 7) + a1;
    end = (a2 << 7) + a3;
    diff = end - start;

    ad = diff;
    if (diff < 0) {
        ad = -diff;
    }
    r = ad / 0x600;
    sh = r;
    r = ad - sh * 0x600;

    if (diff >= 0) {
        m = 0x1000 << sh;
    } else {
        if (r != 0) {
            sh++;
            r = 0x600 - r;
        }
        m = 0x1000 >> sh;
    }

    mm = m;
    prev = mm << 12;

    ar = __builtin_abs(r);
    steps = ar >> 5;
    frac = ar & 0x1F;
    x = 0x103B;
    cur = mm * x;

    for (i = 0; i < steps; i++) {
        prev = mm * x;
        x = 4155 * x;
        x >>= 12;
        cur = mm * x;
    }

    t = (prev + (((u32)(cur - prev) >> 5) * frac)) >> 12;
    if (t >= 0x4000) {
        t = 0x3FFF;
    }
    return t & 0xFFFF;
}
