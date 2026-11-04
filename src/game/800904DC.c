#include "game/types.h"

extern u8 D_800A448C[];
extern s32 func_8008CCA0(s32 arg0, u8 *arg1);

s32 func_800904DC(s32 arg0, s32 arg1)
{
    s32 i;
    s32 off;
    s32 hi;
    u8 *ptr;

    i = 0;
    __asm__("" : "=r"(i) : "0"(i));
    ptr = D_800A448C;
    off = 0;

    for (;;) {
        s32 entry;

        hi = 0x800A0000;
        __asm__ volatile("" : "=r"(hi) : "0"(hi));
        entry = *(s32 *)(hi + off + 0x4484);
        if (entry == 0) {
            break;
        }
        if (entry == arg0) {
            if (func_8008CCA0(arg1, ptr) == 0) {
                return i + 1;
            }
        }
        ptr += 0x2C;
        i++;
        off += 0x2C;
        if (i >= 0x80) {
            break;
        }
    }

    return -1;
}
