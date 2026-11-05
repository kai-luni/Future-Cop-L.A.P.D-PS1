#include "game/types.h"

struct FontEntry {
    u8 unk00[6];
    s16 height;
    s16 unk8;
};

extern s32 D_8009F8B8;
extern u8 *D_8009F878;
extern s16 D_8009F87C;
extern s8 *D_8009F468;
extern s8 D_8009F8C0[];
extern u8 D_800A6FE0[];
extern u8 D_800A3928[];
extern s32 D_8009F8F8[];
extern u8 D_8009F3F8[8];
extern s32 D_8009FA60[];

extern void func_8007AA9C(u8 *dst, u8 *src, s32 len, u8 *table);
extern void func_8007A9D0(u8 *src, u8 *dst, s32 len, u8 *table, s32 tableLen);
extern s32 func_80046C84(s32 id, s32 arg1);
extern struct FontEntry *func_80048E88(void);
extern void func_800492A0(u8 *text, s32 *width, s32 *height);
extern void func_80082EF0(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b);
extern void func_80049100(u8 *text, s32 x, s32 y, s32 color);

void func_8007ABBC(s32 action, s32 baseX, s32 baseY)
{
    u8 text[8];
    s32 width;
    s32 height;
    s32 fontUnk8;
    s32 fontHeight;

    s32 idx;
    u8 *chars;
    s32 i;
    register s32 *base __asm__("$4");
    register s32 off __asm__("$3");

    idx = D_8009F8B8;
    switch (action) {
    case 3: {
        s8 *flag = &D_8009F8C0[idx];
        if (*flag != 0) {
            func_8007AA9C(D_800A6FE0 + idx * 9, D_800A3928 + idx * 8, 8, D_8009F878);
            *flag = 0;
        } else {
            *flag = 1;
            D_8009F8F8[idx] = 0;
            func_8007A9D0(D_800A6FE0 + idx * 9, D_800A3928 + idx * 8, 8, D_8009F878, D_8009F87C);
        }
        break;
    }
    case 4:
        if (D_8009F8C0[idx] != 0) {
            u8 *p;
            func_80046C84(0x46, 0);
            i = D_8009F8F8[idx];
            chars = D_800A3928 + idx * 8;
            p = chars + i;
            if (*p != 0) {
                (*p)--;
            } else {
                *p = D_8009F87C - 1;
            }
        }
        break;
    case 5:
        if (D_8009F8C0[idx] != 0) {
            u8 *p;
            func_80046C84(0x46, 0);
            i = D_8009F8F8[idx];
            chars = D_800A3928 + idx * 8;
            p = chars + i;
            (*p)++;
            if (*p >= D_8009F87C) {
                *p = 0;
            }
        }
        break;
    case 1:
        if (D_8009F8C0[idx] != 0) {
            s32 *p = &D_8009F8F8[idx];
            (*p)++;
            if (*p >= 8) {
                *p = 0;
            }
            func_80046C84(0x47, 0);
        }
        break;
    case 2:
        if (D_8009F8C0[idx] != 0) {
            s32 *p = &D_8009F8F8[idx];
            (*p)--;
            if (*p < 0) {
                *p = 7;
            }
            func_80046C84(0x47, 0);
        }
        break;
    case 0: {
        s32 y = baseY + 10;
        if (D_8009F8C0[idx] != 0) {
            struct FontEntry *font;
            s32 step;
            s32 *sel_ptr;

            font = func_80048E88();
            fontUnk8 = font->unk8;
            step = fontUnk8;
            if (D_8009F468[5] == 1) {
                step++;
            }
            font = func_80048E88();
            chars = D_800A3928 + idx * 8;
            baseX -= step * 4;
            base = D_8009F8F8;
            off = idx << 2;
            fontHeight = font->height;
            __asm__("" : "+r"(off) : "r"(fontHeight));
            sel_ptr = (s32 *)((u8 *)base + off);
            text[1] = 0;
            for (i = 0; i < 8; i++) {
                s32 textColor;
                text[0] = D_8009F878[chars[i]];
                func_800492A0(text, &width, &height);
                if (i == *sel_ptr) {
                    register s32 boxX __asm__("$4");
                    register s32 boxKind __asm__("$7");
                    register u8 *pal __asm__("$3");
                    s32 c_idx;
                    s32 color;
                    boxX = baseX;
                    boxKind = 2;
                    pal = D_8009F3F8;
                    __asm__("" : "=r"(boxX), "=r"(boxKind), "=r"(pal) : "0"(boxX), "1"(boxKind), "2"(pal));
                    c_idx = (D_8009FA60[0] >> 4) & 7;
                    c_idx += (s32)pal;
                    color = *(u8 *)c_idx + 0x80;
                    func_80082EF0(boxX, y + fontHeight + 2, fontUnk8, boxKind, 10, color, color, color);
                    textColor = 8;
                } else {
                    textColor = 0xF;
                }
                func_80049100(text, baseX + (fontUnk8 - width) / 2, y, textColor);
                baseX += step;
            }
        } else {
            u8 *str = D_800A6FE0 + idx * 9;
            func_800492A0(str, &fontUnk8, &fontHeight);
            func_80049100(str, baseX - fontUnk8 / 2, y, 0xF);
        }
        break;
    }
    }
}
