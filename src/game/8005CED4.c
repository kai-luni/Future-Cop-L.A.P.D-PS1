#include "game/types.h"

extern void func_80010820(s32 *);
extern void func_8001006C(s32 *, s32 *, s32);
extern s32 func_8001AB00(s32 *, s32 *);

s32 func_8005CED4(u8 *a0, s32 *a1, u8 *a2)
{
    u8 *p;
    s32 vec[3];

    p = a0;
    do {
        if (p[0x71] & 1) {
            a1[0] = ((*(s32 *)(p + 0x58) >> 4) * *(s16 *)(a0 + 0x48)) >> 8;
            a1[1] = ((*(s32 *)(p + 0x5C) >> 4) * *(s16 *)(a0 + 0x48)) >> 8;
            a1[2] = ((*(s32 *)(p + 0x60) >> 4) * *(s16 *)(a0 + 0x48)) >> 8;
            func_80010820((s32 *)(a0 + 0x18));
            func_8001006C(a1, a1, 1);
            if (!(*(u8 *)(*(s32 *)a0 + 0x18) & 4)) {
                vec[0] = *(s32 *)(a2 + 0x50) - *(s32 *)(a0 + 0x3C);
                vec[1] = *(s32 *)(a2 + 0x54) - *(s32 *)(a0 + 0x40);
                vec[2] = *(s32 *)(a2 + 0x58) - *(s32 *)(a0 + 0x44);
                if (func_8001AB00(a1, vec) < 0) {
                    return 0;
                }
            }
            a1[0] = *(s32 *)(a0 + 0x3C) + a1[0];
            a1[1] = *(s32 *)(a0 + 0x40) + a1[1];
            a1[2] = *(s32 *)(a0 + 0x44) + a1[2];
            p[0x71] &= 0xFE;
            return 1;
        }
        p += 0x1C;
    } while ((s32)p < (s32)(a0 + 0x150));
    return 0;
}
