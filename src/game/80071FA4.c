#include "game/types.h"

typedef struct Ent {
    s32 *verts;
    s32 pad4;
    s16 cnt;
    s16 pad0A;
    u8 r;
    u8 g;
    u8 b;
    u8 padF;
} Ent;

typedef struct Sub {
    u8 pad0[0x1C];
    s16 f1C;
    u8 pad1E[0xA];
    s32 f28;
    s32 f2C;
    s32 f30;
    s32 f34;
    s32 f38;
    s32 f3C;
} Sub;

typedef struct Obj {
    Sub *sub;
    u8 pad4[0x18];
    s16 f1C;
} Obj;

typedef struct Node {
    s32 pad0;
    Obj *a;
    Obj *b;
    s16 count;
    u16 flags;
    Ent *ents;
} Node;

typedef struct Mtx {
    s32 m[12];
} Mtx;

typedef struct Cam {
    u8 pad0[0x28];
    Mtx mtx;
    s32 pad58;
    s32 pad5C;
    s32 pad60;
    s32 pad64;
    s32 pad68;
    s32 pad6C;
    s32 pad70;
    s32 pad74;
    s32 pad78;
    s32 ox;
    s32 oy;
    s32 oz;
} Cam;

typedef struct Prim {
    u32 w0;
    u8 r0, g0, b0, c0;
    s16 x0, y0;
    u32 w3;
    u8 r1, g1, b1, p1;
    s16 x1, y1;
    u32 w6;
    u8 r2, g2, b2, p2;
    s16 x2, y2;
    s16 t24, p26;
    u8 r3, g3, b3, p3;
    s16 x3, y3;
    s16 t30;
} Prim;

extern s32 func_8001A100(s32 *v);
extern void func_80010820(void *m);
extern void func_80010750(void *m);
extern void func_8001006C(void *a, void *b, s32 c);
extern void func_8001047C(void *a, void *b, void *c, s32 n);

extern s16 D_800A6418[][4];
extern u8 D_800AB388[];
extern u8 D_800AB389[];
extern s16 D_800A7840[];
extern s16 D_800A7898[];
extern Cam *D_8009F9E4;
extern Prim *D_8009F9F0;
extern Prim *D_8009FA28;
extern s32 D_8009F9B8;

#define ALLOC(p) \
    p = D_8009F9F0; \
    if (D_8009F9F0 < D_8009FA28) D_8009F9F0 = (Prim *)((u8 *)D_8009F9F0 + 0x34)

#define ALLOC_T(p) p = (D_8009F9F0 >= D_8009FA28) ? D_8009F9F0 : D_8009F9F0++
#define ALLOC_U(p) p = (D_8009F9F0 < D_8009FA28) ? D_8009F9F0++ : D_8009F9F0
#define ALLOC_A(p) ALLOC(p)

#define ALLOC_REG(p, reg) do { \
    register Prim *cur __asm__(reg) = D_8009F9F0; \
    p = cur; if (cur < D_8009FA28) { D_8009F9F0 = (Prim *)((u8 *)p + 0x34); } \
} while (0)

#define SETW0(p, v) (p)->w0 = ((p)->w0 & 0xFF000000) | ((s16)(v) & 0xFFFFFF)

#define INB(x, y) ((x) < 0x140 && (y) < 0xF0)

void func_80071FA4(Node *node, s32 *org)
{
    Mtx m;
    s32 v[3];
    s32 i, j;

    if (node->flags & 0x4000) return;

    for (i = 0; i < node->count; i++) {
        v[0] = node->ents[i].verts[0] - org[0];
        v[1] = node->ents[i].verts[1] - org[1];
        v[2] = node->ents[i].verts[2] - org[2];
        if (node->flags & 8) {
            v[0] += node->a->sub->f34;
            v[1] += node->a->sub->f38;
            v[2] += node->a->sub->f3C;
        }
        if (func_8001A100(v) > 0x1FFFF) {
            v[0] = node->ents[i].verts[node->ents[i].cnt * 3 + 0] - org[0];
            v[1] = node->ents[i].verts[node->ents[i].cnt * 3 + 1] - org[1];
            v[2] = node->ents[i].verts[node->ents[i].cnt * 3 + 2] - org[2];
            if (node->flags & 8) {
                v[0] += node->a->sub->f34;
                v[1] += node->a->sub->f38;
                v[2] += node->a->sub->f3C;
            }
            if (func_8001A100(v) > 0x1FFFF) goto next_node;
        }

        {
            s32 prep;
        if (node->flags & 8) {
            for (prep = 0; prep < node->ents[i].cnt + 1; prep++) {
                D_800A6418[prep][0] = node->ents[i].verts[prep * 3 + 0] >> 4;
                D_800A6418[prep][1] = node->ents[i].verts[prep * 3 + 1] >> 4;
                D_800A6418[prep][2] = node->ents[i].verts[prep * 3 + 2] >> 4;
            }
        } else {
            for (prep = 0; prep < node->ents[i].cnt + 1; prep++) {
                D_800A6418[prep][0] = (node->ents[i].verts[prep * 3 + 0] - D_8009F9E4->ox) >> 4;
                D_800A6418[prep][1] = (node->ents[i].verts[prep * 3 + 1] - D_8009F9E4->oy) >> 4;
                D_800A6418[prep][2] = (node->ents[i].verts[prep * 3 + 2] - D_8009F9E4->oz) >> 4;
            }
        }

        }

        if (node->flags & 8) {
            v[0] = node->a->sub->f28 - D_8009F9E4->ox;
            v[1] = node->a->sub->f2C - D_8009F9E4->oy;
            v[2] = node->a->sub->f30 - D_8009F9E4->oz;
            func_80010820(&D_8009F9E4->mtx);
            func_8001006C((u8 *)node->a->sub + 4, &m, 3);
            func_80010820(&D_8009F9E4->mtx);
            func_8001006C(v, &m.m[9], 1);
        } else {
            s32 first;
            first = D_8009F9E4->mtx.m[0];
            m.m[1] = D_8009F9E4->mtx.m[1];
            m.m[2] = D_8009F9E4->mtx.m[2];
            m.m[3] = D_8009F9E4->mtx.m[3];
            m.m[4] = D_8009F9E4->mtx.m[4];
            m.m[5] = D_8009F9E4->mtx.m[5];
            m.m[6] = D_8009F9E4->mtx.m[6];
            m.m[7] = D_8009F9E4->mtx.m[7];
            m.m[8] = D_8009F9E4->mtx.m[8];
            m.m[9] = D_8009F9E4->mtx.m[9];
            m.m[10] = D_8009F9E4->mtx.m[10];
            m.m[11] = D_8009F9E4->mtx.m[11];
            m.m[0] = first;
        }
        if (node->flags & 8) func_80010750(&m);
        else func_80010820(&m);

        func_8001047C(D_800A6418, D_800A6418, D_800AB388, node->ents[i].cnt + 1);

        {
            s32 a3 = 0, a2 = 1;
            s32 t3 = node->a->f1C << 4;
            s32 step = ((node->b->f1C - node->a->f1C) << 4) / (node->ents[i].cnt + 1);
            for (j = 0; j < node->ents[i].cnt + 1; j++) {
                if (!((D_800AB388[j] | D_800AB388[a3] | D_800AB388[a2]) & 1)) {
                    s32 sc = ((t3 >> 4) << 10) / D_800A6418[a3][2];
                    s32 ax, ay, t, len, rawax;
                    s32 raway;
                    s32 product;
                    if (sc < 3) sc = 3;
                    D_800A7840[j] = D_800A6418[a2][1] - D_800A6418[a3][1];
                    D_800A7898[j] = D_800A6418[a3][0] - D_800A6418[a2][0];
                    rawax = D_800A7840[j];
                    ax = rawax;
                    if (ax < 0) ax = -ax;
                    ay = D_800A7898[j];
                    if (ay < 0) ay = -ay;
                    if (ax < ay) { ax ^= ay; ay ^= ax; ax ^= ay; }
                    if ((ax >> 1) < ay) {
                        t = ax + ay;
                        t = (t << 6) + (t << 3) + (t << 1) + ax + (ay << 5);
                        ay = ax - ay;
                        ax = t >> 7;
                    }
                    len = ax + (ay >> 2) - (ay >> 6);
                    if (len > 0) {
                        sc = (sc << 10) / len;
                        product = rawax * sc;
                        D_800A7840[j] = product >> 10;
                        raway = D_800A7898[j];
                        product = raway * sc;
                        D_800A7898[j] = product >> 10;
                    } else {
                        D_800A7898[j] = 0;
                        D_800A7840[j] = 0;
                    }
                }
                a2 = j + 2;
                a3 = j;
                if (node->ents[i].cnt < a2) a2 = node->ents[i].cnt;
                t3 += step;
                if (t3 < 0) t3 = 0;
            }
        }

        {
        for (j = 0; j < node->ents[i].cnt; j++) {
            Prim *p;
            s32 z;
            if ((D_800AB388[j] | D_800AB388[j + 1]) & 1) continue;
            z = ((D_800A6418[j][2] + D_800A6418[j + 1][2]) >> 1) - 0x100;
            if (D_800A7840[j] == 0 && D_800A7898[j] == 0) {
                s16 dx = D_800A6418[j + 1][0] - D_800A6418[j][0];
                s16 dy = D_800A6418[j + 1][1] - D_800A6418[j][1];
                if (dx < 0) {
                    if (dy < 0) {
                        if (dx < dy) D_800A6418[j][0]--; else D_800A6418[j][1]--;
                    } else if (dy < -dx) D_800A6418[j][0]--; else D_800A6418[j][1]++;
                } else {
                    if (dy < 0) {
                        if (-dy < dx) D_800A6418[j][0]++; else D_800A6418[j][1]--;
                    } else if (dy < dx) D_800A6418[j][0]++; else D_800A6418[j][1]++;
                }
                ALLOC_U(p);
                ((u8 *)p)[3] = 1;
                ((u32 *)p)[1] = 0xE10006A0;
                SETW0(p, z);
                ALLOC_U(p);
                p->c0 = 0x42;
                SETW0(p, z);
                ((u8 *)p)[3] = 3;
                p->r0 = node->ents[i].r;
                p->g0 = node->ents[i].g;
                p->b0 = node->ents[i].b;
                p->x0 = D_800A6418[j][0];
                p->y0 = D_800A6418[j][1];
                ((s16 *)p)[6] = D_800A6418[j + 1][0];
                ((s16 *)p)[7] = D_800A6418[j + 1][1];
            } else {
                ALLOC_T(p);
                p->x0 = D_800A6418[j][0];
                p->y0 = D_800A6418[j][1];
                p->x1 = D_800A6418[j][0] - D_800A7840[j];
                p->y1 = D_800A6418[j][1] - D_800A7898[j];
                p->x3 = D_800A6418[j + 1][0] - D_800A7840[j + 1];
                p->y3 = D_800A6418[j + 1][1] - D_800A7898[j + 1];
                p->x2 = D_800A6418[j + 1][0];
                p->y2 = D_800A6418[j + 1][1];
                if (INB(p->x0, p->y0) || INB(p->x1, p->y1) || INB(p->x2, p->y2) || INB(p->x3, p->y3)) {
                    ((u8 *)p)[3] = 0xC;
                    p->c0 = 0x3E;
                    p->r0 = node->ents[i].r;
                    p->g0 = node->ents[i].g;
                    p->b0 = node->ents[i].b;
                    p->r1 = 0; p->g1 = 0; p->b1 = 0;
                    p->r3 = 0; p->g3 = 0; p->b3 = 0;
                    p->r2 = node->ents[i].r;
                    p->g2 = node->ents[i].g;
                    p->b2 = node->ents[i].b;
                    p->w3 = 0;
                    p->t24 = 0x101;
                    p->t30 = 0x100;
                    p->w6 = (D_8009F9B8 << 16) | 1;
                    SETW0(p, z);
                } else {
                    goto reject_segment;
                }
                ALLOC_T(p);
                p->x0 = D_800A6418[j][0];
                p->y0 = D_800A6418[j][1];
                p->x1 = D_800A6418[j + 1][0];
                p->y1 = D_800A6418[j + 1][1];
                p->x3 = D_800A6418[j + 1][0] + D_800A7840[j + 1];
                p->y3 = D_800A6418[j + 1][1] + D_800A7898[j + 1];
                p->x2 = D_800A6418[j][0] + D_800A7840[j];
                p->y2 = D_800A6418[j][1] + D_800A7898[j];
                if (!(INB(p->x0, p->y0) || INB(p->x1, p->y1) || INB(p->x2, p->y2) || INB(p->x3, p->y3))) {
reject_segment:
                    D_8009F9F0 = (Prim *)((u8 *)D_8009F9F0 - 0x34);
                    continue;
                }
                {
                    ((u8 *)p)[3] = 0xC;
                    p->c0 = 0x3E;
                    p->r0 = node->ents[i].r;
                    p->g0 = node->ents[i].g;
                    p->b0 = node->ents[i].b;
                    p->r1 = node->ents[i].r;
                    p->g1 = node->ents[i].g;
                    p->b1 = node->ents[i].b;
                    p->r2 = 0; p->g2 = 0; p->b2 = 0;
                    p->r3 = 0; p->g3 = 0; p->b3 = 0;
                    p->w3 = 0;
                    p->t24 = 0x101;
                    p->t30 = 0x100;
                    p->w6 = (D_8009F9B8 << 16) | 1;
                    SETW0(p, z);
                }

            }
        }
        }
next_node:
        ;
    }
}
