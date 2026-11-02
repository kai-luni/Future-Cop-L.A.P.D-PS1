#include "game/types.h"

s32 func_800564A8(s32 arg0)
{
    register u32 lo0 __asm__("$8");
    register u32 hi0 __asm__("$9");
    register s32 value __asm__("$2");
    register s32 r __asm__("$3");

    value = 0x10000 - (((arg0 - 0x14A) << 16) / 600);
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(value), "r"(value));
    hi0 <<= 16;
    lo0 >>= 16;
    value = 0x10000 - (hi0 | lo0);
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(0x20000), "r"(value));
    hi0 <<= 16;
    lo0 >>= 16;
    r = hi0 | lo0;
    r += 0xD800;
    if (r <= 0x8FFF) {
        r = 0x9000;
    }
    return r;
}
