#include "game/types.h"

extern u8 D_800A2630[];
extern u8 D_800A2730[];
extern s32 D_8009FAA4;

extern s32 func_80018534(u32);

void func_800654F8(u32 input)
{
    register u32 arg __asm__("$4") = input;

    switch (arg >> 28) {
    case 0: {
        register u32 tmp __asm__("$2");
        register u32 n __asm__("$5");
        s32 step;
        u8 *out;
        s32 y;
        s32 mode;
        s32 shift;
        u32 p_tmp;
        s32 base;
        s32 phase;
        s32 mask;
        s32 x;

        tmp = arg >> 16;
        n = tmp & 15;
        out = D_800A2630;
        step = 0x100 << n;
        y = step * 15;
        mode = (arg >> 24) & 15;
        shift = ((arg >> 20) & 15) + 8;
        p_tmp = arg >> 8;
        base = D_8009FAA4 * (arg & 255);
        phase = p_tmp & 255;
        mask = (0x1000 << n) - 1;

        while (y >= 0) {
            s32 row = y;
            if (!mode) {
                row = 0;
            }
            for (x = step * 15; x >= 0; x -= step) {
                register s32 sum __asm__("$2") = x + row;
                register s32 a __asm__("$5") = base + sum;
                s32 value = 0;

                a &= mask;
                if (a < 0x1000) {
                    value = func_80018534(a) >> shift;
                }
                value += phase;
                value = value < 0 ? 0 : (value > 255 ? 255 : value);
                *out++ = value;
            }
            y -= step;
        }
        break;
    }
    case 1: {
        s32 base = (arg >> 8) & 0xFF;
        s32 sh = arg & 0xFF;
        u8 *dst;
        u8 *src;
        u8 *q;
        s32 cnt;

        base <<= sh;
        dst = D_800A2630;
        src = D_800A2730;
        q = src + 0x100;
        cnt = 0x40;

        while (cnt > 0) {
            s32 v0 = src[0];
            s32 v1 = src[1];
            s32 v2 = src[2];
            s32 p0 = q[0];
            s32 v3 = src[3];
            s32 p1 = q[1];
            s32 p2 = q[2];
            s32 p3 = q[3];

            if (p0 < v0) {
                v0 = p0 * 2 - v0;
            }
            if (p1 < v1) {
                v1 = p1 * 2 - v1;
            }
            if (p2 < v2) {
                v2 = p2 * 2 - v2;
            }
            if (p3 < v3) {
                v3 = p3 * 2 - v3;
            }

            dst[0] = (base + v0) >> sh;
            dst[1] = (base + v1) >> sh;
            dst[2] = (base + v2) >> sh;
            dst[3] = (base + v3) >> sh;

            src += 4;
            q += 4;
            dst += 4;
            cnt--;
        }
        break;
    }
    }
}
