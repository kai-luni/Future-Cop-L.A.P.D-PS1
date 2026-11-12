#include "game/types.h"

typedef long long s64;

typedef struct V3 { s32 x; s32 y; s32 z; } V3;
typedef union Prod { s64 v; u32 w[2]; } Prod;

struct Sub { u8 p00[0x10]; s32 f10; u8 p14[0x20]; s32 f34; };
struct Res { u8 p00[8]; s32 f08; u8 p0c[0x44]; V3 pos; };
struct Hit { u8 p00[0x14]; u16 f14; u8 p16[0x96]; struct Hit *fAC; };
struct Q { u8 p00[0x3E]; u16 f3E; u8 p40[8]; s32 f48; };

struct Obj {
    u8 p00[0x16]; u8 type; u8 p17[2]; u8 f19; u8 p1a[2];
    s16 f1C; u16 f1E; u8 f20; u8 p21[3];
    u32 f24; struct Sub *f28; V3 f2C; V3 f38; u8 p44[0xC];
    V3 pos;
    u8 p5C[0xC]; s32 f68; s16 f6C; u8 p6E[2]; s16 f70; u8 p72[6];
    V3 f78; u8 p84[8]; s32 f8C; u8 p90[8]; s32 f98; u8 p9C[4];
    u32 fA0; u8 pA4[0x24];
    struct Res *fC8; s32 fCC; s32 fD0; u8 pD4[4]; s32 fD8;
    V3 fDC; V3 fE8; s32 fF4; s16 fF8; u8 fFA; u8 pFB;
    u16 fFC; u16 fFE; u16 f100; u16 f102; u8 f104; u8 f105;
    s16 f106; s16 f108; s16 f10A; s16 f10C; s16 f10E;
    s16 f110; s16 f112; s16 f114; s16 f116; s32 f118; s32 f11C;
    u8 p120[0xC]; s16 f12C; u8 p12E[6]; struct Q *f134;
};

extern s32 *D_8009FA94;
extern struct Hit *D_8009FA58;
extern s32 D_8009FAA4;

extern void func_80044B34(void *, void *, s32, s32);
extern void func_800175F8(void *, s32);
extern s32 func_80019668(void);
extern struct Res *func_8004E80C(void *, s32, s32, s32, s32, s32, s32);
extern void func_800320E4(struct Obj *);
extern s32 func_80069A38(V3 *);
extern s32 func_80066F78(V3 *, s32, s32, s32);
extern void func_8003BA50(struct Obj *, s32);
extern void func_8003B898(s32, struct Obj *, s32, s32, s32);
extern struct Hit *func_8004D664(s32);
extern void func_80069D68(s32, s32, s32, struct Obj *);
extern void func_8001ACA4(V3 *, V3 *, s32, V3 *);
extern void func_8001A180(V3 *);
extern s32 func_8001A750(V3 *, V3 *);
extern void func_8001A644(V3 *);
extern void func_8001AB78(s32, V3 *);
extern void func_8001A6F8(V3 *, V3 *);
extern void func_80019310(V3 *, V3 *, V3 *);
extern void func_80010750(V3 *);
extern void func_80010234(s32, void *, s32);
#define FMUL(d, x, y) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(x), "r"(y) : "$8", "$9")

extern void func_80060238(u32 *, s32, struct Obj *, s32, s32);

#define AB8(v) ((((v) >> 8) >> 31) + ((v) >> 8) ^ (((v) >> 8) >> 31))
s32 func_800324E0(s32 unused, struct Obj *o, s32 dt)
{
    V3 a;
    V3 b;
    s32 ay;
    s32 cmp;
    V3 *q;

    if (o->f28->f10 != 0) {
        func_80044B34(o, (u8 *)o + 0x120, dt, 0);
        func_800175F8((u8 *)o + 0x28, o->f12C);
    }

    if (o->type != 0x25) {
        o->f102 -= dt;
        if ((s16)o->f102 < 0) {
            o->f102 = (func_80019668() & 0x1FF) + 0x384;
            o->fC8 = func_8004E80C(o, o->f105, o->f106, o->f104,
                                   o->f108 << 4, 0xFFFF0000, 0);
            if (o->fC8 == 0)
                goto L5E4;
            o->fCC = o->fC8->f08;
        }
        if (o->fC8 != 0 && o->fCC != o->fC8->f08) {
            func_800320E4(o);
            o->fC8 = 0;
        }
L5E4:
        if (o->fFA < 2) {
            o->f100 -= dt;
            if ((s16)o->f100 < 0) {
                if (o->fFA == 0) {
                    func_800320E4(o);
                    o->fFA = 1;
                    o->f100 = o->fFE +
                        (u32)((s16)o->fFE * (func_80019668() & 0xFF)) / 1024;
                } else {
                    o->fFA = 0;
                    o->f100 = o->fFC +
                        (u32)((s16)o->fFC * (func_80019668() & 0xFF)) / 1024;
                }
            }
        }
    }

    switch (o->fFA) {
    case 4: {
        s32 gr;
        o->pos.y += (dt * o->fF4) >> 9;
        gr = func_80069A38(&o->pos) + (o->f6C << 4);
        if (o->pos.y - gr > (o->f114 << 4))
            o->fFA = 0;
        a.z = 0;
        a.x = 0;
        ay = 0x10000;
        goto L_ED0;
    }
    case 0: {
        s32 cA;
        s32 dA;
        if (o->type != 0x25) {
            cA = o->pos.x - o->fD0;
            cA = AB8(cA);
            dA = o->pos.z - o->fD8;
            if (o->f10E < cA || o->f10C < AB8(dA)) {
                func_800320E4(o);
                b = o->fDC;
                if (o->type == 0x25)
                    goto L_AD4;
                o->fFA = 1;
                o->f100 = (func_80019668() & 0x7FF) + 0x4B0;
                goto L_AD4;
            }
        }
        if (o->fC8 != 0) {
            b = o->fC8->pos;
            goto L_AD4;
        }
        b = o->fDC;
        if (o->type == 0x25)
            goto L_AD4;
        o->fFA = 1;
        o->f100 = 0x4B0 - ((0x4B0 - (s16)o->fFE) >> 31 & (0x4B0 - (u16)o->fFE));
        goto L_AD4;
    }
    case 1:
        b = o->fDC;
        goto L_AD4;
    case 3: {
        V3 *pp;
        V3 *f78p;
        s32 t3;
        s32 c3;
        pp = &o->pos;
        t3 = func_80066F78(pp, 0, 0, 0);
        if (!(t3 + (o->f68 >> 1) < o->pos.y)) {
            if (o->type == 0x25) {
                s32 k;
                k = D_8009FA94[0x13];
                o->f1C = o->f1E;
                o->f24 |= 0x35;
                o->f100 = (10 - k) * 150 + 600;
                if (o->f20 != 0) {
                    if (!(o->f24 & 0x40000))
                        func_8003BA50(o, 0);
                }
                o->f1C = -1;
                func_8003B898(o->f20, o, 0, (o->f19 >> 3) & 0x1E, 2);
                if (o->f134->f48 == 0)
                    o->f134->f48 = D_8009FAA4;
                o->f134->f3E++;
                if (o->f8C != 0) {
                    struct Hit *h;
                    h = func_8004D664(o->f8C & 0x3FFFFFFF);
                    if (h != 0 && D_8009FA58 != 0 &&
                        (h->f14 & 0x3FFF) == D_8009FA58->f14)
                        D_8009FA58->fAC->f14++;
                }
                if (o->f98 != 0)
                    func_80069D68(o->f98, 0, -1, o);
            }
            o->fFA = 5;
            return 0;
        }
        func_8001ACA4(&o->fE8, &o->fDC, -0x400, &o->fE8);
        t3 = func_80019668();
        c3 = dt << 8;
        o->fE8.y -= ((t3 & 0xF) - 3) << 9;
        func_8001ACA4(&o->fDC, &o->fE8, c3, &o->fDC);
        func_8001ACA4(pp, &o->fDC, c3, pp);
        a = o->f38;
        a.y = a.y + 0x2000;
        f78p = &o->f78;
        q = &o->f78;
        o->f78 = o->fDC;
        func_8001A180(q);
        goto L_ED8;
    }
    case 5:
        if (o->type != 0x25)
            o->f1C = -1;
        return 0;
    default:
        break;
    }

L_AD4: {
    s32 ang;
    s32 dL;
    s32 gL;
    s32 cq;
    Prod pr;
    a.x = b.x - o->pos.x;
    a.y = b.y - o->pos.y;
    a.z = b.z - o->pos.z;
    ang = func_8001A750(&b, &o->pos);
    if (o->type != 0x25) {
        if (o->f116 > 0)
            o->f116 -= dt;
    }
    if ((ang >> 4) < o->f10A || o->f116 > 0) {
        a.x = -a.x;
        a.y = -a.y;
        a.z = -a.z;
        if (o->type != 0x25 && o->f116 <= 0)
            o->f116 = (func_80019668() & 0x7F) + 0x96;
    }
    a.y = 0;
    func_8001A644(&a);

    dL = (dt * o->fF8) >> 3;
    gL = func_80069A38(&o->pos) + (o->f6C << 4);
    func_8001AB78(dL, &a);
    cmp = gL + (o->f114 << 4);
    if (o->pos.y < cmp) {
        if (gL + (o->f110 << 4) >= o->pos.y) {
            dL = 0x1800;
        } else {
            pr.v = (s64)(cmp - o->pos.y) * o->f11C;
            dL = ((s32)((pr.w[1] << 16) | (pr.w[0] >> 16)) * 0x1800) >> 16;
        }
    } else {
        if (o->pos.y >= gL + (o->f112 << 4)) {
            dL = -0x1800;
        } else {
            pr.v = (s64)(cmp - o->pos.y) * o->f118;
            dL = ((s32)((pr.w[1] << 16) | (pr.w[0] >> 16)) * 0x1800) >> 16;
        }
    }
    a.y = (((dL >> 31) + dL) ^ (dL >> 31)) < 0x201 ? 0 : dL;
    func_8001A6F8(&a, &o->fE8);
    o->fE8.y = o->fE8.y - o->fE8.y / 4;
    o->f78.x += a.x;
    o->f78.y += a.y;
    o->f78.z += a.z;
    o->f78.y = o->f78.y - o->f78.y / 8;
    func_8001A644(&o->f78);
    dL = o->f78.y;
    if (dL < -0x8000)
        o->f78.y = -0x8000;
    else if (dL > 0x8000)
        o->f78.y = 0x8000;
    func_8001A180(&o->f78);

    dL = (dt * o->fF4) >> 8;
    cmp = (0x10000 - (((o->f38.y >> 31) + o->f38.y) ^ (o->f38.y >> 31))) * 2;
    dL = (dL * (0x200 - ((cmp - cmp / 16) >> 8))) >> 9;

    FMUL(cq, dL, o->f78.x);
    o->pos.x += cq;
    FMUL(cq, dL, o->f78.y);
    o->pos.y += cq;
    FMUL(cq, dL, o->f78.z);
    o->pos.z += cq;
    if (o->pos.y < gL)
        o->pos.y = gL;

    if (o->type == 0x25 || o->fFA == 0) {
        if (o->fA0 != 0)
            func_80060238(&o->fA0, dt, o, 0, 1);
    }
    a = o->fE8;
    ay = a.y + 0x2000;
}

L_ED0:
    a.y = ay;
L_ED8:
    func_80019310(q, &a, &o->f2C);
    func_80010750(&o->f2C);
    func_80010234((o->f70 << 4) + 8 + o->f28->f34, (u8 *)o + 0x5C, 1);
    return 0;
}
