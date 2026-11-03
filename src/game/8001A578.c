#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

void func_8001A578(u32 angle, s32 *vec)
{
    s32 sine;
    s32 cosine;
    register s32 p0 __asm__("$3");
    register s32 b __asm__("$2");
    register s32 a __asm__("$4");
    register s32 x __asm__("$5");

    func_8001846C(angle, &sine, &cosine);

    x = vec[0];
    b = cosine;
    FMUL(p0, b, x);
    a = sine;
    b = vec[1];
    FMUL(b, a, b);
    p0 -= b;
    vec[0] = p0;

    b = sine;
    FMUL(p0, b, x);
    a = cosine;
    b = vec[1];
    FMUL(b, a, b);
    p0 += b;
    vec[1] = p0;
}
