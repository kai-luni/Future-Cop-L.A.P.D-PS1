#include "game/types.h"

extern u32 *D_8009E9F4;
extern u32 *D_8009E9C8;
extern volatile u32 *D_8009E9D4;
extern u32 D_8009E8B0[];
extern u32 D_8009E934[];
extern u8 D_80010000[];
extern void func_80096240(u32 *, u32);
extern void func_8008E000(void *, s32);

void func_80096150(s32 option)
{
    u32 *p;
    register u32 *q __asm__("$2");
    switch (option) {
    case 0:
        p = D_8009E9F4;
        __asm__ volatile ("" : : "r"(p));
        *p = 0x80000000;
        q = D_8009E9C8;
        __asm__ volatile ("" : : "r"(q) : "$4");
        *q = 0;
        *D_8009E9D4 = 0;
        *D_8009E9F4 = 0x60000000;
        func_80096240(D_8009E8B0, 0x20);
        func_80096240(D_8009E934, 0x20);
        break;
    case 1:
        *D_8009E9F4 = 0x80000000;
        *D_8009E9C8 = 0;
        *D_8009E9D4 = 0;
        (void)*D_8009E9D4;
        *D_8009E9F4 = 0x60000000;
        break;
    default:
        __asm__ volatile ("" : : : "$4");
        func_8008E000(D_80010000 + 0x2b8c, option);
        break;
    }
}
