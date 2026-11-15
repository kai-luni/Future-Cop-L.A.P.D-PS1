#include "game/types.h"

extern void func_800181D4(s32 value);
extern u32 D_8009F7B0;

void func_8004EFE4(s32 arg)
{
    if (arg != 0) {
        func_800181D4(arg);
    }
    D_8009F7B0 = 0;
}
