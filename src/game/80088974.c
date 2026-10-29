#include "game/types.h"

extern u8 *D_8009F970;
extern void func_80088770(s32, s32, s32);

void func_80088974(s32 arg0, s32 arg1, s32 arg2)
{
    s32 *base = (s32 *)D_8009F970;

    base[0x114] = arg0;
    base[0x115] = arg1;
    func_80088770(6, arg2, 0);
}
