#include "game/types.h"

extern void func_800788F0(u8 *text, s32 x, s32 y, s32 color, s32 textId,
                           s32 alignX, s32 alignY, s32 font);
extern void func_80078B34(s32 arg0, s32 arg1, s32 arg2);

s32 func_8007A2C4(s32 arg0, s32 arg1)
{
    s32 ret;

    ret = arg0;
    if (arg1 == 0) {
        func_800788F0(0, 0xA0, 0x38, 0x10, 0x42E, 4, 8, 1);
        func_800788F0(0, 0xA0, 0x6C, 0x10, 0x438, 4, 8, 1);
        func_800788F0(0, 0xA0, 0xA0, 0x10, 0x442, 4, 8, 1);
        func_80078B34(0x456, 0, 0);
        func_80078B34(0x44C, 1, 0);
    }
    return ret;
}
