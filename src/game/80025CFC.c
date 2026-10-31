#include "game/types.h"

extern s16 D_8009F65C[];
extern void func_80047284(s32 id, s32 arg1, s32 arg2, s32 arg3);

register s32 zero __asm__("$5");

void func_80025CFC(u8 *arg0)
{
    s32 index;

    index = *(s8 *)(*(u8 **)(arg0 + 0xAC) + 0xC1);
    if (D_8009F65C[index] == 0) {
        D_8009F65C[index] = 1;
        if (*(u8 *)(*(u8 **)(arg0 + 0xAC) + 0xDE) == 0) {
            zero = 0;
            func_80047284(0x18, zero, zero, -1);
        }
    }
}
