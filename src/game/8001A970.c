#include "game/types.h"
#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

void func_8001A970(volatile s32 *m, volatile s32 *v, s32 *out)
{
    register s32 x __asm__("$12");
    register s32 y __asm__("$11");
    register s32 z __asm__("$10");
    register s32 a __asm__("$3");
    register s32 b __asm__("$7");
    register s32 b2 __asm__("$5");
    register s32 c __asm__("$2");
    x = v[0];
    c = m[0];
    FMUL(a, x, c);
    y = v[1];
    c = m[3];
    FMUL(b, y, c);
    z = v[2];
    c = m[6];
    FMUL(c, z, c);
    out[0] = a + b + c;
    c = m[1];
    FMUL(a, x, c);
    c = m[4];
    FMUL(b2, y, c);
    c = m[7];
    FMUL(c, z, c);
    out[1] = a + b2 + c;
    c = m[2];
    FMUL(a, x, c);
    c = m[5];
    FMUL(b2, y, c);
    c = m[8];
    FMUL(c, z, c);
    out[2] = a + b2 + c;
    out[0] += m[9];
    out[1] += m[10];
    out[2] += m[11];
}
