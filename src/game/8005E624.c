#include "game/types.h"

extern s32 func_8001A7AC(s32 *vec);
extern void func_8001ABE0(s32 scale, s32 *p);
extern s32 func_80095900(s32 value);
extern s32 func_80095C30(s32 x, s32 y);
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

#define MULU16(out, a, b) \
    __asm__ volatile("multu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")
#define MUL16(out, a, b) \
    __asm__ volatile("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

void func_8005E624(s32 angle, s32 *out, s32 *src, s32 *dst, s32 scale)
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
    func_8001846C((q3 << 12) >> 12, &vec[2], &vec[3]);
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
