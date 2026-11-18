// Categories: Graphics > Camera > Offset Placement | Utilities > Mathematics > Trigonometry
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

extern s32 gMissionWorkingSet __asm__("D_8009F9A0");
extern void gSinCos(s32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");

#define FMUL(a, b) ({ s32 r_; __asm__("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
#define UFMUL(a, b) ({ s32 r_; __asm__("nop\n\tnop\n\tmultu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
#define FMULR(a, b) ({ s32 r_; __asm__("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" : "=r"(r_) : "r"(a), "0"(b) : "$8", "$9"); r_; })

/**
 * @brief Places one point on a heading/pitch ray from a base position, with a
 *        scaled lateral offset taken from a data block.
 *
 * @param position Base position, three s32; its y is also folded into the
 *        distance.
 * @param heading Yaw that rotates the offset into world space (sin/cos from
 *        func_8001846C).
 * @param angle Second yaw applied to the final ray step.
 * @param pitch Pitch whose cotangent converts the remaining height into a
 *        horizontal step.
 * @param distance Distance along the ray; out[1] receives it after the half-height
 *        bias.
 * @param data Offset block, three s32 read as x/y/z.
 * @param out Output position, three s32.
 * @param arg7 Percentage shrink of the offset; it is halved first, so the factor
 *        is (0x100 - arg7 / 2) over 256.
 * @param arg8 Height bias of the offset, scaled by 21 in 16.16.
 * @return Nothing.
 *
 * @par Confirmed
 * The base position is copied, the heading sine/cosine pair is built, and the
 * offset is scaled by (0x100 - arg7 / 2) / 256 on all three axes with
 * (arg8 * 21 * 256) >> 16 subtracted from y. The offset's y then gains
 * position->y and distance gains position->y / 2. When D_8009F9A0 is non-zero
 * the offset is zeroed. After the pitch pair is built, the four sign
 * combinations select a signed or unsigned half-reciprocal,
 * UFMUL(absCos, 0x80000000 / (absSin >> 1)), which evaluates to the cotangent
 * in 16.16; it multiplies distance - offset.y and the result is made positive.
 * The offset is then rotated by heading as rx = cos * z - sin * x and
 * rz = sin * z + cos * x, and with the angle pair out[0] = rz + base.x -
 * FMUL(scale, sinA) and out[2] = (rx + base.z) - FMUL(scale, cosA) while
 * out[1] = distance.
 *
 * @par Likely role
 * The shared camera-anchor placement helper: func_80056634, func_8005678C,
 * func_800569EC and func_800573A4 all feed it a per-view offset block and a
 * player's own bias words, and read the three outputs as a world position for a
 * view object. The pitch cotangent is what turns "this far away" into "this far
 * along the ground at this eye height".
 *
 * @par Unknown
 * The physical meaning of the two bias words arg7 and arg8 (an FOV or distance
 * percentage and a vertical nudge) and why the global D_8009F9A0 zeroes the
 * offset.
 *
 * @par Callers
 * func_80056634, func_8005678C, func_800569EC (four sites) and func_800573A4.
 *
 * @par Environment path
 * Places one point on a heading/pitch ray from a base position: it scales the
 * lateral offset block by (0x100 - arg7/2)/256, folds the base height in,
 * converts the remaining height into a ground step with the pitch cotangent
 * (built from func_8001846C and a 16.16 half-reciprocal), rotates the offset
 * by the heading and writes the result to out[0..2]. Callers are the four
 * view placements func_80056634, func_8005678C, func_800569EC (four sites)
 * and func_800573A4, i.e. the code that positions the players' views before
 * the environment pass runs. It is one step upstream of gDrawViewLayers
 * rather than part of it: it produces the anchor the view object later uses,
 * and when D_8009F9A0 is set the lateral offset is silently zeroed.
 */
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
    gSinCos(heading, &L.sinH, &L.cosH);

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

    if (gMissionWorkingSet != 0) {
        L.vec.x = 0;
        L.vec.y = 0;
        L.vec.z = 0;
    }

    gSinCos(pitch, &L.sinP, &L.cosP);

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

    gSinCos(angle, &L.sinA, &L.cosA);

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
