#include "game/types.h"

extern s32 func_80090890(u8 *arg0, u8 *arg1, s32 arg2);

s32 func_800901F8(u8 *arg0, u8 *arg1)
{
    return func_80090890(arg0, arg1, 0xC) == 0;
}
