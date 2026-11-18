// Categories: Utilities > Mathematics > Ballistic Trajectory | Physics > Motion > Projectile | Graphics > GTE
#include "game/types.h"

extern int gSpawnLobShot();
extern int gWeaponFireDispatch();

extern s32 func_8001A7AC(s32 *vec);
extern void func_8001ABE0(s32 scale, s32 *p);
extern s32 func_80095900(s32 value);
extern s32 func_80095C30(s32 x, s32 y);
extern void gSinCos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");

#define MULU16(out, a, b) \
    __asm__ volatile("multu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")
#define MUL16(out, a, b) \
    __asm__ volatile("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

/**
 * @brief Builds a launch vector from two positions and a speed
 *        parameter: the planar direction is normalised, an elevation is
 *        solved with reciprocal divisions and the table arctangent.
 *
 * @param angle Speed parameter; multiplied by 300 to give the magnitude
 *        of the returned vector.
 * @param out Output triple: out[0] horizontal x, out[1] vertical,
 *        out[2] horizontal z.
 * @param src Source position, read only (x +0x00, y +0x04, z +0x08).
 * @param dst Target position, read only, same layout.
 * @param scale Geometry scale: it multiplies the planar distance before
 *        the elevation is solved.
 * @return Nothing.
 *
 * @par Confirmed
 * Planar delta: vec[0] = dst[0] - src[0], vec[1] = dst[2] - src[2];
 * dy = dst[1] - src[1] is kept apart. func_8001A7AC(vec) returns the
 * planar length and normalises vec in place, and a length below 2 is
 * raised to 2. s1 = angle * 300.
 * Every quotient uses the reciprocal form MULU16(a, 0x80000000 / b), i.e.
 * a * b^-1 with the middle 32 bits of the 64-bit product, so each result
 * carries the same 2^15 factor:
 * t = scale * len, q = (s1 * s1) / 2, d = t / q, s0 = d / 2, and
 * r = dy / (len / 2); the code keeps the four sign combinations of dy
 * and len in separate branches and forms v = s0 + r or s0 - r.
 * q2 = s0 * v, then a0 = 0x10000 - 4 * q2 clamped at 0, otherwise
 * func_80095900(a0) * 4 (the reciprocal table), and a0 += 0x10000. With
 * v2 = 2 * s0 the same reciprocal form produces a0 again, sign-mirrored
 * through the negative and positive branches.
 * phi = func_80095C30(a0, 0x10000) is the 12-bit arctangent;
 * func_8001846C((phi << 12) >> 12, &vec[2], &vec[3]) yields sine and
 * cosine, and the cosine is forced positive. func_8001ABE0(cos * s1,
 * vec) rescales the planar unit vector. Finally out[0] = vec[0],
 * out[1] = sin * s1 and out[2] = vec[1]; nothing else is written and
 * the inputs are never modified.
 *
 * @par Likely role
 * The lobbed-shot solver of the attack dispatcher: the planar distance
 * and the height difference determine the elevation, scale bends the
 * arc (a gravity or curvature constant), and angle selects the launch
 * speed, so the output is a muzzle velocity for a grenade-type attack.
 * func_8005F6E8 calls it for its attack type 9, gSpawnLobShot and
 * gWeaponFireDispatch call it with table-driven angle and scale values from
 * their descriptors, and func_80058C3C calls it for a kind-7 effect.
 *
 * @par Unknown
 * What scale physically is (gravity, arc height or time of flight), why
 * the arctangent uses the fixed denominator 0x10000, whether the
 * elevation is aimed at the target or deliberately overshoots, and why
 * func_80058C3C passes the camera pointer as dst.
 *
 * @par Callers
 * func_80058C3C (0x80058D48), gSpawnLobShot (0x8005EB54),
 * func_8005F6E8 (0x8005FCE8) and gWeaponFireDispatch (0x8006B010); all
 * analysis/edges.json kind "direct".
 */
void gSolveLobVector(s32 angle, s32 *out, s32 *src, s32 *dst, s32 scale) __asm__("func_8005E624");

void gSolveLobVector(s32 angle, s32 *out, s32 *src, s32 *dst, s32 scale)
{
    s32 vec[4];
    s32 len;
    s32 dy;
    s32 s1;
    s32 t;
    s32 q;
    s32 d;
    s32 s0;
    s32 v;
    s32 q2;
    s32 q3;
    s32 q4;
    s32 q5;
    s32 a0;
    s32 v2;
    s32 sn;
    s32 c;
    s32 r4;
    s32 half4;
    u32 k;

    vec[0] = dst[0] - src[0];
    vec[1] = dst[2] - src[2];
    len = func_8001A7AC(vec);
    k = 0x80000000u;
    if (len < 2) {
        len = 2;
    }
    dy = dst[1] - src[1];
    s1 = angle * 300;
    MUL16(t, scale, len);
    MUL16(q, s1, s1);
    q >>= 1;
    MULU16(d, t, k / (u32)q);
    s0 = d / 2;

    if (dy > 0) {
        if (len > 0) {
            s32 half = len >> 1;
            s32 r;
            MULU16(r, dy, k / (u32)half);
            v = s0 + r;
        } else {
            s32 half = -len;
            s32 r;
            half >>= 1;
            MULU16(r, dy, k / (u32)half);
            v = s0 - r;
        }
    } else {
        register s32 ndy __asm__("$3");
        ndy = -dy;
        if (len > 0) {
            s32 half = len >> 1;
            s32 r;
            MULU16(r, ndy, k / (u32)half);
            v = s0 - r;
        } else {
            s32 half = -len;
            s32 r;
            half >>= 1;
            MULU16(r, ndy, k / (u32)half);
            v = s0 + r;
        }
    }

    MUL16(q2, s0, v);
    a0 = 0x10000 - (q2 << 2);
    if (a0 < 0) {
        a0 = 0;
    } else {
        a0 = func_80095900(a0) << 2;
    }
    a0 += 0x10000;

    v2 = s0 << 1;
    if (a0 > 0) {
        if (v2 > 0) {
            half4 = v2 >> 1;
            goto positive;
        }
        half4 = -v2;
        half4 >>= 1;
        goto negative;
    }
    if (v2 > 0) {
        half4 = v2 >> 1;
        a0 = -a0;
    negative:
        MULU16(r4, a0, 0x80000000u / (u32)half4);
        a0 = -r4;
    } else {
        a0 = -a0;
        half4 = -v2;
        half4 >>= 1;
    positive:
        MULU16(r4, a0, 0x80000000u / (u32)half4);
        a0 = r4;
    }

    q3 = func_80095C30(a0, 0x10000);
    gSinCos((q3 << 12) >> 12, &vec[2], &vec[3]);
    c = vec[3];
    if (c < 0) {
        vec[3] = -c;
        __asm__ volatile("" ::: "memory");
        c = vec[3];
    }
    MUL16(q4, c, s1);
    func_8001ABE0(q4, vec);
    out[0] = vec[0];
    sn = vec[2];
    MUL16(q5, sn, s1);
    out[1] = q5;
    out[2] = vec[1];
}
