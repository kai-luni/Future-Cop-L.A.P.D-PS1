#include "game/types.h"

extern volatile u32 D_800A6E18;
extern volatile u32 D_800A9448;

s32 func_8008C8B0(void)
{
    u32 count;

    count = *(volatile u16 *)0x1F801120 & 0xFFFF;

    if (count < D_800A6E18) {
        if (*(volatile u16 *)0x1F801128 != 0) {
            count += *(volatile u16 *)0x1F801128;
        } else {
            count += 0x10000;
        }
    }

    if (*(volatile u16 *)0x1F801124 & 0x200) {
        return (count - D_800A6E18) >= D_800A9448;
    } else {
        return ((count - D_800A6E18) >> 3) >= D_800A9448;
    }
}
