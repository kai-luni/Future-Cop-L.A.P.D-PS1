#include "game/types.h"

typedef struct Node {
    s32 next;
    s16 x4;
    s16 y6;
    s16 z8;
    u16 wA;
    s16 xC;
    s16 yE;
    s16 z10;
    s16 w12;
    u8 b14;
    u8 r;
    u8 g;
    u8 b;
    u16 w18;
} Node;

typedef struct Vec {
    s32 x, y, z;
} Vec;

typedef struct Mat {
    s32 m[9];
    s32 x, y, z;
} Mat;

typedef struct Ent {
    u8 pad00[0x0E];
    s8 unk0E;
    u8 pad0F[0x0B];
    u16 unk1A;
    void *unk1C;
    u8 *unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    Mat mat34;
    u8 pad64[0x18];
    u16 unk7C;
    u16 unk7E;
    u16 unk80;
    u16 unk82;
    s16 unk84;
    s16 unk86;
    s16 unk88;
    u8 pad8A[4];
    s8 unk8E;
    u8 unk8F;
    u8 unk90;
    u8 unk91;
    u8 unk92;
    u8 unk93;
    u8 unk94;
    u8 pad95;
    s16 unk96;
    s16 unk98;
    s16 unk9A;
    s16 unk9C;
    s16 unk9E;
    s16 unkA0;
    u8 unkA2;
    u8 unkA3;
} Ent;

extern s32 D_8009FAB0;
extern s32 func_80019668(void);
extern Mat *func_8003D9F8(void *a0, u16 a1, s8 a2, s32 a3, void *a4, Mat *a5);
extern void func_800188D0(s32 a, s32 b, s32 c, s32 *out);
extern void func_8001B148(Mat *m, s16 *src, s16 *dst, s32 n);
extern Node *func_80042764(Ent *e);
extern void func_80042788(Ent *e, Node *n);

void func_80042AF4(Ent *self, Node *src, u8 flag)
{
    s32 buf[14];
    u8 fl;
    s32 v54;
    s32 v58;
    s32 sc;
    s32 base;
    Node *n;
    Node *n2;
    Mat *mat;
    void *par;
    s32 i;
    s32 s3;
    s32 s0;
    s32 s4;
    s32 sh0;
    s32 sh1;
    s32 t;

    fl = flag;
    if (src == 0) {
        fl = 0;
    }
    par = self->unk1C;
    mat = func_8003D9F8(par ? *(void **)((u8 *)par + 0x34) : 0, self->unk1A,
                        self->unk0E, 1, par, &self->mat34);
    v58 = self->unk86;
    base = (*(s32 *)(self->unk20 + 0x20) * 300) >> 16;
    v54 = self->unk84;
    if ((s16)self->unk80 == -1) {
        self->unk80 = self->unk82;
    }
    sc = self->unk82 - self->unk80;
    for (i = 0; i < self->unk88; i++) {
        if (src != 0) {
            n = &src[i];
            if (fl == 0) {
                n = func_80042764(self);
                if (n == 0) {
                    return;
                }
                *n = src[i];
                goto after;
            }
        } else {
            n = func_80042764(self);
            if (n == 0) {
                return;
            }
        }
        {
            register s32 r __asm__("$2");
            r = func_80019668() & 0xFFFF;
            __asm__("li\t$9,4096\n\taddiu\t$3,$9,0\n\tmult\t%1,$3\n\tmflo\t$8\n\tsrl\t%0,$8,16" : "=r"(r) : "r"(r));
            __asm__ volatile(".set\tnoreorder\n\tjal\tfunc_80019668\n\taddu\t%0,$0,%1\n\t.set\treorder" : "=&r"(s3), "+r"(r));
            s0 = v54 + ((u32)((r & 0xFFFF) * (v58 - v54)) >> 16);
        }
        if ((s16)sc != 0) {
            s4 = (u16)self->unk80 + ((u32)((func_80019668() & 0xFFF) * (s16)sc) >> 12);
        } else {
            s4 = (u16)self->unk82;
        }
        func_800188D0(s3, s0, 0, buf);
        *(Vec *)&buf[10] = *(Vec *)&buf[3];
        if (self->unkA0 == 0) {
            n->x4 = 0;
            n->y6 = 0;
            n->z8 = 0;
        } else if (self->unkA0 < 0) {
            t = (s32)(((-self->unkA0) * (func_80019668() & 0xFFF)) << 4) >> 16;
            n->x4 = ((buf[10] >> 4) * t) >> 12;
            n->y6 = ((buf[11] >> 4) * t) >> 12;
            n->z8 = ((buf[12] >> 4) * t) >> 12;
        } else {
            n->x4 = ((buf[10] >> 4) * self->unkA0) >> 12;
            n->y6 = ((buf[11] >> 4) * self->unkA0) >> 12;
            n->z8 = ((buf[12] >> 4) * self->unkA0) >> 12;
        }
        n->xC = ((buf[10] >> 4) * (s16)s4) >> 12;
        n->yE = ((buf[11] >> 4) * (s16)s4) >> 12;
        n->z10 = ((buf[12] >> 4) * (s16)s4) >> 12;
        if ((s16)self->unk7E < 0) {
            n->wA = ((u32)((-(s16)self->unk7E) * (func_80019668() & 0xFFF)) >> 12);
        } else {
            n->wA = self->unk7E;
        }
        n->w12 = 0x1000000 / (u32)(s16)n->wA;
        n->w12 = ((s16)n->w12 * (s16)self->unk7C) >> 12;
after:
        if (src != 0 && fl != 0) {
            n2 = func_80042764(self);
            if (n2 == 0) {
                return;
            }
            *n2 = *n;
            n = n2;
        }
        sh0 = 0x20 - self->unkA2;
        sh1 = 0x20 - self->unkA3;
        if (D_8009FAB0 >= 0x2BC) {
            return;
        }
        D_8009FAB0++;
        if (self->unk96 < 0) {
            n->r = self->unk92;
        } else {
            n->r = self->unk8F;
        }
        if (self->unk98 < 0) {
            n->g = self->unk93;
        } else {
            n->g = self->unk90;
        }
        if (self->unk9A < 0) {
            n->b = self->unk94;
        } else {
            n->b = self->unk91;
        }
        if (sh0 < 0x20) {
            n->xC += func_80019668() >> sh0;
            n->yE += func_80019668() >> sh0;
            n->z10 += func_80019668() >> sh0;
        }
        if (sh1 < 0x20) {
            n->x4 += func_80019668() >> sh1;
            n->y6 += func_80019668() >> sh1;
            n->z8 += func_80019668() >> sh1;
        }
        if (self->unk1A & 0x100) {
            func_8001B148(&self->mat34, &n->xC, &n->xC, 1);
            func_8001B148(&self->mat34, &n->x4, &n->x4, 1);
            n->x4 += self->mat34.x >> 4;
            n->y6 += self->mat34.y >> 4;
            n->z8 += self->mat34.z >> 4;
        } else {
            func_8001B148(mat, &n->xC, &n->xC, 1);
            func_8001B148(mat, &n->x4, &n->x4, 1);
            n->x4 += (mat->x - self->unk28) >> 4;
            n->y6 += (mat->y - self->unk2C) >> 4;
            n->z8 += (mat->z - self->unk30) >> 4;
        }
        n->b14 = 0;
        n->w18 = base & 0x1FFF;
        if (self->unk8E < 0) {
            n->w18 |= (((func_80019668() % (u32)(-(s32)self->unk8E)) + 1) << 13);
        } else {
            n->w18 = (base & 0x1FFF) | (self->unk8E << 13);
        }
        func_80042788(self, n);
    }
}
