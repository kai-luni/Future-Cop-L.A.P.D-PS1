#include "game/types.h"

typedef struct {
    u32 unk0;
    u32 unk4;
    u8 pad08[8];
    u8 buf0[0x3C00];
    u8 buf1[0x3C00];
} Ring;

extern volatile Ring *D_8009F4D8;
extern void func_8008731C(u8 *dst, u8 *src, u32 count);

void func_80087430(u8 *arg0)
{
    u32 magic;
    u32 s0;
    u32 s1;
    u32 s5;
    u32 used;
    u8 *s2;
    u8 *s4;

    magic = *(u32 *)(arg0 + 4);
    s0 = (((magic >> 24) | ((magic >> 8) & 0xFF00) |
           ((magic & 0xFF00) << 8) | (magic << 24)) - 0x10) >> 1;
    s0 = s0 / 15 * 15;
    s2 = arg0 + 0x10;
    s4 = s2 + s0;
    s5 = s0 / 15 * 16;

    do {
        used = D_8009F4D8->unk4 - 0x3C00;
    } while (D_8009F4D8->unk0 - used < s5);

    s1 = ((0x3C00 - D_8009F4D8->unk4 % 0x3C00) >> 4) * 15;

    if (s1 >= s0) {
        func_8008731C(D_8009F4D8->buf0 + D_8009F4D8->unk4 % 0x3C00, s2, s0);
        func_8008731C(D_8009F4D8->buf1 + D_8009F4D8->unk4 % 0x3C00, s4, s0);
    } else {
        func_8008731C(D_8009F4D8->buf0 + D_8009F4D8->unk4 % 0x3C00, s2, s1);
        func_8008731C(D_8009F4D8->buf0, s2 + s1, s0 - s1);
        func_8008731C(D_8009F4D8->buf1 + D_8009F4D8->unk4 % 0x3C00, s4, s1);
        func_8008731C(D_8009F4D8->buf1, s4 + s1, s0 - s1);
    }
    D_8009F4D8->unk4 += s5;
}
