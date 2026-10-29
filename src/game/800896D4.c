#include "game/types.h"

extern s32 D_8009F980;
extern s32 D_8009F984;
extern s32 D_8009F988;

s32 func_800896D4(s32 a0, s32 a1, s32 a2)
{
    s32 t;

    D_8009F980 = a0;
    a0 <<= 11;
    a0 += 0x57C;
    D_8009F984 = a1;
    a1 += a0;
    t = a2 * 56;
    D_8009F988 = a2;
    return a1 + t;
}
