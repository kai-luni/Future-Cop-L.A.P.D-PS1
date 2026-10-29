#include "game/types.h"

extern s32 func_8001A030(s32 arg0);
extern s32 func_80095900(s32 arg0);
extern void func_8001ABE0(u32 arg0, s32 arg1);

s32 func_8001A7AC(s32 arg0)
{
    s32 s0;

    s0 = func_80095900(func_8001A030(arg0)) << 2;
    if (s0 >= 2) {
        func_8001ABE0(0x80000000u / (u32)(s0 >> 1), arg0);
    }
    return s0;
}
