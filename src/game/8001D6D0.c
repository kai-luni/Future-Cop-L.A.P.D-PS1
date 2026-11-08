#include "game/types.h"

struct UnkD6D0 {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 flags;
    s32 f14;
};

extern volatile struct UnkD6D0 D_800A6D78[];

extern s32 func_8001C478(void);
extern s32 func_8001D0A0(s32 index);

s32 func_8001D6D0(s32 arg0, s32 arg1, s32 arg2)
{
    s32 sel;
    s32 a1;
    register s32 a2 __asm__("$6");
    register s32 a0 __asm__("$4");
    register s32 lim __asm__("$3");
    s32 flags;
    register s32 test __asm__("$3");

    a1 = -9;
    if ((u32)arg0 < 4) {
        if (D_800A6D78[arg0].f00 != 0) {
            a1 = D_800A6D78[arg0].f00;
        }
    }
    if (a1 < 0) {
        return a1;
    }

    while ((D_800A6D78[arg0].f00 & 0xFF) == 2 ||
           (D_800A6D78[arg0].f00 & 0xFF) == 3) {
    }

    flags = D_800A6D78[arg0].flags;
    D_800A6D78[arg0].f00 = 2;

    test = flags & 0xF;
    __asm__("" : "+r"(test) : "r"(flags));
    if (test == 1) {
        a1 = func_8001C478();
        if (a1 < 0) {
            return a1;
        }
    }

    sel = arg2 & 7;
    a1 = D_800A6D78[arg0].f04;
    a2 = D_800A6D78[arg0].f0C;

    switch (sel) {
    case 3:
        return a2 - a1;
    default:
    case 0:
        a0 = a1 + arg1;
        break;
    case 1:
        a0 = a2 + arg1;
        break;
    case 2: {
        register s32 tmp __asm__("$2");
        tmp = a1 + D_800A6D78[arg0].f08;
        a0 = tmp + arg1;
        break;
    }
    }

    lim = D_800A6D78[arg0].f08;
    arg1 = a0 - a1;
    if (arg1 > lim || arg1 < 0) {
        if (arg1 < 0) {
            arg1 = 0;
        } else {
            arg1 = lim;
        }
        a0 = arg1 + a1;
    }
    D_800A6D78[arg0].f0C = a0;

    if (arg2 & 8) {
        s32 ret = func_8001D0A0(arg0);
        if (ret < 0) {
            return ret;
        }
    }

    {
        register s32 lim2 __asm__("$2");
        lim2 = D_800A6D78[arg0].f08;
        D_800A6D78[arg0].f00 = 1;
        if (arg1 >= lim2) {
            D_800A6D78[arg0].f00 |= 0x100;
        }
    }
    return arg1;
}
