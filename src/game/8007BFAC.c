#include "game/types.h"

extern s32 D_8009F8C8;
extern void *D_8009F8C4;
extern u8 D_80012088[];

void func_8007BFAC(s32 a0)
{
    if (a0 == 3) {
        D_8009F8C8 = a0;
        D_8009F8C4 = D_80012088;
    }
}
