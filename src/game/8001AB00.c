#include "game/types.h"

s32 func_8001AB00(s32 *a, s32 *b)
{
    register u32 lo0 __asm__("$8");
    register u32 hi0 __asm__("$9");
    register s32 x __asm__("$3");
    register s32 y __asm__("$2");
    register s32 p0 __asm__("$3");
    register s32 p1 __asm__("$2");
    register s32 x1 __asm__("$6");
    register s32 x2 __asm__("$4");

    x = a[0];
    y = b[0];
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x), "r"(y));
    hi0 <<= 16;
    lo0 >>= 16;
    p0 = hi0 | lo0;
    x1 = a[1];
    y = b[1];
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x1), "r"(y));
    hi0 <<= 16;
    lo0 >>= 16;
    p1 = hi0 | lo0;
    p0 += p1;
    x2 = a[2];
    y = b[2];
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x2), "r"(y));
    hi0 <<= 16;
    lo0 >>= 16;
    p1 = hi0 | lo0;
    return p0 + p1;
}
