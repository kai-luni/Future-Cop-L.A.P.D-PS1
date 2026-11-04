#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk8006D52_1C {
    u8 unk00[0x04];
    s32 unk04;
    u8 unk08[0x2C];
    s32 unk34;
    u8 unk38;
};

struct Unk8006D52_20 {
    u8 unk00[0x2E];
    u16 unk2E;
};

struct Unk8006D520 {
    u8 unk00[0x0E];
    s8 unk0E;
    u8 unk0F;
    u8 unk10;
    u8 unk11;
    s16 unk12;
    u8 unk14[0x06];
    u16 unk1A;
    struct Unk8006D52_1C *unk1C;
    struct Unk8006D52_20 *unk20;
    u8 unk24[0x10];
    u8 unk34[0x3C];
    s16 unk70;
    u8 unk72[0x26];
    u16 unk98;
    s16 unk9A;
};

extern s32 D_8009FA58;
extern s32 D_8009FA5C;

extern u8 *func_8003D9F8(s32 arg0, s32 arg1, s32 arg2, s32 arg3, void *arg4, void *arg5);
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);
extern void func_800413EC(s32 arg0, s32 arg1, s32 arg2, s32 context, s32 arg4, s32 mode);
extern void func_80041428(s32 arg0, s32 arg1, s32 arg2, s32 context, struct Vec3s32 *position,
                          s32 height, s32 arg6);

void func_8006D520(struct Unk8006D520 *arg0)
{
    struct Unk8006D520 *e = arg0;
    s32 sine;
    s32 cosine;
    u8 *base;

    if (e->unk98 & 0x400) {
        base = func_8003D9F8(e->unk1C != 0 ? e->unk1C->unk34 : 0,
                             e->unk1A, e->unk0E, 1, e->unk1C, e->unk34);
        if (e->unk98 & 0x200) {
            func_8001846C(0x100, &sine, &cosine);
        } else {
            func_8001846C(0x200, &sine, &cosine);
        }
        func_80041428((s32)(base + 0x24), e->unk20->unk2E,
                      (e->unk98 & 0x10 ? e->unk12 + e->unk70 * 2 : e->unk12 + e->unk70) << 4,
                      e->unk12 << 4,
                      (struct Vec3s32 *)(base + 0x18), cosine,
                      (e->unk1C != 0 && e->unk1C->unk38 == 0) ? e->unk1C->unk04 : 0);
    } else {
        register s32 a0 __asm__("$4");
        s32 color;

        a0 = (s32)func_8003D9F8(e->unk1C != 0 ? e->unk1C->unk34 : 0,
                                e->unk1A, e->unk0E, 0, e->unk1C, e->unk34);

        if (e->unk10 & 0x08) {
            a0 += 0x24;
            color = D_8009FA58;
        } else if (e->unk10 & 0x10) {
            a0 += 0x24;
            color = D_8009FA5C;
        } else {
            color = 0;
            if (e->unk1C != 0 && e->unk1C->unk38 == 0) {
                a0 += 0x24;
                color = e->unk1C->unk04;
            } else {
                a0 += 0x24;
            }
        }

        func_800413EC(a0, e->unk20->unk2E,
                      (e->unk98 & 0x10 ? e->unk12 + e->unk70 * 2 : e->unk12 + e->unk70) << 4,
                      e->unk9A << 4, color,
                      (e->unk98 & 0x40) ? 2 : 0);
    }

    e->unk9A = e->unk12;
}
