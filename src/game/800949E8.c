#include "game/types.h"

extern u32 *D_8009DEC4;
extern u32 *D_8009DEC8;
extern void func_80094A3C(s32, s32, s32, s32);

u32 func_800949E8(u32 arg0)
{
    u32 value = arg0 | 0x10000000;

    *D_8009DEC8 = value;
    return *D_8009DEC4 & 0xFFFFFF;
}
