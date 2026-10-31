#include "game/types.h"

extern u32 *D_8009C7D0;

void func_8008CA30(u32 *arg0, u32 arg1)
{
    u32 *ptr;
    register u32 count __asm__("$2");

    if ((arg0 == 0) || (arg1 == 0)) {
        return;
    }

    while (arg1 & 7) {
        arg1--;
    }

    ptr = (u32 *)((u8 *)arg0 + arg1);
    ptr[-2] = (u32)arg0;
    ptr -= 2;
    ptr[1] = 0;
    arg0[0] = (u32)ptr;

    count = ((arg1 + 7) >> 3) - 1;
    D_8009C7D0 = arg0;
    arg0[1] = count;
}
