#include "game/types.h"

extern s32 D_8009F554;
extern s32 D_8009F558;
extern s32 D_8009F55C;
extern s32 D_8009F560;
extern s32 D_8009F564;
extern s32 D_8009F568;

void func_80012E1C(s32 arg0)
{
    register s32 next __asm__("$2");

    next = arg0 + 0x400;
    D_8009F558 = next;
    next = arg0 + 0x800;
    D_8009F55C = next;
    next = arg0 + 0xC00;
    D_8009F554 = arg0;
    D_8009F560 = next;
    D_8009F564 = 0;
    D_8009F568 = 0;
}
