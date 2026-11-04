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
extern void func_8007F128(s32 input0, s32 input1, s32 input2, s32 input3, s32 arg5, s32 arg6);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);
extern void func_800492A0(u8 *text, s32 *width, s32 *height);
extern struct FontEntry *func_80048E88(void);

void func_8007F1F8(s32 state, s32 input_baseX)
{
    u8 text[8];
    s32 width;
    s32 height;
    s32 i;
    s32 x;
    s32 y;
    s32 color;
    register s32 boxX __asm__("$4");
    register s32 boxY __asm__("$5");
    register s32 boxW __asm__("$6");
    register s32 boxKind __asm__("$7");
    register s32 *sel __asm__("$20");
    register s32 *base __asm__("$4");
    register s32 off __asm__("$3");
    s32 fontUnk8;
    register s32 fontHeight __asm__("$21");
    s32 hundred;
    u8 *chars;
    struct FontEntry *font;
    s32 idx;
    s32 baseX;

    static void *const label_table[] __attribute__((used)) = {
        &&state_0, &&state_1, &&state_2, &&state_3, &&state_4, &&state_5
    };

    baseX = input_baseX;
    idx = D_8009F8B8;
    if ((u32)state >= 6) {
        goto done;
    }
    goto *jtbl_800121A4[(u32)state];
state_0:
        if ((func_8007EA78() & 0xFF) == 0) {
            D_8009F92A = 0x3AAC;
            func_80046C84(0x4C, 0);
        } else {
            func_80046C84(0x4D, 0);
            func_80078740();
        }
        goto done;
state_1:
        D_8009F92A = 0;
        func_80046C84(0x46, 0);
        {
            u8 *p;
            i = D_8009F8F8;
            p = D_800A3948 + i;
            if (*p == 0) {
                *p = 0x19;
            } else {
                (*p)--;
            }
        }
        goto done;
state_2:
        D_8009F92A = 0;
        func_80046C84(0x46, 0);
        {
            u8 *p;
            i = D_8009F8F8;
            p = D_800A3948 + i;
            (*p)++;
            if (*p >= 0x1A) {
                *p = 0;
            }
        }
        goto done;
state_3:
        D_8009F92A = 0;
        D_8009F8F8++;
        if (D_8009F8F8 >= 0xA) {
            D_8009F8F8 = 0;
        }
        func_80046C84(0x47, 0);
        goto done;
state_4:
        D_8009F92A = 0;
        D_8009F8F8--;
        if (D_8009F8F8 < 0) {
            D_8009F8F8 = 9;
        }
        func_80046C84(0x47, 0);
        goto done;
state_5:
        if (D_8009F92A != 0) {
            func_800493C0(0x11, 0x1F, 0xA, 0xA);
            func_800788F0(0, 0xA0, 0x82, 0x11, D_8009F92A, 4, 8, 1);
        }
        if ((D_8009FA60[0] >> 9) & 1) {
            func_80078B34(0x6E5, 0, 0);
            func_80078B34(0x6E0, 1, 0);
        } else {
            func_80078B34(0x3AA2, 0, 0);
            func_80078B34(0x10CC, 1, 0);
        }
        func_800788F0(0, 0xA0, 0x3C, 0x10, 0x3A98, 4, 8, 1);
        func_800788F0(0, 0xA0, 0xA2, 0x10, 0x3AB6, 4, 8, 2);
        func_8007F128(0xA0, 0xAD, 0xF, 2, 4, 8);

        hundred = 100;
        font = func_80048E88();
        fontUnk8 = font->unk8;
        font = func_80048E88();
        fontHeight = font->height;
        chars = D_800A3948;
        x = baseX - 5 * (fontUnk8 + 1);
        text[1] = 0;
        {
            base = D_8009F8F8_arr;
            off = idx << 2;
            __asm__("" : "+r"(off) : "r"(fontHeight));
            sel = (s32 *)((u8 *)base + off);
            for (i = 0; i < 0xA; i++) {
                text[0] = *(u8 *)((u32)i + (u32)chars) + 0x41;
                func_800492A0(text, &width, &height);
                if (i == *sel) {
                    boxX = x;
                    boxY = hundred + fontHeight + 2;
                    boxW = fontUnk8;
                    boxKind = 2;
                    __asm__ volatile("" : "+r"(boxKind) : "r"(boxX), "r"(boxY), "r"(boxW));
                    color = D_8009F3F8[(D_8009FA60[0] >> 4) & 7] + 0x80;
                    func_80082EF0(boxX, boxY, boxW, boxKind, 10, color, color, color);
                    y = 8;
                } else {
                    y = 0xF;
                }
                func_80049100(text, x + (fontUnk8 - width) / 2, hundred, y);
                { s32 t = x + 1; x = t + fontUnk8; }
            }
        }
        goto done;
done:
    ;
}
