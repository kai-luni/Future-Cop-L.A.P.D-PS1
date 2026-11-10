#include "game/types.h"

struct E8007E1A4 {
    u8 unk00[4];
    s32 unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u16 unkC;
    u8 unk0E[6];
};

extern s8 *D_8009F468;
extern s32 D_8009F8B8;
extern s8 D_8009F8BC;
extern struct E8007E1A4 D_8009C0A8[];
extern u8 D_8009F490[];

extern s32 func_80046C84(s32 id, s32 arg1);
extern s32 func_80048E28(s32 index);
extern void func_800492A0(u8 *text, s32 *width, s32 *height);
extern u8 *func_80089914(s32 id);
extern void func_8008CD30(u8 *dest, u8 *fmt, ...);
extern void func_800788F0(u8 *text, s32 x, s32 y, s32 color, s32 textId,
                          s32 alignX, s32 alignY, s32 font);

void func_8007E1A4(s32 mode, s32 x, s32 y, s32 color, s32 idx)
{
    struct E8007E1A4 *e;
    struct E8007E1A4 *e2;
    u8 *base;
    u8 *p;
    u8 *s;
    s32 n;
    s32 old;
    s32 w;
    s32 h;
    s32 t;
    s32 b;
    s32 a;
    u8 buf[16];

    t = D_8009F468[D_8009F8B8];
    a = t - 1;
    b = idx - 1;
    if (D_8009F8B8 == 1) {
        a = t - 5;
        b = idx - 5;
    }
    base = (u8 *)D_8009C0A8;
    e = (struct E8007E1A4 *)(base + a * 20);
    p = (u8 *)e->unk4;
    switch (mode) {
    case 2:
        func_80046C84(0x46, 0);
        p[D_8009F8B8]--;
        if (((s8 *)p)[D_8009F8B8] < e->unk9) {
            p[D_8009F8B8] = e->unk8 - 1;
        }
        break;
    case 1:
        func_80046C84(0x46, 0);
        p[D_8009F8B8]++;
        if (((s8 *)p)[D_8009F8B8] >= e->unk8) {
            p[D_8009F8B8] = e->unk9;
        }
        break;
    case 0:
        e2 = &D_8009C0A8[b];
        n = e2->unkA - e2->unk9 + 1;
        if (n >= 2) {
            s = func_80089914(e2->unkC);
            if (s != 0) {
                old = -1;
                if (D_8009F8BC != 2) {
                    old = D_8009F8BC;
                    func_80048E28(2);
                }
                func_800492A0(s, &w, &h);
                func_8008CD30(buf, D_8009F490, n);
                func_800788F0(buf, x + w, y, color, 0, 1, 8, 2);
                if (old != -1) {
                    func_80048E28(old);
                }
            }
        }
        break;
    }
}
