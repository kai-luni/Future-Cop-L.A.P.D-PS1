#include "game/types.h"

typedef struct V8 {
    s16 x;
    s16 y;
    s16 z;
    s8 a;
    s8 b;
} V8;

typedef struct Mtx {
    s32 m[9];
} Mtx;

typedef struct Cam {
    u8 pad00[0x28];
    Mtx mtx;
    u8 pad4C[0x30];
    s32 px;
    s32 py;
    s32 pz;
} Cam;

typedef struct Prim {
    u32 w0;
    u32 w4;
    u32 w8;
    u32 wC;
    u32 w10;
    u32 w14;
    u32 w18;
    u32 w1C;
    u32 w20;
    u32 w24;
    u32 w28;
    u32 w2C;
    u32 w30;
} Prim;

typedef struct Model {
    s16 *tbl;
    s8 *faces;
    s16 count;
    s16 stride;
    s8 div;
    u8 cnt;
    u8 pad0E[2];
    V8 *verts;
    u8 *flags;
    u8 flg;
} Model;

typedef struct Obj {
    u8 pad[0x24];
    s32 x;
    s32 y;
    s32 z;
} Obj;

extern Cam *D_8009F9E4;
extern Prim *D_8009F9F0;
extern Prim *D_8009FA28;
extern u32 D_8009F9B8;

extern void func_80010820(void *);
extern void func_8001006C(void *, void *, s32);
extern void func_80010750(void *);
extern void func_8001047C(void *, void *, void *, s32);
extern void func_8006CAAC(void *, void *, s16, void *, s16);

void func_8006CB58(Model *model, Obj *obj, s32 size, u8 r, u8 arg_g, u8 arg_b,
                   u8 arg_tr, u8 arg_tg, u8 arg_tb)
{
    struct {
        s32 vin[4];
        Mtx mtx;
        s32 vout[3];
        union {
            u32 w;
            u8 b[4];
        } col;
        u16 cur[3];
        u16 dlt[3];
        V8 v[16];
    } L;
    register s32 g __asm__("$14") = arg_g;
    register s32 b __asm__("$15") = arg_b;
    register s32 tr __asm__("$11") = arg_tr;
    register s32 tg __asm__("$12") = arg_tg;
    register s32 tb __asm__("$13") = arg_tb;
    register s32 two __asm__("$3");
    register s32 tc __asm__("$10");
    register V8 *p0 __asm__("$9");
    register V8 *p1 __asm__("$2");
    register s32 n __asm__("$17");
    register s32 i __asm__("$18");
    register s32 j __asm__("$12");
    register s32 c __asm__("$14");
    register s32 neg __asm__("$8");
    register u8 flag __asm__("$21");
    register u32 hi __asm__("$22");
    register u32 lo __asm__("$20");
    register s8 *f __asm__("$8");
    s16 *dummy;
    register u8 *fl __asm__("$11");
    register V8 *vp __asm__("$7");
    register V8 *q0 __asm__("$2");
    register V8 *q1 __asm__("$3");
    register V8 *q2 __asm__("$4");
    register V8 *q3 __asm__("$5");
    register V8 *qlast __asm__("$2");
    register s32 o0 __asm__("$2");
    register s32 o1 __asm__("$3");
    register s32 o2 __asm__("$4");
    register s32 o3 __asm__("$5");
    register s32 countdown __asm__("$3");
    register s32 depth __asm__("$4");
    s32 stride;
    register u32 mask __asm__("$16");
    register u32 pkthi __asm__("$24");
    register u32 masked __asm__("$15");
    register u32 quad __asm__("$13");
    register u32 old __asm__("$7");
    register s8 *facebase __asm__("$3");
    s32 m0;
    register Mtx *mat __asm__("$16");
    register V8 *iter __asm__("$16");
    register s32 cr __asm__("$4");
    register s32 cg __asm__("$5");
    register s32 cb __asm__("$6");
    register s32 tmp __asm__("$2");

    register Model *mp __asm__("$19") = model;
    register Obj *op __asm__("$18") = obj;

    model = mp; obj = op;
    __asm__("" : "+r"(mp), "+r"(op));
    flag = 0;
    n = 2;
    p0 = &L.v[0];
    __asm__("" : "+r"(p0));
    two = n;
    __asm__("" : "+r"(two));
    p1 = &L.v[1];
    __asm__("" : "+r"(p1));
    __asm__("" : "+r"(g), "+r"(b), "+r"(tr), "+r"(tg), "+r"(tb));
    neg = -size;
    __asm__("" : "+r"(neg));
    L.v[0].x = size; p0->y = size; p0->z = size; p0->a = 0; p0->b = two;
    L.v[1].x = size; p1->y = size; p1->z = size; p1->a = two; p1->b = 0;
    p1 = &L.v[2];
    __asm__("" : "+r"(p1));
    L.v[2].x = neg; p1->b = n; p1->y = size; p1->z = size; p1->a = 0;
    p1 = &L.v[3];
    __asm__("" : "+r"(p1));
    __asm__("addiu %0,$0,2" : "=r"(tc));
    L.v[3].x = neg; p1->y = size; p1->z = size; p1->a = tc; p1->b = 0;
    n = 4;
    if (!(mp->flg & 4)) {
        n = 8;
        p1 = &L.v[4];
    __asm__("" : "+r"(p1));
    L.v[4].x = size; p1->y = size; p1->z = neg; p1->a = 0; p1->b = tc;
        p1 = &L.v[5];
    __asm__("" : "+r"(p1));
    L.v[5].x = neg; p1->y = size; p1->z = neg; p1->a = 0; p1->b = tc;
        p1 = &L.v[6];
    __asm__("" : "+r"(p1));
    L.v[6].x = size; p1->y = size; p1->z = neg; p1->a = tc; p1->b = 0;
        p1 = &L.v[7];
    __asm__("" : "+r"(p1));
    L.v[7].x = neg; p1->y = size; p1->z = neg; p1->a = tc; p1->b = 0;
    }
    if (!(mp->flg & 2)) {
        o0 = n++ << 3; o1 = n++ << 3; o2 = n++ << 3;
        __asm__("" : "+r"(o0), "+r"(o1), "+r"(o2));
        q0 = (V8 *)((u8 *)p0 + o0);
        q1 = (V8 *)((u8 *)p0 + o1);
        q2 = (V8 *)((u8 *)p0 + o2);
        q0->x = size; q0->y = neg; q0->z = size; q0->a = 0; q0->b = tc;
        qlast = p0 + n;
        q1->x = size; q1->y = neg; q1->z = size; q1->a = tc; q1->b = 0;
        q2->x = neg; q2->y = neg; q2->z = size; q2->a = 0; q2->b = tc;
        qlast->x = neg; qlast->y = neg; qlast->z = size; qlast->a = tc; qlast->b = 0;
        n++;
        if (!(mp->flg & 4)) {
            o0 = n++ << 3; o1 = n++ << 3; o2 = n++ << 3; o3 = n++ << 3;
            __asm__("" : "+r"(o0), "+r"(o1), "+r"(o2), "+r"(o3));
            q0 = (V8 *)((u8 *)p0 + o0);
            q1 = (V8 *)((u8 *)p0 + o1);
            q2 = (V8 *)((u8 *)p0 + o2);
            q3 = (V8 *)((u8 *)p0 + o3);
            q0->x = size; q0->y = neg; q0->z = neg; q0->a = 0; q0->b = tc;
            q1->x = neg; q1->y = neg; q1->z = neg; q1->a = 0; q1->b = tc;
            q2->x = size; q2->y = neg; q2->z = neg; q2->a = tc; q2->b = 0;
            q3->x = neg; q3->y = neg; q3->z = neg; q3->a = tc; q3->b = 0;
        }
    }

    cr = r;
    tmp = cr << 4;
    cg = (u8)g;
    L.cur[0] = tmp;
    tmp = cg << 4;
    cb = (u8)b;
    L.cur[1] = tmp;
    tmp = cb << 4;
    L.col.b[1] = r;
    L.col.b[2] = g;
    L.col.b[3] = b;
    L.cur[2] = tmp;
    switch (mp->div) {
    case 8:
        tmp = (tr - cr) << 1; L.dlt[0] = tmp;
        tmp = (tg - cg) << 1; L.dlt[1] = tmp;
        tmp = (tb - cb) << 1; L.dlt[2] = tmp;
        break;
    case 2:
        tmp = (tr - cr) << 3; L.dlt[0] = tmp;
        tmp = (tg - cg) << 3; L.dlt[1] = tmp;
        tmp = (tb - cb) << 3; L.dlt[2] = tmp;
        break;
    case 4:
        tmp = (tr - cr) << 2; L.dlt[0] = tmp;
        tmp = (tg - cg) << 2; L.dlt[1] = tmp;
        tmp = (tb - cb) << 2; L.dlt[2] = tmp;
        break;
    default:
        tmp = (tr - (u8)r) << 4; tmp = tmp / mp->div; L.dlt[0] = tmp;
        tmp = (tg - g) << 4; tmp = tmp / mp->div; L.dlt[1] = tmp;
        tmp = (tb - b) << 4; tmp = tmp / mp->div; L.dlt[2] = tmp;
        break;
    }

    L.vin[0] = op->x - D_8009F9E4->px;
    L.vin[1] = op->y - D_8009F9E4->py;
    L.vin[2] = op->z - D_8009F9E4->pz;
    func_80010820(&D_8009F9E4->mtx);
    func_8001006C(L.vin, L.vout, 1);
    mat = &L.mtx;
    if (mp->flg & 8) {
        func_80010820(&D_8009F9E4->mtx);
        func_8001006C(op, mat, 3);
    } else {
        tmp = D_8009F9E4->mtx.m[1];
        m0 = D_8009F9E4->mtx.m[0];
        L.mtx.m[1] = tmp;
        L.mtx.m[2] = D_8009F9E4->mtx.m[2];
        L.mtx.m[3] = D_8009F9E4->mtx.m[3];
        L.mtx.m[4] = D_8009F9E4->mtx.m[4];
        L.mtx.m[5] = D_8009F9E4->mtx.m[5];
        L.mtx.m[6] = D_8009F9E4->mtx.m[6];
        L.mtx.m[7] = D_8009F9E4->mtx.m[7];
        L.mtx.m[8] = D_8009F9E4->mtx.m[8];
        L.mtx.m[0] = m0;
    }
    i = 0;
    if (n > 0) {
        iter = L.v;
        do {
            vp = iter;
            iter++;
            func_8006CAAC(mp->verts, mp->tbl, mp->stride, vp, i++);
        } while (i < n);
    }
    func_80010750(&L.mtx);
    func_8001047C(mp->verts, mp->verts, mp->flags, mp->stride * n);

    c = 0;
    if (mp->flg & 1) {
        countdown = mp->cnt;
        tmp = (u8)mp->div;
        c = countdown * tmp;
    }
    i = 0;
    if (mp->count > 0) {
        mask = 0xFFFFFF;
        pkthi = 0x3E000000;
        __asm__("" : : "r"(pkthi));
        do {
        tmp = c - 1;
        c = tmp;
        __asm__("" : "+r"(tmp));
        if ((s8)tmp < 0) {
            flag = 1;
            countdown = mp->cnt;
            cb = L.cur[0];
            tmp = L.dlt[0];
            old = L.col.w;
            cg = L.cur[1];
            cr = L.dlt[2];
            c = countdown - 1;
            cb += tmp;
            countdown = L.dlt[1];
            tmp = (u32)cb >> 4;
            L.col.b[1] = tmp;
            cg += countdown;
            countdown = L.cur[2];
            tmp = (u32)cg >> 4;
            L.col.b[2] = tmp;
            countdown += cr;
            tmp = (u32)countdown >> 4;
            L.col.b[3] = tmp;
            tmp = L.col.w;
            hi = old >> 8;
            L.cur[0] = cb;
            L.cur[1] = cg;
            L.cur[2] = countdown;
            lo = (u32)tmp >> 8;
        }
        if (flag) {
            vp = mp->verts;
            tmp = i << 2;
            j = 0;
            __asm__("" : "+r"(j) : "r"(tmp));
            tmp += i;
            facebase = mp->faces;
            f = facebase + tmp;
            fl = mp->flags;
            if (n > 0) {
                masked = hi & mask;
                quad = masked | pkthi;
                __asm__("" : : "r"(quad));
                do {
                register s32 i1 __asm__("$6");
                register s32 i2 __asm__("$5");
                register s32 i3 __asm__("$9");
                register s32 i4 __asm__("$10");
                u32 fv;
                register s16 m __asm__("$6");
                register V8 *d1 __asm__("$2");
                register V8 *d2 __asm__("$4");
                register s32 z1 __asm__("$5");
                register s32 z2 __asm__("$3");
                register Prim *p __asm__("$4");
                register Prim *raw __asm__("$3");
                register u32 tex __asm__("$3");
                if (f[0] == 4) {
                    i1 = f[1]; i2 = f[2]; i3 = f[3]; i4 = f[4];
                    __asm__("" : "+r"(i1), "+r"(i2), "+r"(i3), "+r"(i4));
                    fv = fl[i1] | fl[i2] | fl[i3] | fl[i4];
                    if (fv & 1) goto next;
                    tmp = 0xC0;
                    if (fv != tmp) goto next;
                    d2 = (V8 *)((i2 << 3) + (s32)vp);
                    d1 = (V8 *)((i1 << 3) + (s32)vp);
                    z1 = d1->z;
                    z2 = d2->z;
                    m = (u16)d1->z;
                    tmp = (u16)d2->z;
                    if (z2 < z1) m = tmp;
                    d1 = (V8 *)((i3 << 3) + (s32)vp);
                    z2 = (s16)m;
                    depth = d1->z;
                    tmp = (u16)d1->z;
                    depth = depth < z2;
                    if (depth) m = tmp;
                    d1 = (V8 *)((i4 << 3) + (s32)vp);
                    z2 = (s16)m;
                    depth = d1->z;
                    tmp = (u16)d1->z;
                    depth = depth < z2;
                    if (depth) m = tmp;
                    raw = D_8009F9F0;
                    if (raw >= D_8009FA28) {
                        p = raw;
                        __asm__("" : "+r"(p));
                    } else {
                        p = raw;
                        D_8009F9F0 = p + 1;
                    }
                    ((u8 *)p)[3] = 0xC;
                    p->w1C = quad;
                    p->w4 = quad;
                    p->w28 = lo;
                    p->w10 = lo;
                    p->w8 = *(u32 *)&vp[f[1]];
                    p->w14 = *(u32 *)&vp[f[2]];
                    p->w2C = *(u32 *)&vp[f[3]];
                    __asm__ volatile ("" : : : "memory");
                    tmp = *(u32 *)&vp[f[4]];
                    p->wC = 0;
                    p->w20 = tmp;
                    __asm__ volatile ("" : : : "memory");
                    tex = D_8009F9B8;
                    tmp = 0x101; *(u16 *)&p->w24 = tmp;
                    tmp = 0x100; *(u16 *)&p->w30 = tmp;
                    __asm__ volatile ("" : "+r"(tex) : : "memory");
                    p->w18 = (tex << 16) | 1;
                } else {
                    i1 = f[1]; i2 = f[2]; i3 = f[3];
                    __asm__("" : "+r"(i1), "+r"(i2), "+r"(i3));
                    fv = fl[i1] | fl[i2] | fl[i3];
                    if (fv & 1) goto next;
                    tmp = 0xC0;
                    if (fv != tmp) goto next;
                    d2 = (V8 *)((i2 << 3) + (s32)vp);
                    d1 = (V8 *)((i1 << 3) + (s32)vp);
                    z1 = d1->z;
                    z2 = d2->z;
                    m = (u16)d1->z;
                    tmp = (u16)d2->z;
                    if (z2 < z1) m = tmp;
                    d1 = (V8 *)((i3 << 3) + (s32)vp);
                    z2 = (s16)m;
                    depth = d1->z;
                    tmp = (u16)d1->z;
                    depth = depth < z2;
                    if (depth) m = tmp;
                    raw = D_8009F9F0;
                    if (raw >= D_8009FA28) {
                        p = raw;
                        __asm__("" : "+r"(p));
                    } else {
                        p = raw;
                        D_8009F9F0 = p + 1;
                    }
                    tmp = 9;
                    ((u8 *)p)[3] = tmp;
                    __asm__ volatile ("" : : : "memory");
                    tmp = masked | 0x36000000;
                    p->w4 = tmp;
                    p->w1C = lo;
                    p->w10 = lo;
                    p->w8 = *(u32 *)&vp[f[1]];
                    p->w14 = *(u32 *)&vp[f[2]];
                    tmp = *(u32 *)&vp[f[3]];
                    p->wC = 0;
                    p->w20 = tmp;
                    __asm__ volatile ("" : : : "memory");
                    tmp = D_8009F9B8;
                    *(u16 *)&p->w24 = 1;
                    p->w18 = (tmp << 16) | 0x101;
                }
                tex = p->w0;
                tmp = 0xFF000000;
                tex = tex & tmp;
                __asm__("" : "+r"(tex) : "r"(tmp));
                tmp = (s16)m;
                tmp = tmp - 0x100;
                tmp = tmp & mask;
                tex = tex | tmp;
                __asm__("" : "+r"(tex));
                p->w0 = tex;
            next:
                tmp = mp->stride;
                z2 = tmp << 3;
                fl += tmp;
                vp = (V8 *)((s32)vp + z2);
                } while (++j < n);
            }
        }
        } while (++i < mp->count);
    }
}
