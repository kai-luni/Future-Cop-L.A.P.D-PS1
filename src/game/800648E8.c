#include "game/types.h"

extern u32 D_8009AFE8[];
extern u32 *D_8009F9F0;
extern u32 *D_8009FA28;
extern u32 D_800A8E80[];
extern u8 D_800A2630[];
extern u16 D_8009F83C;
extern s32 func_800648BC(s32 a, s32 b, s32 c);

typedef struct { s32 w0; s16 h4; u8 b6; u8 b7; } V;

void func_800648E8(u8 *base, u32 arg1, u32 *tbl, u32 arg3)
{
    u32 w, x0, x1, x2, x3;
    register u32 b0 __asm__("$6");
    register u32 b1 __asm__("$3");
    register u32 b2 __asm__("$5");
    V *v0, *v1, *v2, *v3;
    u32 *pkt;
    s32 avg;
    u16 *pa;
    u16 *pb;
    u32 f;
    u32 col;
    u32 op;
    register u32 t __asm__("$5");
    u8 *o;
    u16 *p5;
    u16 *a1;
    u32 t3;
    u32 a0;
    u32 k;
    u8 c;
    u16 *cl;
    u32 e;

    w = D_8009AFE8[(arg1 >> 15) & 0x7F];
    b0 = w & 0xFF;
    w >>= 8;
    b1 = w & 0xFF;
    w >>= 8;
    b2 = w & 0xFF;
    w >>= 8;
    __asm__("" : "+r"(b0), "+r"(b1), "+r"(b2), "+r"(w));
    v1 = (V *)(base + (b1 << 3));
    v0 = (V *)(base + (b0 << 3));
    v2 = (V *)(base + (b2 << 3));
    x1 = v1->b7;
    x0 = v0->b7;
    t = x0 & x1;
    f = x0 | x1;
    x1 = v2->b7;
    t &= x1;
    f |= x1;
    v3 = (V *)(base + (w << 3));
    x1 = v3->b7;
    f |= x1;
    x2 = x1 & t;
    if (f & 1) return;
    if (!(f & 2) && x2) return;
    if (arg1 & 1) {
        register s32 pa0 __asm__("$4");
        register s32 pa1 __asm__("$5");
        register s32 pa2 __asm__("$6");
        pa0 = v0->w0;
        __asm__ __volatile__("");
        pa1 = v1->w0;
        __asm__ __volatile__("");
        pa2 = v2->w0;
        if (func_800648BC(pa0, pa1, pa2)) return;
    }
    if (v0 == v3) {
        avg = (v3->h4 + v1->h4 + v2->h4);
        avg += v3->h4 + avg * 4;
        avg >>= 4;
    } else {
        avg = (v0->h4 + v1->h4 + v2->h4 + v3->h4) >> 2;
    }
    if (D_8009F9F0 >= D_8009FA28) {
        o = (u8 *)D_8009F9F0;
        __asm__ __volatile__("");
    } else {
        o = (u8 *)D_8009F9F0;
        D_8009F9F0 = (u32 *)o + 13;
    }
    *(u32 *)o = (*(u32 *)o & 0xFF000000) | (avg & 0xFFFFFF);
    p5 = (u16 *)((u8 *)tbl[1] + ((arg1 >> 21) & 0x7FE));
    a1 = (u16 *)((u8 *)tbl[2] + (arg1 & 0x7FE));
    t3 = *p5;
    if (t3 & 0x800) {
        volatile u16 *d = (volatile u16 *)0x1F800070;
        u16 off = D_8009F83C;
        d[0] = ((volatile u16 *)a1)[0] + off;
        d[1] = ((volatile u16 *)a1)[1] + off;
        d[2] = ((volatile u16 *)a1)[2] + off;
        d[3] = ((volatile u16 *)a1)[3] + off;
        a1 = (u16 *)0x1F800070;
    }
    a0 = D_800A8E80[(t3 >> 8) & 7];
    k = t3 >> 13;
    switch (k) {
    case 0:
    case 1:
        c = t3 & 0xFC;
        o[4] = c; o[5] = c; o[6] = c;
        *(u32 *)(o + 0xC) = a1[0] | (a0 & 0xFFFF0000);
        *(u32 *)(o + 0x14) = a1[1] | (a0 << 16);
        *(u32 *)(o + 8) = v0->w0;
        *(u32 *)(o + 0x10) = v1->w0;
        if (k == 0) {
            o[3] = 7;
            o[7] = ((t3 & 0x1000) >> 11) | 0x24;
            *(u32 *)(o + 0x18) = v2->w0;
            *(u16 *)(o + 0x1C) = a1[2];
        } else {
            o[3] = 9;
            o[7] = ((t3 & 0x1000) >> 11) | 0x2C;
            *(u16 *)(o + 0x24) = a1[2];
            *(u16 *)(o + 0x1C) = a1[3];
            *(u32 *)(o + 0x20) = v2->w0;
            *(u32 *)(o + 0x18) = v3->w0;
        }
        break;
    case 2:
    case 4:
    case 6:
        o[3] = 9;
        o[7] = ((t3 & 0x1000) >> 11) | 0x34;
        *(u32 *)(o + 0xC) = a1[0] | (a0 & 0xFFFF0000);
        *(u32 *)(o + 0x18) = a1[1] | (a0 << 16);
        *(u16 *)(o + 0x24) = a1[2];
        *(u32 *)(o + 8) = v0->w0;
        *(u32 *)(o + 0x14) = v1->w0;
        *(u32 *)(o + 0x20) = v2->w0;
        if (k == 2) {
            t3 = (t3 << 16) | p5[1];
            c = (t3 >> 16) & 0xFC;
            o[4] = c; o[5] = c; o[6] = c;
            c = (t3 >> 10) & 0xFC;
            o[0x10] = c; o[0x11] = c; o[0x12] = c;
            c = (t3 >> 4) & 0xFC;
            o[0x1C] = c; o[0x1D] = c; o[0x1E] = c;
            break;
        } else if (k == 4) {
            u32 sh, m;
            cl = (u16 *)tbl[0];
            e = cl[t3 & 0xFF];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[4] = (e >> (8 - sh)) & m;
            o[5] = (e >> (3 - sh)) & m;
            o[6] = (e << (sh + 2)) & m;
            t3 = p5[1];
            e = cl[t3 >> 8];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x10] = (e >> (8 - sh)) & m;
            o[0x11] = (e >> (3 - sh)) & m;
            o[0x12] = (e << (sh + 2)) & m;
            e = cl[t3 & 0xFF];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x1C] = (e >> (8 - sh)) & m;
            o[0x1D] = (e >> (3 - sh)) & m;
            o[0x1E] = (e << (sh + 2)) & m;
            break;
        } else {
            u8 *tab = D_800A2630;
            u32 ty = t3 & 7, lo = arg3 & 0xF, hi = arg3 & 0xF0, lo1 = (lo + 1) & 0xF, hi1 = (hi + 0x10) & 0xF0;
            u8 *p;
            if (ty != 1) p = tab + arg3;
            else p = tab + (hi | lo1);
            c = *p;
            o[4] = c; o[5] = c; o[6] = c;
            c = tab[ty == 3 ? (hi1 | lo1) : (hi1 | lo)];
            o[0x1C] = c; o[0x1D] = c; o[0x1E] = c;
            c = tab[(ty == 3 || ty == 0) ? (hi | lo1) : (hi1 | lo1)];
            o[0x10] = c; o[0x11] = c; o[0x12] = c;
            return;
        }
    case 3:
    case 5:
    case 7:
        o[3] = 0xC;
        o[7] = ((t3 & 0x1000) >> 11) | 0x3C;
        *(u32 *)(o + 0xC) = a1[0] | (a0 & 0xFFFF0000);
        *(u32 *)(o + 0x18) = a1[1] | (a0 << 16);
        *(u16 *)(o + 0x30) = a1[2];
        *(u16 *)(o + 0x24) = a1[3];
        *(u32 *)(o + 8) = v0->w0;
        *(u32 *)(o + 0x14) = v1->w0;
        *(u32 *)(o + 0x2C) = v2->w0;
        *(u32 *)(o + 0x20) = v3->w0;
        if (k == 3) {
            t3 = (t3 << 16) | p5[1];
            c = (t3 >> 16) & 0xFC;
            o[4] = c; o[5] = c; o[6] = c;
            c = (t3 >> 10) & 0xFC;
            o[0x10] = c; o[0x11] = c; o[0x12] = c;
            c = (t3 >> 4) & 0xFC;
            o[0x28] = c; o[0x29] = c; o[0x2A] = c;
            c = (t3 & 0x3F) << 2;
            o[0x1C] = c; o[0x1D] = c; o[0x1E] = c;
            break;
        } else if (k == 5) {
            u32 sh, m;
            cl = (u16 *)tbl[0];
            e = cl[t3 & 0xFF];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[4] = (e >> (8 - sh)) & m;
            o[5] = (e >> (3 - sh)) & m;
            o[6] = (e << (sh + 2)) & m;
            t3 = p5[1];
            e = cl[t3 >> 8];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x10] = (e >> (8 - sh)) & m;
            o[0x11] = (e >> (3 - sh)) & m;
            o[0x12] = (e << (sh + 2)) & m;
            e = cl[t3 & 0xFF];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x28] = (e >> (8 - sh)) & m;
            o[0x29] = (e >> (3 - sh)) & m;
            o[0x2A] = (e << (sh + 2)) & m;
            t3 = p5[2];
            e = cl[t3 >> 8];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x1C] = (e >> (8 - sh)) & m;
            o[0x1D] = (e >> (3 - sh)) & m;
            o[0x1E] = (e << (sh + 2)) & m;
            break;
        } else {
            u32 lo = arg3 & 0xF, hi = arg3 & 0xF0, lo1 = (lo + 1) & 0xF, hi1 = (hi + 0x10) & 0xF0;
            c = D_800A2630[arg3];
            o[4] = c; o[5] = c; o[6] = c;
            c = D_800A2630[hi | lo1];
            o[0x10] = c; o[0x11] = c; o[0x12] = c;
            c = D_800A2630[hi1 | lo1];
            o[0x28] = c; o[0x29] = c; o[0x2A] = c;
            c = D_800A2630[hi1 | lo];
            o[0x1C] = c; o[0x1D] = c; o[0x1E] = c;
            break;
        }
    default:
        return;
    }
}
