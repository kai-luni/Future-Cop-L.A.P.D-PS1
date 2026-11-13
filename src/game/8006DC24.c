#include "game/types.h"
typedef struct { u32 w[13]; } QW;
typedef union Packet {
    u32 w[13];
    u16 h[26];
    u8 b[52];
} Packet;
typedef struct { u16 h[26]; } QH;
typedef struct Mat { s32 m[9]; } Mat;
typedef struct Vtx { u32 xy; s16 z; s16 pad; } Vtx;

typedef struct Obj {
    u8 pad0[0xE];
    s8 f0E;
    u8 pad0F[3];
    s16 f12;
    u8 pad14[6];
    u16 f1A;
    s32 *f1C;
    u8 pad20[0x14];
    Mat mat;
    u8 pad58[0xC];
    u16 f64;
    u8 pad66[0xA];
    u16 f70;
    u8 pad72[0xA];
    u8 f7C;
    u8 pad7D;
    u8 f7E;
    u8 pad7F;
    u8 f80;
    u8 pad81;
    u8 f82;
    u8 pad83[5];
    u32 f88;
    u32 f8C;
    u8 pad90[8];
    u16 f98;
} Obj;

extern s32 *D_8009F9E4;
extern u8 *D_8009F9F0;
extern u8 *D_8009FA28;
extern u32 D_8009F9B8;
extern u32 D_8009B214[];
extern s32 D_800A8E80[];

extern s32 *func_8003D9F8();
extern s32 func_8001A100();
extern void func_80010820();
extern void func_8001006C();
extern void func_80010750();
extern void func_8001047C();
extern void func_8001846C();

void func_8006DC24(Obj *o, s32 *pos)
{
    struct { s32 mat[9]; s32 flag[3]; s32 mask[2]; } w;
    s16 pts[6][4];
    s16 out[8][6][4];
    Vtx v[6];
    s32 vec[3];
    s32 c, s;
    s32 step;
    s16 half;
    s16 cnt;
    s32 *s0;
    s32 *s2;
    s32 nverts;
    register s32 angle __asm__("$22");
    s32 fp;
    s32 i, j, k, n, m;
    u16 r1;
    s32 r2, r3;
    Packet *p;
    register u32 tp __asm__("$7");
    s32 mt;

    angle = 0;
    half = (s16)o->f64 >> 3;
    r1 = o->f12;
    r2 = r1 + o->f70;
    if (!(o->f98 & 0x10)) {
        nverts = 4;
    } else {
        r3 = r1 + (o->f70 << 1);
        nverts = 6;
    }
    s0 = func_8003D9F8(o->f1C ? (s32)o->f1C[0xD] : 0, o->f1A, o->f0E, 1, o->f1C, &o->mat);
    vec[0] = s0[9] - pos[0];
    vec[1] = s0[10] - pos[1];
    vec[2] = s0[11] - pos[2];
    s2 = vec;
    if (func_8001A100(s2) - (o->f12 << 4) > 0x1FFFF)
        return;
    vec[0] = s0[9] - D_8009F9E4[0x1F];
    vec[1] = s0[10] - D_8009F9E4[0x20];
    vec[2] = s0[11] - D_8009F9E4[0x21];
    func_80010820(D_8009F9E4 + 10);
    func_8001006C(s2, w.flag, 1);
    if (o->f98 & 0x20) {
        mt = o->mat.m[0];
        w.mat[1] = o->mat.m[1];
        w.mat[2] = o->mat.m[2];
        w.mat[3] = o->mat.m[3];
        w.mat[4] = o->mat.m[4];
        w.mat[5] = o->mat.m[5];
        w.mat[6] = o->mat.m[6];
        w.mat[7] = o->mat.m[7];
        w.mat[8] = o->mat.m[8];
        w.mat[0] = mt;
    } else {
        func_80010820(D_8009F9E4 + 10);
        func_8001006C(s0, w.mat, 3);
    }
    func_80010750(w.mat);
    step = 0x200 / half;
    fp = 8;
    if (o->f98 & 0x400) fp = 2;
    if (o->f98 & 0x200)
        cnt = half >> 1;
    else
        cnt = half;
    for (k = 0; k < cnt; k++) {
        func_8001846C(angle, &c, &s);
        c >>= 4;
        s >>= 4;
        pts[0][0] = (o->f12 * c) >> 12;
        pts[0][1] = (o->f12 * s) >> 12;
        pts[1][0] = ((s16)r2 * c) >> 12;
        pts[1][1] = ((s16)r2 * s) >> 12;
        if (o->f98 & 0x10) {
            pts[4][0] = ((s16)r3 * c) >> 12;
            pts[4][1] = ((s16)r3 * s) >> 12;
        }
        if (k == half - 1) angle = 0x200;
        else angle += step;
        func_8001846C(angle, &c, &s);
        c >>= 4;
        s >>= 4;
        if (o->f98 & 0x10) {
            pts[5][0] = ((s16)r3 * c) >> 12;
            pts[5][1] = ((s16)r3 * s) >> 12;
        }
        pts[2][0] = ((s16)r2 * c) >> 12;
        pts[2][1] = ((s16)r2 * s) >> 12;
        pts[3][0] = (o->f12 * c) >> 12;
        pts[3][1] = (o->f12 * s) >> 12;
        n = 4;
        if (o->f98 & 0x10) n = 6;
        for (i = 0; i < 8; i++) {
            for (j = 0; j < n; j++) {
                s32 a, b, x, y;
                if ((D_8009B214[i] >> 2) & 1) { a = pts[j][1]; b = pts[j][0]; x = a; }
                else { b = pts[j][1]; a = pts[j][0]; x = a; }
                if (D_8009B214[i] & 1) x = -a;
                out[i][j][0] = x;
                out[i][j][1] = 0;
                y = b;
                if ((D_8009B214[i] >> 1) & 1) y = -b;
                out[i][j][2] = y;
            }
            if (i == 1 && (o->f98 & 0x400)) break;
        }
        for (i = 0; i < fp; i++) {
            func_8001047C(out[i], v, w.mask, nverts);
            if (w.mask[0] & 0x01010101) continue;
            if ((o->f98 & 0x10) && ((u16)w.mask[1] & 0x101)) continue;
            if (o->f98 & 1) {
                if (D_8009F9F0 < D_8009FA28) goto L_alloc1;
                __asm__("");
                p = (Packet *)D_8009F9F0;
                goto L_done1;
            L_alloc1:
                p = (Packet *)D_8009F9F0;
                D_8009F9F0 = (u8 *)p + 0x34;
            L_done1:
                p->b[3] = 9; p->b[7] = 0x2E;
                tp = D_800A8E80[5];
                p->b[4] = 0x80; p->b[5] = 0x80; p->b[6] = 0x80;
                p->b[0x1C] = p->b[0x24] = o->f7C;
                p->b[0xC] = p->b[0x14] = o->f7C + o->f80 - 1;
                p->b[0x1D] = p->b[0xD] = o->f7E;
                p->b[0x15] = p->b[0x25] = o->f7E + o->f82 - 1;
                {
                    u32 x0 = v[0].xy;
                    u32 x1 = v[1].xy;
                    u32 x2 = v[2].xy;
                    u32 x3 = v[3].xy;
                p->h[7] = tp >> 16;
                p->h[11] = tp;
                p->w[4] = x1;
                __asm__ __volatile__("lui %0,0xFF00" : "+r"(x1));
                p->w[2] = x0;
                p->w[6] = x3;
                p->w[8] = x2;
                {
                    s32 z0 = v[0].z;
                    u32 tag;
                    __asm__("" : "+r"(p) : "r"(z0));
                    tag = p->w[0] & x1;
                p->w[0] = (((z0 + v[1].z + v[2].z + v[3].z) >> 2) - 0x100 & 0xFFFFFF) |
                    tag;
                }
                }
                if (o->f98 & 0x10) {
                    if (D_8009F9F0 < D_8009FA28) goto L_alloc2;
                    __asm__("");
                    p = (Packet *)D_8009F9F0;
                    goto L_done2;
                L_alloc2:
                    p = (Packet *)D_8009F9F0;
                    D_8009F9F0 = (u8 *)p + 0x34;
                L_done2:
                    p->b[3] = 9; p->b[7] = 0x2E;
                    tp = D_800A8E80[5];
                    p->b[4] = 0x80; p->b[5] = 0x80; p->b[6] = 0x80;
                    p->b[0x1C] = p->b[0x24] = o->f7C;
                    p->b[0xC] = p->b[0x14] = o->f7C + o->f80 - 1;
                    p->b[0x1D] = p->b[0xD] = o->f7E;
                    p->b[0x15] = p->b[0x25] = o->f7E + o->f82 - 1;
                    {
                        u32 x1 = v[1].xy;
                        u32 x2 = v[2].xy;
                        u32 x4 = v[4].xy;
                        u32 x5 = v[5].xy;
                        p->h[7] = tp >> 16;
                    p->h[11] = tp;
                    p->w[6] = x4;
                    __asm__ __volatile__("lui %0,0xFF00" : "+r"(x4));
                    p->w[2] = x5;
                    p->w[4] = x2;
                    p->w[8] = x1;
                    {
                        s32 z0 = v[5].z;
                        u32 tag;
                        __asm__("" : "+r"(p) : "r"(z0));
                        tag = p->w[0] & x4;
                    p->w[0] = (((z0 + v[1].z + v[2].z + v[4].z) >> 2) - 0x100 & 0xFFFFFF) |
                        tag;
                    }
                    }
                }
            } else {
                u32 color0 = o->f88;
                u32 color1 = o->f8C;
                register u32 t2 __asm__("$10");
                register u32 t3 __asm__("$11");
                register s16 zz __asm__("$9");
                t2 = color0 >> 8;
                __asm__ __volatile__("lui %0,0x3E00" : "+r"(color0), "+r"(color1) : "r"(t2));
                t2 |= color0;
                t3 = color1 >> 8;
                if (o->f98 & 0x10) {
                    s32 first = v[0].z;
                    zz = first;
                    if (v[1].z < first) zz = v[1].z;
                    if (v[2].z < zz) zz = v[2].z;
                    if (v[3].z < zz) zz = v[3].z;
                    if (v[4].z < zz) zz = v[4].z;
                    if (v[5].z < zz) zz = v[5].z;
                } else {
                    s32 first = v[0].z;
                    zz = first;
                    if (v[1].z < first) zz = v[1].z;
                    if (v[2].z < zz) zz = v[2].z;
                    if (v[3].z < zz) zz = v[3].z;
                }
                zz -= 0x100;
                if (D_8009F9F0 < D_8009FA28) goto L_alloc3;
                __asm__("");
                p = (Packet *)D_8009F9F0;
                goto L_done3;
            L_alloc3:
                p = (Packet *)D_8009F9F0;
                D_8009F9F0 = (u8 *)p + 0x34;
            L_done3:
                p->b[3] = 0xC;
                p->w[7] = t2; p->w[1] = t2;
                p->w[10] = t3; p->w[4] = t3;
{
    u32 x0 = v[0].xy;
    u32 x1 = v[1].xy;
    u32 x2 = v[2].xy;
    u32 x3 = v[3].xy;
                p->w[3] = 0;
                p->w[2] = x0;
                p->w[5] = x1;
                p->w[8] = x3;
                p->w[11] = x2;
                p->w[0] = (zz & 0xFFFFFF) | (p->w[0] & 0xFF000000);
}
                p->h[18] = 0x101;
                p->h[24] = 0x100;
                p->w[6] = ((u32)D_8009F9B8 << 16) | 1;
                if (o->f98 & 0x10) {
                    if (D_8009F9F0 < D_8009FA28) goto L_alloc4;
                    __asm__("");
                    p = (Packet *)D_8009F9F0;
                    goto L_done4;
                L_alloc4:
                    p = (Packet *)D_8009F9F0;
                    D_8009F9F0 = (u8 *)p + 0x34;
                L_done4:
                    p->b[3] = 0xC;
                    p->w[7] = t2; p->w[1] = t2;
                    p->w[10] = t3; p->w[4] = t3;
    {
        u32 x1 = v[1].xy;
        u32 x2 = v[2].xy;
        u32 x4 = v[4].xy;
        u32 x5 = v[5].xy;
                        p->w[3] = 0;
                    p->w[2] = x5;
                    p->w[5] = x2;
                    p->w[8] = x4;
                    p->w[11] = x1;
                    p->w[0] = (zz & 0xFFFFFF) | (p->w[0] & 0xFF000000);
    }
                    p->h[18] = 0x101;
                    p->h[24] = 0x100;
                    p->w[6] = ((u32)D_8009F9B8 << 16) | 1;
                }
            }
        }
    }
}
