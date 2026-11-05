#include "game/types.h"

extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

void func_8006C4C4(s16 *dst, s16 step, s16 count)
{
    s32 sine;
    s32 cosine;
    s32 s;
    s32 c;
    s16 total;
    u16 inc;
    s16 row;
    s16 col;
    s32 n;

    n = 0;
    if (count != 0) {
        inc = 0x800 / (0x400 / step);
        total = 0x800;
        dst[0] = 0;
        dst[1] = total;
        dst[2] = 0;
    } else {
        dst[0] = 0;
        dst[1] = 0x1000;
        dst[2] = 0;
    }

    n++;
    for (row = step; row < 0x401; row += step) {
        func_8001846C(row, &sine, &cosine);
        sine >>= 4;
        cosine >>= 4;
        if (count != 0) {
            for (col = 0; col < 0x201; col += step) {
                func_8001846C(col, &s, &c);
                s >>= 4;
                c >>= 4;
                dst[n * 4 + 0] = (s16)((sine * s) >> 12);
                dst[n * 4 + 1] = (s16)((total * cosine) >> 12);
                dst[n * 4 + 2] = (s16)((sine * c) >> 12);
                n++;
            }
            total += inc;
        } else {
            for (col = 0; col < 0x201; col += step) {
                func_8001846C(col, &s, &c);
                s >>= 4;
                c >>= 4;
                dst[n * 4 + 1] = (s16)((0x1000 * cosine) >> 12);
                dst[n * 4 + 0] = (s16)((sine * s) >> 12);
                dst[n * 4 + 2] = (s16)((sine * c) >> 12);
                n++;
            }
        }
    }
}
