#include "game/types.h"

extern s8 D_8009F8C0[];
extern s8 *D_8009F468;
extern u8 *D_8009F92C;

void func_8008227C(void)
{
    *(u16 *)(D_8009F92C + 0x942) = 0x385E;
}

s32 D_8008228C(s32 arg0)
{
    s32 result;

    result = 0;
    if (D_8009F8C0[arg0] != 0) {
        result = (D_8009F468[arg0] < 10) << 1;
    }
    return result;
}
