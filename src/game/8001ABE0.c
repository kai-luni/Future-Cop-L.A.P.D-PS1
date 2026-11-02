#include "game/types.h"

void func_8001ABE0(s32 scale, s32 *p)
{
    register u32 lo0 __asm__("$8");
    register u32 hi0 __asm__("$9");
    register s32 x __asm__("$2");
    register s32 r __asm__("$3");

    x = p[0];
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x), "r"(scale));
    hi0 <<= 16;
    lo0 >>= 16;
    r = hi0 | lo0;
    x = p[1];
    p[0] = r;
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x), "r"(scale));
    hi0 <<= 16;
    lo0 >>= 16;
    r = hi0 | lo0;
    p[1] = r;
}
