#include "game/types.h"

struct FontEntry {
    s16 f00;
    s16 f02;
    s16 f04;
    s16 f06;
    s16 f08;
    union {
        u16 u;
        s16 s;
    } f0A;
    s16 f0C;
    s16 f0E;
    s32 f10;
    s32 f14;
    s32 f18;
    u8 data[0x100];
};

struct FontSource {
    u8 pad00[0xA];
    u16 count;
    u8 pad0C[6];
    u8 f12;
    u8 f13;
    s32 off14;
    s32 pad18;
    s32 off1C;
};

struct FontInfo {
    u8 pad00[4];
    s16 f04;
};

extern struct FontEntry D_800A19A0[];
extern s32 D_8009F364;

extern s32 func_800489EC(unsigned char *value);
extern void func_80048BF8(s32 index);
extern s32 func_80048E28(s32 index);

void func_80048A14(s32 arg0, s32 *arg1)
{
    register struct FontSource *src __asm__("$7");
    struct FontEntry *e;
    struct FontInfo *info;
    u8 *p;
    register s32 i __asm__("$6");
    s32 positive;
    s32 off;
    u16 cnt;
    s32 off14;

    src = (struct FontSource *)arg0;
    if (D_8009F364 >= 4) {
        return;
    }

    e = &D_800A19A0[D_8009F364];
    off = src->off1C;
    e->f18 = (s32)arg1;
    e->f02 = (s16)(s8)src->f12;
    e->f04 = (s16)(s8)src->f13;
    e->f06 = 0;
    cnt = src->count;
    info = (struct FontInfo *)((u8 *)src + off);
    e->f14 = (s32)info;
    e->f0C = (s16)cnt;
    off14 = src->off14;
    e->f08 = 0;
    e->f0A.u = 0;
    e->f10 = (s32)((u8 *)src + off14);

    __asm__("" : : "r"(src) : "memory");
    p = (u8 *)e->f10;
    positive = e->f0C > 0;
    __asm__("" : "=r"(positive) : "0"(positive));
    if (positive) {
        i = 0;
        do {
            e->data[p[0]] = (u8)i;
            e->f0A.u = e->f0A.u + p[2];
            if ((s16)p[2] > e->f08) {
                __asm__("" : : : "memory");
            e->f08 = (s16)p[2];
            }
            if ((s16)p[3] > e->f06) {
                __asm__("" : : : "memory");
            e->f06 = (s16)p[3];
            }
            p += 11;
            i++;
        } while (i < e->f0C);
    }

    e->f0A.s = (s16)(e->f0A.s / e->f0C);
    e->f00 = (s16)func_800489EC((u8 *)info);
    e->f0E = (s16)((((info->f04 * func_800489EC((u8 *)info)) + 0x1F) & ~0x1F) >> 3);

    func_80048BF8(D_8009F364);
    D_8009F364++;
    if (D_8009F364 == 1) {
        func_80048E28(0);
    }
}
