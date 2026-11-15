// Categories: Graphics > Effects > Shadow | Graphics > 3D Drawing
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

/**
 * @brief Draws an object's ground marker: a tilted 3D disc or a flat
 *        quad, coloured by the link state.
 *
 * @param e Marker owner; +0x98 the mode flags, +0x12 the width,
 *        +0x70 the width bonus, +0x9A the previous width, +0x1C the link.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8003D9F8 resolves the draw base from the link's unk34, unk1A and
 * unk0E. With flag 0x400: func_8001846C provides sine/cosine of 0x100
 * (flag 0x200) or 0x200, and func_80041428 draws the tilted marker at
 * base+0x24 with the width (unk12 + unk70, doubled by flag 0x10) << 4 and
 * the link colour when unk38 == 0. Otherwise func_800413EC draws flat
 * with colour D_8009FA58 (flag 0x08), D_8009FA5C (flag 0x10) or the link
 * colour, mode 2 when flag 0x40. Finally unk9A = unk12.
 *
 * @par Likely role
 * The shadow/target-ring renderer of the entity draw path (func_8006D770):
 * the two tilt angles select shadow steepness, the colour globals are the
 * team tints, and the width bonus animates the ring.
 *
 * @par Unknown
 * What mode 2 of func_800413EC changes.
 *
 * @par Callers
 * func_8006D770.
 */
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
