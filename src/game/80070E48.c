#include "game/types.h"

typedef struct V3 {
    s32 x;
    s32 y;
    s32 z;
} V3;

typedef struct S3 {
    s16 x;
    s16 y;
    s16 z;
} S3;

typedef struct Msg {
    s32 f0;
    s16 f4;
    u8 f6;
    u8 pad7;
    s32 f8;
    s32 padC;
} Msg;

typedef struct Loc { V3 v; V3 arr[3] __attribute__((aligned(8))); V3 q[5] __attribute__((aligned(8))); V3 w[5] __attribute__((aligned(8))); } Loc;
typedef union MU { Msg m; V3 t; } MU;
typedef struct Dev Dev;
struct Dev {
    u8 pad0[4];
    void (*fn)(s32, Dev *, Msg *, s32);
    u8 pad8[0xE];
    u8 type;
    u8 pad17;
    u16 flags;
};

typedef struct Obj {
    u8 pad0[4];
    Dev *sub;
    u8 pad8[0x10];
    u16 flags;
} Obj;

typedef struct Entry {
    V3 *verts;
    S3 *shorts;
    s16 count;
    u16 padA;
    u8 r;
    u8 g;
    u8 b;
    u8 padF;
} Entry;

typedef struct Node {
    void *next;
    Obj *a;
    Obj *b;
    s16 count;
    u16 flags;
    Entry *entries;
    s16 field14;
    u16 field16;
    u16 field18;
    u8 r;
    u8 g;
    u8 bl;
    u8 pad1D;
    s16 field1E;
    s16 field20;
    s16 field22;
    s16 field24;
} Node;

extern Dev *D_8009FA58[];

extern s16 func_80070730(Node *node, Obj *obj, V3 *out, void *arg);
extern s32 func_80019668(void);
extern s32 func_8001A314(V3 *v);

void func_80070E48(Node *node, Entry *e, s32 mode)
{
    Loc L;
    V3 acc1;
    V3 acc2;
    V3 pos;
    V3 off;
    MU ma;
    MU mb;
    MU mc;
    s32 step;
    register s32 s0 __asm__("$16");
    s32 i;
    s32 j;
    s16 idx;
    register s32 h __asm__("$2");
    s32 scale;
    s32 rem;
    s32 len;
    register s32 p1 __asm__("$12");
    register s32 r __asm__("$2");
    register s32 p2 __asm__("$11");
    s32 p3;

    e->r = node->r;
    e->g = node->g;
    e->b = node->bl;

    if (node->field24 > 0) {
        if (node->a != 0 && node->a->sub != 0 && (node->a->flags & 4)) {
            ma.m.f0 = 0;
            ma.m.f6 = 8;
            if (node->a->sub->type == 2 || node->a->sub->type == 15 || node->a->sub->type == 7) {
                ma.m.f4 = 1000;
            } else {
                ma.m.f4 = node->field24;
            }
            ma.m.f8 = 0;
            node->a->sub->fn(14, node->a->sub, &ma.m, 0);
        }
        if (node->b != 0 && node->b->sub != 0 && (node->b->flags & 4)) {
            mb.m.f0 = 0;
            mb.m.f6 = 8;
            if (node->b->sub->type == 2 || node->b->sub->type == 15 || node->b->sub->type == 7) {
                mb.m.f4 = 1000;
            } else {
                mb.m.f4 = node->field24;
            }
            mb.m.f8 = 0;
            node->b->sub->fn(14, node->b->sub, &mb.m, 0);
        }
    }

    if ((u8)mode) {
        idx = func_80070730(node, node->a, e->verts, 0);
        if (idx != -1) {
            ma.m.f0 = 0;
            ma.m.f6 = 8;
            {
                u32 base;
                u16 damage = node->field24;
                __asm__("lui\t%0,%%hi(D_8009FA58)\n\taddiu\t%0,%0,%%lo(D_8009FA58)" : "=r"(base));
                ma.m.f4 = damage;
                ma.m.f8 = 0;
                ((Dev **)base)[idx]->fn(14, ((Dev **)base)[idx], &ma.m, 0);
            }
        }
        idx = func_80070730(node, node->b, e->verts + e->count, e->verts);
        if (idx != -1) {
            mc.m.f0 = 0;
            mc.m.f6 = 8;
            {
                u32 base;
                u16 damage = node->field24;
                __asm__("lui\t%0,%%hi(D_8009FA58)\n\taddiu\t%0,%0,%%lo(D_8009FA58)" : "=r"(base));
                mc.m.f4 = damage;
                mc.m.f8 = 0;
                ((Dev **)base)[idx]->fn(14, ((Dev **)base)[idx], &mc.m, 0);
            }
        }
    }
    h = node->field16;
    __asm__ volatile("" : "+r"(h));
    h = h << 16;
    s0 = h >> 23;
    __asm__ volatile("");
    __asm__ volatile("");
    L.v.x = e->verts[e->count].x - e->verts[0].x;
    L.v.y = e->verts[e->count].y - e->verts[0].y;
    L.v.z = e->verts[e->count].z - e->verts[0].z;
    len = func_8001A314(&L.v);
    scale = (len >> 4) / e->count;
    __asm__ ("" : : "r"(len >> 4));
    j = 1;
    L.arr[0].x = L.v.x >> 4;
    L.arr[0].y = L.v.y >> 4;
    L.arr[0].z = L.v.z >> 4;
    p1 = L.arr[0].x * scale;
    p2 = L.arr[0].y * scale;
    p3 = L.arr[0].z * scale;
    r = p1 >> 9;
    L.arr[0].x = r;
    r = p2 >> 9;
    L.arr[0].y = r;
    r = p3 >> 9;
    L.arr[0].z = r;
    {
        s32 *pz = &L.arr[0].z;
        s32 *py = &L.arr[0].y;
        s32 *px = &L.arr[0].x;
        for (; j < 3; j++) {
            L.arr[j].x = L.arr[0].x + *px;
            L.arr[j].y = L.arr[0].y + *py;
            L.arr[j].z = L.arr[0].z + *pz;
            px += 3;
            py += 3;
            pz += 3;
        }
    }

    {
        s32 ax = L.q[1].x = L.v.x < 0 ? -L.v.x : L.v.x;
        s32 ay = L.q[1].y = L.v.y < 0 ? -L.v.y : L.v.y;
        s32 az = L.q[1].z = L.v.z < 0 ? -L.v.z : L.v.z;
        if (ax <= 0xCCCB) {
            L.q[0].x = 0;
            L.q[0].y = -L.v.z;
            L.q[0].z = L.v.y;
        } else if (ay <= 0xCCCB) {
            L.q[0].x = L.v.z;
            L.q[0].y = 0;
            L.q[0].z = -L.v.x;
        } else if (az <= 0xCCCB) {
            L.q[0].x = -L.v.y;
            L.q[0].y = L.v.x;
            L.q[0].z = 0;
        }
    }

    L.w[0].x = ((L.v.z * (L.q[0].y >> 4)) >> 12) - ((L.v.y * (L.q[0].z >> 4)) >> 12);
    L.w[0].y = ((L.v.x * (L.q[0].z >> 4)) >> 12) - ((L.v.z * (L.q[0].x >> 4)) >> 12);
    L.w[0].z = ((L.v.y * (L.q[0].x >> 4)) >> 12) - ((L.v.x * (L.q[0].y >> 4)) >> 12);

    L.q[0].x = -((L.q[0].x * s0) >> 12);
    L.q[0].y = -((L.q[0].y * s0) >> 12);
    L.q[0].z = -((L.q[0].z * s0) >> 12);
    off.x = -(L.q[0].x >> 1);
    off.y = -(L.q[0].y >> 1);
    off.z = -(L.q[0].z >> 1);
    {
        s32 *pz = &L.q[0].z;
        s32 *py = &L.q[0].y;
        s32 *px = &L.q[0].x;
        for (j = 1; j < 5; j++) {
            L.q[j].x = off.x + *px;
            L.q[j].y = off.y + *py;
            L.q[j].z = off.z + *pz;
            px += 3;
            py += 3;
            pz += 3;
        }
    }

    j = 1;
    L.w[0].x = -((L.w[0].x * s0) >> 12);
    L.w[0].y = -((L.w[0].y * s0) >> 12);
    L.w[0].z = -((L.w[0].z * s0) >> 12);
    off.x = -(L.w[0].x >> 1);
    off.y = -(L.w[0].y >> 1);
    off.z = -(L.w[0].z >> 1);
    {
        s32 *pz = &L.w[0].z;
        s32 *py = &L.w[0].y;
        s32 *px = &L.w[0].x;
        for (; j < 5; j++) {
            L.w[j].x = off.x + *px;
            L.w[j].y = off.y + *py;
            L.w[j].z = off.z + *pz;
            px += 3;
            py += 3;
            pz += 3;
        }
    }

    pos = e->verts[0];
    acc1.x = 0;
    acc1.y = 0;
    acc1.z = 0;
    acc2.x = 0;
    acc2.y = 0;
    acc2.z = 0;
    rem = 0x1000;
    step = 0x1000 / e->count;
    for (i = 1; i < e->count; i++) {
        s32 i0 = (u32)func_80019668() % 5;
        s32 i1 = (u32)func_80019668() % 5;
        s32 i2 = (u32)func_80019668() % 3;

        acc1.x += ((Loc *)((s32)&L + i0 * 12))->q[0].x;
        acc1.y += ((Loc *)((s32)&L + i0 * 12))->q[0].y;
        acc1.z += ((Loc *)((s32)&L + i0 * 12))->q[0].z;
        acc2.x += ((Loc *)((s32)&L + i1 * 12))->w[0].x;
        acc2.y += ((Loc *)((s32)&L + i1 * 12))->w[0].y;
        acc2.z += ((Loc *)((s32)&L + i1 * 12))->w[0].z;
        ma.t.x = (((acc1.x + acc2.x) >> 4) * rem) >> 8;
        ma.t.y = (((acc1.y + acc2.y) >> 4) * rem) >> 8;
        ma.t.z = (((acc1.z + acc2.z) >> 4) * rem) >> 8;
        e->verts[i].x = pos.x + ((Loc *)((s32)&L + i2 * 12))->arr[0].x;
        e->verts[i].y = pos.y + ((Loc *)((s32)&L + i2 * 12))->arr[0].y;
        e->verts[i].z = pos.z + ((Loc *)((s32)&L + i2 * 12))->arr[0].z;
        e->verts[i].x += ma.t.x;
        e->verts[i].y += ma.t.y;
        e->verts[i].z += ma.t.z;
        pos.x += L.arr[1].x;
        pos.y += L.arr[1].y;
        pos.z += L.arr[1].z;
        rem -= step;
        {
            s32 f14 = node->field14;
            if (f14 != 0) {
                e->shorts[i].x = ((ma.t.x >> 4) * f14) >> 12;
                e->shorts[i].y = ((ma.t.y >> 4) * f14) >> 12;
                e->shorts[i].z = ((ma.t.z >> 4) * f14) >> 12;
            }
        }
    }
}
