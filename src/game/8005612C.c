#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Locals {
    struct Vec3s32 base;
    s32 pad0;
    struct Vec3s32 vec;
    s32 pad1;
    s32 sinH;
    s32 cosH;
    s32 pad2[2];
    s32 sinP;
    s32 cosP;
    s32 sinA;
    s32 cosA;
};

extern s32 D_8009F9A0;
extern void func_8001846C(s32 angle, s32 *sine, s32 *cosine);

#define FMUL(a, b) ({ s32 r_; __asm__("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
#define UFMUL(a, b) ({ s32 r_; __asm__("nop\n\tnop\n\tmultu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
#define FMULR(a, b) ({ s32 r_; __asm__("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" : "=r"(r_) : "r"(a), "0"(b) : "$8", "$9"); r_; })

void func_8005612C(struct Vec3s32 *position, s32 heading, s32 angle, s32 pitch,
                   s32 distance, u8 *data, s32 *out, s32 arg7, s32 arg8)
{
    struct Locals L;
    register s32 f __asm__("$5");
    register s32 c __asm__("$2");
    s32 rx;
    s32 rz;
    s32 scale;
    s32 absSin;
    s32 absCos;

    L.base = *position;
    func_8001846C(heading, &L.sinH, &L.cosH);

    L.vec = *(struct Vec3s32 *)data;

    arg7 = arg7 / 2;
    c = 0x100;
    f = c - arg7;
    L.vec.x = (L.vec.x * f) / 256;
    L.vec.y = (L.vec.y * f) / 256;
    L.vec.y -= (arg8 * 21 * 256) >> 16;
    L.vec.z = (L.vec.z * f) / 256;

    L.vec.y += position->y;
    distance += position->y / 2;

    if (D_8009F9A0 != 0) {
        L.vec.x = 0;
        L.vec.y = 0;
        L.vec.z = 0;
    }

    func_8001846C(pitch, &L.sinP, &L.cosP);

    absCos = L.cosP;
    if (absCos > 0) {
        absSin = L.sinP;
        if (absSin > 0) goto same_sign;
        absSin = -absSin;
        goto mixed_sign;
    }
    absSin = L.sinP;
    if (absSin <= 0) goto both_negative;
    absCos = -absCos;

mixed_sign:
    scale = -UFMUL(absCos, 0x80000000U / (u32)(absSin >> 1));
    goto scale_done;

both_negative:
    absCos = -absCos;
    absSin = -absSin;
same_sign:
    scale = UFMUL(absCos, 0x80000000U / (u32)(absSin >> 1));

scale_done:
    scale = FMUL(distance - L.vec.y, scale);
    if (scale < 0) {
        scale = -scale;
    }

    {
        register s32 ch __asm__("$11");
        register s32 vz __asm__("$10");
        register s32 sh __asm__("$3");
        register s32 vx __asm__("$7");

        ch = L.cosH;
        vz = L.vec.z;
        sh = L.sinH;
        vx = L.vec.x;
        rx = FMUL(ch, vz) - FMUL(sh, vx);
        rz = FMUL(sh, vz) + FMUL(ch, vx);
    }

    func_8001846C(angle, &L.sinA, &L.cosA);

    {
        register s32 b __asm__("$3");
        s32 ca, bz, prod;

        b = L.base.x;
        out[1] = distance;
        out[0] = rz + b - FMULR(scale, L.sinA);
        ca = L.cosA;
        bz = L.base.z;
        prod = FMULR(scale, ca);
        out[2] = (rx + bz) - prod;
    }
}
