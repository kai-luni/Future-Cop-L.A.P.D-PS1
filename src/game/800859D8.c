#include "game/types.h"

extern s32 D_8009F4C0;
extern s32 D_8009F4C4;
extern s32 D_8009F4C8;
extern s32 D_8009F4CC;

void func_800859D8(s32 arg0)
{
    if ((arg0 & 0xFF) != 0) {
        D_8009F4C0 = 1;
        D_8009F4C8 = 3;
        D_8009F4C4 = 1;
    } else {
        D_8009F4C0 = 2;
        D_8009F4C8 = 2;
        D_8009F4C4 = 0;
    }
    D_8009F4CC = 2;
}
