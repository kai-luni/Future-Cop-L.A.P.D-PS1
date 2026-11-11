#include "game/types.h"

typedef struct {
    s32 unk0;
    u8 *unk4;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u8 unkC;
    u8 unkD;
    u8 unkE;
    u8 unkF;
    u8 unk10;
    u8 unk11;
    u16 unk12;
} Unk8009C0A8;

typedef struct {
    u8 pad0[8];
    s16 unk8;
    s16 unkA;
    u8 padC[4];
    s16 unk10;
    u8 pad12[8];
    s16 unk1A;
} UnkBox;

typedef struct {
    u8 pad0[6];
    s16 unk6;
} UnkFont;

extern s32 D_8009FA60[];
extern u8 D_8009F3F8;
extern Unk8009C0A8 D_8009C0A8[];
extern s8 *D_8009F468;
extern s32 D_8009F8B8;

extern UnkBox *func_80082AB0(s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern UnkBox *func_80082B0C(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80082EF0(s32, s32, s32, s32, s32, s32, s32, s32);
extern void func_80083060(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
extern UnkFont *func_80048E88(void);
extern void func_800788F0(s32, s32, s32, s32, s32, s32, s32, s32);

void func_8007D7CC(u8 *p, s32 x, s32 arg2, s32 flag, s32 idx, s32 sel)
{
    s32 c0, c1, c2, c3;
    s32 c;
    s32 ci;
    s32 shade;
    u8 *pal;
    Unk8009C0A8 *e;
    register u32 base __asm__("$3");
    register UnkBox *box __asm__("$23");

    register s32 s1 __asm__("$17");
    register s32 s3 __asm__("$19");
    register s32 s6 __asm__("$22");
    register s32 s0 __asm__("$16");
    register s32 s2 __asm__("$18");
    register s32 s4 __asm__("$20");
    register s32 bx __asm__("$4");
    register s32 dx __asm__("$2");
    register s32 ay __asm__("$5");
    register s32 bw __asm__("$6");
    s32 ten;
    UnkFont *font;
    register s32 y __asm__("$21");

    y = arg2;
    ten = 10;

    if (flag) {
        c3 = 2;
        ci = D_8009FA60[0];
        __asm__("lui %0,%%hi(D_8009F3F8)\n\taddiu %0,%0,%%lo(D_8009F3F8)" : "=r"(pal));
        c0 = c1 = c2 = *(u8 *)(((ci >> 4) & 7) + (u32)pal) + 0x80;
    } else {
        c0 = 0x60;
        c1 = 0xC0;
        c2 = 0xFF;
        c3 = 1;
    }
    if (sel == 0) {
        c = p[2];
    } else {
        c = p[1];
    }
    base = (u32)D_8009C0A8;
    e = (Unk8009C0A8 *)((u32)(idx * 20) + base);
    __asm__ volatile("" : : "r"(e));
    shade = (e->unk4[D_8009F8B8] > e->unkA) ? 0x30 : 0x80;
    box = func_80082AB0(c, x, y, -1, -1, 0x14, shade, 1, 8);
    if (box != 0) {
        func_80082EF0(box->unk8 - c3, box->unkA - c3,
                      (box->unk10 - box->unk8) + c3 * 2,
                      (box->unk1A - box->unkA) + c3 * 2,
                      0xBB8, c0, c1, c2);
        if (e->unk4[D_8009F8B8] > e->unkA) {
            s1 = box->unk10;
            dx = box->unk8;
            s0 = box->unk1A;
            s1 -= dx;
            s1 = x + s1;
            s3 = s1 - 3;
            __asm__("" : "+r"(s0) : "r"(s3));
            s0 -= box->unkA;
            s0 = y + s0;
            s2 = s0 - 3;
            s6 = x + 3;
            s4 = 2;
            func_80083060(x, y + 3, s6, y, s3, s0, s1, s2, 0, 0xFF, 0, 0, s4);
            func_80083060(x, s2, s6, s0, s3, y, s1, y + 3, 0, 0xFF, 0, 0, s4);
        }
        if (flag && D_8009F468[5] != 2) {
            s2 = (s32)func_80082B0C(e->unkF, e->unk10, e->unk12, -1, -1, 0x14, c0, c1, c2, 1, 8);
            if (e->unk11) {
                func_80082B0C(e->unkF, e->unk11, e->unk12, -1, -1, 0x14, c0, c1, c2, 1, 8);
            }
            if (s2 != 0) {
                s0 = ((UnkBox *)s2)->unkA;
                dx = ((UnkBox *)s2)->unk1A;
                s1 = box->unkA;
                base = (u32)(s32)box->unk1A;
                bx = ((UnkBox *)s2)->unk10;
                s0 = s0 + ((dx - s0) >> 1);
                ay = s0;
                bw = 0x7E - bx;
                __asm__("" : "+r"(s1) : "r"(ay), "r"(bw));
                s1 = s1 + (((s32)base - s1) >> 1);
                func_80082EF0(bx, ay, bw, 1, ten, 0x60, 0xC0, 0xFF);
                bx = 0x7E;
                __asm__("" : "+r"(s0) : "r"(bx));
                s0 = s0 - s1;
                dx = s0 >> 31;
                ay = (dx & s0) + s1;
                bw = 1;
                __asm__("" : "+r"(s0) : "r"(ay), "r"(bw));
                s0 = (dx + s0) ^ dx;
                func_80082EF0(bx, ay, bw, s0 + 1, ten, 0x60, 0xC0, 0xFF);
                dx = box->unk8;
                __asm__("" : "+r"(dx) : "r"(s1));
                func_80082EF0(0x7E, s1, box->unk8 - 0x7E - c3, 1, ten, 0x60, 0xC0, 0xFF);
            }
        }
        y -= 1;
        x += 4 + (box->unk10 - box->unk8);
    }
    if (D_8009F468[5] != 2) {
        x = 0xC1;
        y += 2;
    }
    func_80082AB0(D_8009C0A8[idx].unkE, x + 1, y - 2, -1, -1, 0x14, 0x80, 1, 8);
    font = func_80048E88();
    func_800788F0(0, x, y + font->unk6, 0x10, *(u16 *)(p + 4), 1, 8, 2);
}
