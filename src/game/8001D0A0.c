#include "game/types.h"

struct UnkD0A0 {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 flags;
    s32 f14;
};

extern struct UnkD0A0 D_800A6D78[];
extern u8 D_8009F614[];
extern u8 D_8009F615[];
extern u8 D_800A0798[];

extern void func_8001C494(void *a0, s32 a1);
extern s32 func_8008E1BC(s32 a0, void *a1, void *a2);

s32 func_8001D0A0(s32 index)
{
    register u8 *a0 __asm__("$4");
    register u8 *a1 __asm__("$5");
    register u8 *a2 __asm__("$6");
    s32 t = D_800A6D78[index].flags & 0xF;

    if (t == 1 || t == 0x11) {
        s32 r;

        a1 = (u8 *)D_800A6D78[index].f0C;
        a0 = D_8009F614;
        __asm__("" : "+r"(a0));
        func_8001C494(a0, a1);

        a0 = (u8 *)0x15;
        a1 = D_8009F615 - 1;
        a2 = D_800A0798;
        r = func_8008E1BC((s32)a0, a1, a2);
        if (r == 0) {
            return -4;
        }
    }
    return 0;
}
