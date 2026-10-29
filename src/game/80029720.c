#include "game/types.h"

extern u8 D_8009F6B8;

void func_80029720(void)
{
    u8 state = D_8009F6B8;

    D_8009F6B8 = state & 0xEF;
    if ((state & 0x20) == 0) {
        D_8009F6B8 = (state & 0xEF) | 0x08;
    }
}
