#include "game/types.h"

extern u32 *D_8009E9F4;
extern u8 D_80012BA8[];
extern void func_8009649C(void *);

s32 func_8009635C(void)
{
    volatile s32 count = 0x100000;

    if (*D_8009E9F4 & 0x20000000) {
        do {
            count = count - 1;
            if (count == -1) {
                register u8 *message __asm__("$4");
                __asm__ volatile("lui\t%0,%%hi(D_80012BA8)" : "=r"(message));
                func_8009649C(message + 0x2BA8);
                return -1;
            }
        } while (*D_8009E9F4 & 0x20000000);
    }
    return 0;
}
