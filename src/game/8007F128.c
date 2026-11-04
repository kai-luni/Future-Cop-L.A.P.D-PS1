#include "game/types.h"

struct FontEntry {
    u8 unk00[6];
    s16 height;
    s16 unk8;
};

extern s32 D_8009F8B8;
extern s32 D_8009F8F8;
extern s32 D_8009F8F8_arr[];
extern u16 D_8009F92A;
extern u8 D_800A3948[];
extern u8 D_800A3938[];
extern u8 D_8009C1AC[];
extern u8 D_8009C1BC[];
extern u8 D_8009F3F8[8];
extern s32 D_8009FA60[];
extern void *jtbl_800121A4[];

extern s32 func_8007EA78(void);
extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_80078740(void);
extern void func_800493C0(u32 index, u8 blue, u8 green, u8 red);
extern void func_800788F0(u8 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7);
extern void func_80078B34(s32 arg0, s32 arg1, s32 arg2);
extern void func_80082EF0(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_800492A0(u8 *text, s32 *width, s32 *height);
extern struct FontEntry *func_80048E88(void);
extern void func_8007F128(s32 input0, s32 input1, s32 input2, s32 input3, s32 arg5, s32 arg6)
{
    u8 buf[12];
    s32 a0;
    s32 a1;
    s32 a2;
    u32 i;
    u8 *odd;
    u8 *even;
    u8 *dst;
    u8 *src;
    u8 *letter;

    s32 a3;

    a3 = input3;
    a0 = input0;
    a1 = input1;
    a2 = input2;
    i = 0;
    odd = D_8009C1BC;
    even = D_8009C1AC;
    dst = buf;
    src = D_800A3938;
    do {
        if (i & 1) {
            letter = (u8 *)(((*src + i) & 7) + (u32)odd);
        } else {
            letter = (u8 *)(((*src + i) & 0xF) + (u32)even);
        }
        *dst++ = *letter;
        __asm__ volatile("");
        i++;
        src++;
    } while (i < 10);
    buf[i] = 0;
    func_800788F0(buf, a0, a1, a2, 0, arg5, arg6, a3);
}
