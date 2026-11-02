#include "game/types.h"

extern s32 D_80010028;
extern void func_8008CB40(s32 *header);

void func_8001E2C0(s32 block)
{
    if (block != 0) {
        s32 size = ((s32 *)block)[-1];

        D_80010028 = D_80010028 - size - 4;
        func_8008CB40((s32 *)block - 1);
    }
}
