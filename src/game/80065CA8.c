#include "game/types.h"

extern u8 D_8009AFE8[][4];
extern u8 D_8009B1A4[];
extern u8 *D_8009F9F0;
extern u8 *D_8009FA28;
extern u32 D_800A8E80[];
extern u8 D_800A2630[];
extern u16 D_8009F83C;

#define PK ((u8 *)t0)
#define SRC ((u16 *)a0)
#define TEX ((u16 *)t4)

#define PAL(off, c)                                    \
    a1 = (c);                                          \
    a0 = a1 >> 15;                                     \
    PK[(off)] = (a1 >> (8 - a0)) & (t1 = 0x1F << (a0 + 2)); \
    PK[(off) + 1] = (a1 >> (3 - a0)) & t1;             \
    PK[(off) + 2] = (a1 << (a0 + 2)) & t1;

#define GRAY(off, c)                                   \
    gr = (c);                                          \
    PK[(off)] = gr;                                    \
    PK[(off) + 1] = gr;                                \
    PK[(off) + 2] = gr;

void func_80065CA8(u8 *ctx, s32 arg1, s32 x, s32 y)
{
    s32 idx;
    u32 *wp;
    s8 *base;
    s32 count;
    s16 v[8];
    s32 t0;
    u32 t1;
    u32 t2;
    s32 t3;
    s32 t4;
    s32 a0;
    u32 a1;
    s32 lo, hi, lo1, hi1;
    u32 gr;
    u32 wd;

    t2 = arg1 >> 4;
    idx = arg1;
    a1 = ((u16 *)(ctx + 0x3CC))[idx];
    t2 = t2 + idx;
    x = x >> 16;
    y = y >> 16;
    wp = (u32 *)(ctx + (((s32)a1 >> 4) + 0x5D0));
    count = a1 & 0xF;
    base = (s8 *)(ctx + (t2 * 3 + 0xC));

    for (; count > 0; count--) {
        wd = *wp++;
        t0 = (wd >> 15) & 0x7F;
        if (t0 >= 0x47) {
            return;
        }
        { u8 *e = D_8009AFE8[t0];
        a1 = base[e[0]] + base[e[1]] + base[e[2]] + base[e[3]]; }
        switch (D_8009B1A4[t0] >> 3) {
        case 0:
            v[4] = x; v[0] = x; v[3] = y; v[1] = y; v[2] = x + 10;
            v[5] = y + 10;
            break;
        case 1:
            v[2] = x + 10; v[0] = x + 10; v[1] = y; v[4] = x;
            v[5] = y + 10; v[3] = y + 10;
            break;
        case 2:
            v[4] = x; v[0] = x; v[1] = y; v[2] = x + 10;
            v[5] = y + 10; v[3] = y + 10;
            break;
        case 3:
            v[0] = x; v[3] = y; v[1] = y; v[4] = x + 10; v[2] = x + 10;
            v[5] = y + 10;
            break;
        case 4:
            v[6] = x; v[0] = x; v[3] = y; v[1] = y; v[4] = x + 10; v[2] = x + 10;
            v[7] = y + 10; v[5] = y + 10;
            break;
        }
        if (D_8009F9F0 < D_8009FA28) {
            t0 = (s32)D_8009F9F0;
            D_8009F9F0 = PK + 0x34;
        } else {
            t0 = (s32)D_8009F9F0;
        }
        *(u32 *)PK = (*(u32 *)PK & 0xFF000000) | ((0x400 - a1) & 0xFFFFFF);
        t4 = *(s32 *)(ctx + 4) + ((wd >> 21) & 0x7FE);
        a0 = *(s32 *)(ctx + 8) + (wd & 0x7FE);
        t2 = *TEX;
        if (t2 & 0x800) {
            u16 g = D_8009F83C;
            u16 *d = (u16 *)0x1F800070;
            d[0] = *(u16 *)(a0 + 0) + g;
            d[1] = *(u16 *)(a0 + 2) + g;
            d[2] = *(u16 *)(a0 + 4) + g;
            d[3] = *(u16 *)(a0 + 6) + g;
            a0 = 0x1F800070;
        }
        a1 = D_800A8E80[(t2 >> 8) & 7];
        t1 = t2 >> 13;
        switch (t1) {
        case 0:
            gr = t2 & 0xFC;
            PK[3] = 7;
            PK[7] = ((t2 & 0x1000) >> 11) | 0x24;
            PK[4] = gr;
            PK[5] = gr;
            PK[6] = gr;
            *(u32 *)(PK + 0xC) = SRC[0] | (a1 & 0xFFFF0000);
            *(u32 *)(PK + 0x14) = SRC[1] | (a1 << 16);
            *(u16 *)(PK + 0x1C) = SRC[2];
            *(u32 *)(PK + 0x8) = *(u32 *)&v[0];
            *(u32 *)(PK + 0x10) = *(u32 *)&v[2];
            *(u32 *)(PK + 0x18) = *(u32 *)&v[4];
            break;
        case 1:
            gr = t2 & 0xFC;
            PK[3] = 9;
            PK[7] = ((t2 & 0x1000) >> 11) | 0x2C;
            PK[4] = gr;
            PK[5] = gr;
            PK[6] = gr;
            *(u32 *)(PK + 0xC) = SRC[0] | (a1 & 0xFFFF0000);
            *(u32 *)(PK + 0x14) = SRC[1] | (a1 << 16);
            *(u16 *)(PK + 0x24) = SRC[2];
            *(u16 *)(PK + 0x1C) = SRC[3];
            *(u32 *)(PK + 0x8) = *(u32 *)&v[0];
            *(u32 *)(PK + 0x10) = *(u32 *)&v[2];
            *(u32 *)(PK + 0x20) = *(u32 *)&v[4];
            *(u32 *)(PK + 0x18) = *(u32 *)&v[6];
            break;
        case 2:
        case 4:
        case 6:
            PK[3] = 9;
            PK[7] = ((t2 & 0x1000) >> 11) | 0x34;
            *(u32 *)(PK + 0xC) = SRC[0] | (a1 & 0xFFFF0000);
            *(u32 *)(PK + 0x18) = SRC[1] | (a1 << 16);
            *(u16 *)(PK + 0x24) = SRC[2];
            *(u32 *)(PK + 0x8) = *(u32 *)&v[0];
            *(u32 *)(PK + 0x14) = *(u32 *)&v[2];
            *(u32 *)(PK + 0x20) = *(u32 *)&v[4];
            if (t1 == 2) {
                t2 = (t2 << 16) | TEX[1];
                GRAY(4, (t2 >> 16) & 0xFC);
                GRAY(0x10, (t2 >> 10) & 0xFC);
                GRAY(0x1C, (t2 >> 4) & 0xFC);
            } else if (t1 == 4) {
                t3 = *(s32 *)ctx;
                PAL(4, ((u16 *)t3)[t2 & 0xFF]);
                t2 = TEX[1];
                PAL(0x10, ((u16 *)t3)[t2 >> 8]);
                PAL(0x1C, ((u16 *)t3)[t2 & 0xFF]);
            } else {
                a1 = t2 & 7;
                t3 = idx & 0xF;
                t2 = idx & 0xF0;
                a0 = (t3 + 1) & 0xF;
                t1 = (t2 + 0x10) & 0xF0;
                t4 = (s32)D_800A2630;
                GRAY(4, *(a1 != 1 ? (u8 *)t4 + idx : (u8 *)t4 + (t2 | a0)));
                GRAY(0x1C, ((u8 *)t4)[a1 != 3 ? (t1 | t3) : (t1 | a0)]);
                GRAY(0x10, ((u8 *)t4)[(a1 == 3 || a1 == 0) ? (t2 | a0) : (t1 | a0)]);
            }
            break;
        case 3:
        case 5:
        case 7:
            PK[3] = 0xC;
            PK[7] = ((t2 & 0x1000) >> 11) | 0x3C;
            *(u32 *)(PK + 0xC) = SRC[0] | (a1 & 0xFFFF0000);
            *(u32 *)(PK + 0x18) = SRC[1] | (a1 << 16);
            *(u16 *)(PK + 0x30) = SRC[2];
            *(u16 *)(PK + 0x24) = SRC[3];
            *(u32 *)(PK + 0x8) = *(u32 *)&v[0];
            *(u32 *)(PK + 0x14) = *(u32 *)&v[2];
            *(u32 *)(PK + 0x2C) = *(u32 *)&v[4];
            *(u32 *)(PK + 0x20) = *(u32 *)&v[6];
            if (t1 == 3) {
                t2 = (t2 << 16) | TEX[1];
                GRAY(4, (t2 >> 16) & 0xFC);
                GRAY(0x10, (t2 >> 10) & 0xFC);
                GRAY(0x28, (t2 >> 4) & 0xFC);
                GRAY(0x1C, (t2 & 0x3F) << 2);
            } else if (t1 == 5) {
                t3 = *(s32 *)ctx;
                PAL(4, ((u16 *)t3)[t2 & 0xFF]);
                t2 = TEX[1];
                PAL(0x10, ((u16 *)t3)[t2 >> 8]);
                PAL(0x28, ((u16 *)t3)[t2 & 0xFF]);
                t2 = TEX[2];
                PAL(0x1C, ((u16 *)t3)[t2 >> 8]);
            } else {
                lo = idx & 0xF;
                hi = idx & 0xF0;
                lo1 = (lo + 1) & 0xF;
                hi1 = (hi + 0x10) & 0xF0;
                GRAY(4, D_800A2630[idx]);
                GRAY(0x10, D_800A2630[hi | lo1]);
                GRAY(0x28, D_800A2630[hi1 | lo1]);
                GRAY(0x1C, D_800A2630[hi1 | lo]);
            }
            break;
        }
    }
}
