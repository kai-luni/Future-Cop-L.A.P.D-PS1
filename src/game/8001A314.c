#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

#define FMULN(d, a, b) __asm__("nop\n\tnop\n\tmult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern s32 func_80095900(s32 value);

s32 func_8001A314(s32 *vec)
{
    s32 x, y, z, ax, ay, az, d, m, sum, len, t, scale, r;
    s32 p0, p1, p2;

    x = vec[0];
    y = vec[1];
    z = vec[2];
    ax = x;
    ay = y;
    az = z;
    if (ax < 0) ax = -ax;
    if (ay < 0) ay = -ay;
    if (az < 0) az = -az;

    d = ay - ax;
    m = ay - ((d >> 31) & d);
    d = az - m;
    m = az - ((d >> 31) & d);

    if (m < 0x200) {
        x <<= 10; y <<= 10; z <<= 10;
    } else if (m > 0x800000) {
        x >>= 10; y >>= 10; z >>= 10;
    }

    FMUL(p0, x, x);
    FMUL(p1, y, y);
    FMUL(p2, z, z);
    len = func_80095900(p0 + p1 + p2);
    t = len << 2;
    if (t >= 2) {
        scale = (s32)(0x80000000U / (u32)(t >> 1));
        FMULN(r, x, scale);
        vec[0] = r;
        FMUL(r, y, scale);
        vec[1] = r;
        FMUL(r, z, scale);
        vec[2] = r;
    }
    if (m < 0x200) {
        t >>= 10;
    } else if (m > 0x800000) {
        t = len << 12;
    }
    return t;
}
