#include "game/types.h"

struct State8009FA94 {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
};

extern s8 *D_8009F468;
extern s32 D_8009F8B8;
extern s8 D_8009F87E;
extern s8 D_8009F87F;
extern s8 D_8009F8C0;
extern s8 D_8009F8C0_arr[];
extern s32 D_8009F8C8;
extern s32 D_8009FA60;
extern struct State8009FA94 *D_8009FA94;
extern u8 D_8009F484[];
extern u8 D_8009B3D4[];
extern s8 D_800A3838[];

extern void func_8007B460(s32);
extern void func_800788F0(u8 *, s32, s32, s32, s32, s32, s32, s32);
extern void func_800493C0(u32, u8, u8, u8);
extern s32 func_8008CD30(u8 *, u8 *, s32);
extern void func_80078B34(s32, s32, s32);

void func_8007BCF4(s32 arg0)
{
    u8 text[8];
    s32 y;
    s32 i;
    s32 c;
    s32 a0;
    s32 s0;
    s32 v;
    s32 k;
    static void *const label_table[] __attribute__((used)) = {
        &&case_0, &&case_1, &&case_2, &&case_3, &&case_4, &&case_5
    };

    if (arg0 != 0) {
        return;
    }

    if (D_8009F468 != D_800A3838) {
        func_8007B460(D_8009F8B8);
    }

    { register s32 idx __asm__("$2") = D_8009F8B8; register s8 *base __asm__("$3"); __asm__("" : "+r"(idx)); base = D_8009F8C0_arr; __asm__("" : "+r"(base)); v = *(s8 *)(idx + (s32)base); }
    if (v == 2) {
        y = 0x52;
        func_800788F0(0, 0x57, 0x3B, 0x10, 0xC80, 4, 8, v);
        i = 0;
        if (D_8009F87F + 1 > 0) {
            do {
                if (i == D_8009F87E) {
                    u8 *p;
                    c = 0x11;
                    p = D_8009B3D4 + ((D_8009FA60 >> 4) & 7) * 3;
                    func_800493C0(c, p[0], p[1], p[2]);
                } else {
                    c = 0xF;
                }
                { register u8 *t __asm__("$4") = text; register u8 *src __asm__("$5") = D_8009F484; __asm__("" : "+r"(t)); __asm__("" : "+r"(src)); func_8008CD30(t, src, ++i); }
                func_800788F0(text, 0x57, y, c, 0, 4, 8, 2);
                y += 0xA;
            } while (i < D_8009F87F + 1);
        }
    }

    if (D_8009F8C8 != -1) {
        return;
    }

    { register s32 idx __asm__("$2") = D_8009F8B8; register s8 *base __asm__("$3"); __asm__("" : "+r"(idx)); base = D_8009F8C0_arr; __asm__("" : "+r"(base)); v = *(s8 *)(idx + (s32)base); }
    if (v == 2) {
        a0 = 0xC6C;
        s0 = 0xC62;
        goto common;
    }
    a0 = 0;
    s0 = 0x44C;
    k = *D_8009F468;
    if ((u32)k >= 6) {
        goto common;
    }
    goto *label_table[k];

case_0:
    if (D_8009F8C0 != 0) {
        a0 = 0xC26;
        s0 = 0xC1C;
    } else {
        a0 = 0xC4E;
    }
    goto common;
case_1:
    a0 = 0xC44;
    goto common;
case_2:
    a0 = 0xC56;
    goto common;
case_3:
    a0 = 0xC3A;
    if (D_8009FA94->unk5C == 1) {
        a0 = 0xC30;
        if (D_8009FA94->unk0C == 1) {
            a0 = 0xC3A;
        }
    }
    goto common;
case_4:
    a0 = 0xC53;
    goto common;
case_5:
    a0 = 0xC30;

common:
    if ((D_8009FA60 >> 9) & 1) {
        s0 = 0x10CC;
    }
    func_80078B34(a0, 0, 0);
    func_80078B34(s0, 1, 0);
}
