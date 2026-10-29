#include "game/types.h"

extern s32 *D_8009F970;
extern void func_8008888C(s32, s32, s32);

void func_800889A8(s32 arg0, s32 arg1, s32 arg2)
{
    *(s32 *)((u8 *)D_8009F970 + 0x450) = arg0;
    *(s32 *)((u8 *)D_8009F970 + 0x454) = arg1;
    func_8008888C(6, arg2, 0);
}
