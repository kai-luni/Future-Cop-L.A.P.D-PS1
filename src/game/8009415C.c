#include "game/types.h"

__asm__(".macro move dst, src\n\taddu \\dst, \\src, $zero\n.endm\n");

extern u32 *D_8009DEE4;
extern u32 *D_8009DEE0;
extern u32 *D_8009DED8;
extern u32 *D_8009DEDC;

extern void func_800951D8(void);
extern s32 func_8009520C(void);

s32 func_8009415C(u8 *arg0, s32 arg1)
{
    *D_8009DEE4 |= 0x8000000;
    *D_8009DEE0 = 0;
    *D_8009DED8 = (u32 *)(arg0 + ((arg1 << 2) - 4));
    *D_8009DEDC = arg1;
    *D_8009DEE0 = 0x11000002;
    func_800951D8();
    if (*D_8009DEE0 & 0x1000000) {
        do {
            if (func_8009520C() != 0) {
                return -1;
            }
        } while (*D_8009DEE0 & 0x1000000);
    }
    return arg1;
}
