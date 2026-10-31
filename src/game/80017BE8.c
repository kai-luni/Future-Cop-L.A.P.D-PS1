#include "game/types.h"

extern s32 D_8009F590;
extern s32 D_8009F59C;
extern s32 D_8009F5A0;
extern s32 D_8009F9E8;
extern s32 D_8009FA9C;

extern s32 func_8001E240(s32, s32);

void func_80017BE8(void)
{
    D_8009F59C = 0x203;
    D_8009FA9C = 0;
    D_8009F590 = 0;
    D_8009F5A0 = 0x182;

    D_8009F590 = func_8001E240(0x810, 3);
    D_8009F9E8 = 0x15A000;
    D_8009FA9C = func_8001E240(0x15A000, 3);

    while (D_8009FA9C == 0) {
        D_8009F9E8 -= 0x1000;
        D_8009FA9C = func_8001E240(D_8009F9E8, 3);
    }
}
