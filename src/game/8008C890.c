#include "game/types.h"

extern u32 D_800A6E18;
extern u32 D_800A9448;

void func_8008C890(u32 a0)
{
    u32 count;

    count = *(volatile u16 *)0x1F801120;
    D_800A9448 = a0;
    D_800A6E18 = count;
}
