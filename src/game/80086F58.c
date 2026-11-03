#include "game/types.h"
typedef struct { u8 unk00[0x7B74]; s32 unk7B74; } U;
extern U *D_8009F4D8;
extern s32 func_800966B0(u32 lo, u32 hi, s32 a, s32 b);

s32 func_80086F58(u32 arg0)
{
    if (D_8009F4D8->unk7B74 != 1) {
        return arg0 << 10;
    }
    {
        register u32 k __asm__("$2") = 0x271000;
        register u32 lo __asm__("$4");
        register u32 hi __asm__("$5");
        register s32 a2 __asm__("$6");
        register s32 a3 __asm__("$7");
        __asm__ volatile("multu\t%0,%1" : : "r"(arg0), "r"(k));
        __asm__ volatile("addiu\t%0,$0,0" : "=r"(a3));
        a2 = 0xBB5;
        __asm__ volatile("mfhi\t%0" : "=r"(hi) : "r"(a2), "r"(a3));
        __asm__ volatile("mflo\t%0" : "=r"(lo) : "r"(hi));
        return func_800966B0(lo, hi, a2, a3);
    }
}
