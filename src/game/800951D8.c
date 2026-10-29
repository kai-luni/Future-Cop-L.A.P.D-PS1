#include "game/types.h"

extern s32 D_8009DEFC;
extern s32 D_8009DF00;
extern s32 func_80091270(s32 arg0);

void func_800951D8(void)
{
    s32 value;

    value = func_80091270(-1) + 0xF0;
    D_8009DEFC = value;
    D_8009DF00 = 0;
}
