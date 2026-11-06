#include "game/types.h"

union Product1AC28 { long long value; u32 word[2]; };

void func_8001AC28(s32 *a, s32 *b, s32 scale, s32 *out)
{
    register union Product1AC28 wide __asm__("$8");
    register u32 sum __asm__("$2");
    register u32 mul __asm__("$3");

    mul = b[0];
    sum = a[0];
    __asm__ volatile("mult %2, %3\n\tmflo %0\n\tmfhi %1"
                     : "=r"(wide.word[0]), "=r"(wide.word[1])
                     : "r"(mul), "r"(scale));
    wide.word[1] <<= 16;
    wide.word[0] >>= 16;
    mul = wide.word[1] | wide.word[0];
    out[0] = sum + mul;

    mul = b[1];
    sum = a[1];
    __asm__ volatile("mult %2, %3\n\tmflo %0\n\tmfhi %1"
                     : "=r"(wide.word[0]), "=r"(wide.word[1])
                     : "r"(mul), "r"(scale));
    wide.word[1] <<= 16;
    wide.word[0] >>= 16;
    mul = wide.word[1] | wide.word[0];
    out[1] = sum + mul;

    mul = b[2];
    sum = a[2];
    __asm__ volatile("mult %2, %3\n\tmflo %0\n\tmfhi %1"
                     : "=r"(wide.word[0]), "=r"(wide.word[1])
                     : "r"(mul), "r"(scale));
    wide.word[1] <<= 16;
    wide.word[0] >>= 16;
    mul = wide.word[1] | wide.word[0];
    out[2] = sum + mul;
}
