#include "game/types.h"

extern s32 *D_8009F590;
extern s32 D_8009F594;
extern s32 D_8009F598;
extern void *D_8009F9D0;
extern s32 D_8009F9E8;
extern void *D_8009FA4C;
extern s32 D_8009FA9C;
extern void *D_8009FAAC;

extern void *func_800194DC(s32 count, s32 size, s32 tag);

void func_80017C68(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5)
{
    s32 off = D_8009F9E8;
    s32 *ptr = D_8009F590;
    s32 base;

    D_8009F594 = 3;
    base = D_8009FA9C;
    D_8009F598 = 1;
    ptr[0] = base;
    ptr[1] = base;
    ptr[2] = base + off;
    ptr[3] = base + off;
    D_8009F9D0 = func_800194DC(a1, a0, 2);
    D_8009FA4C = func_800194DC(a5, a4, 2);
    D_8009FAAC = func_800194DC(a3, a2, 2);
}
