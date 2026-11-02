
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern s32 func_80095C14(u32 value);
extern s32 func_80019904(s32 value);

void func_8001A180(struct Vec3s32 *vector)
{
    s32 x;
    s32 y;
    s32 z;
    s32 ax;
    s32 ay;
    s32 az;
    s32 diff;
    register s32 shift __asm__("$4");
    s32 sum;
    s32 scale;
    s32 m0;
    register s32 p0 __asm__("$2");
    register s32 p1 __asm__("$3");

    x = vector->x;
    y = vector->y;
    z = vector->z;

    ax = x;
    ay = y;
    az = z;

    if (ax < 0) {
        ax = -ax;
    }
    if (ay < 0) {
        ay = -ay;
    }
    if (az < 0) {
        az = -az;
    }

    diff = ay - ax;
    ay -= (diff >> 31) & diff;

    diff = az - ay;
    ay = az - ((diff >> 31) & diff);

    if (ay != 0) {
        shift = func_80095C14((u32)ay);
        p1 = 0x1F;
        shift = p1 - shift;
    } else {
        shift = 0;
    }

    if (shift >= 0x15) {
        shift -= 0x14;
        x >>= shift;
        y >>= shift;
        z >>= shift;
    } else {
        shift = 0x14 - shift;
        x <<= shift;
        y <<= shift;
        z <<= shift;
    }

    FMUL(p0, x, x);
    FMUL(p1, y, y);
    FMUL(shift, z, z);
    sum = p0 + p1 + shift;

    if (sum > 0) {
        scale = func_80019904(sum);
        FMUL(m0, x, scale);
        vector->x = m0;
        FMUL(m0, y, scale);
        vector->y = m0;
        FMUL(m0, z, scale);
        vector->z = m0;
    }
}
