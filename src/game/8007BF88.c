#include "game/types.h"

extern s32 D_8009F8C8;
extern u8 *D_8009F8C4;
extern u8 D_80012094[];

void func_8007BF88(s32 arg0)
{
    if (arg0 == 3) {
        D_8009F8C8 = 5;
        D_8009F8C4 = D_80012094;
    }
}
