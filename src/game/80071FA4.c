// Categories: Graphics > Model Renderer > Full Path | Graphics > GTE > Lighting
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
extern void gGteLoadRotation(void *m) __asm__("func_80010820");
extern void gGteLoadMatrix(void *m) __asm__("func_80010750");
extern void gGteTransformWords(void *a, void *b, s32 c) __asm__("func_8001006C");
extern void func_8001047C(void *a, void *b, void *c, s32 n);

extern s16 gPreparedVertexTable[][4] __asm__("D_800A6418");
extern u8 D_800AB388[];
extern u8 D_800AB389[];
extern s16 D_800A7840[];
extern s16 D_800A7898[];
extern Cam *gCamera __asm__("D_8009F9E4");
extern Prim *gPrimPoolNext __asm__("D_8009F9F0");
extern Prim *gPrimPoolEnd __asm__("D_8009FA28");
extern s32 D_8009F9B8;

#define ALLOC(p) \
    p = gPrimPoolNext; \
    if (gPrimPoolNext < gPrimPoolEnd) gPrimPoolNext = (Prim *)((u8 *)gPrimPoolNext + 0x34)

#define ALLOC_T(p) p = (gPrimPoolNext >= gPrimPoolEnd) ? gPrimPoolNext : gPrimPoolNext++
#define ALLOC_U(p) p = (gPrimPoolNext < gPrimPoolEnd) ? gPrimPoolNext++ : gPrimPoolNext
#define ALLOC_A(p) ALLOC(p)

#define ALLOC_REG(p, reg) do { \
    register Prim *cur __asm__(reg) = gPrimPoolNext; \
    p = cur; if (cur < gPrimPoolEnd) { gPrimPoolNext = (Prim *)((u8 *)p + 0x34); } \
} while (0)

#define SETW0(p, v) (p)->w0 = ((p)->w0 & 0xFF000000) | ((s16)(v) & 0xFFFFFF)

#define INB(x, y) ((x) < 0x140 && (y) < 0xF0)

/**
 * @brief The full model renderer: per-vertex transform, lighting, projection and
 *        packet emission for one node.
 *
 * @param node Node: vertex stream at +0, a short-coordinate block, a vertex count,
 *             a base RGB triple and a subdivision table (see the struct at the
 *             top of the file).
 * @param arg  Frame argument forwarded to the sub-calls.
 * @return Nothing.
 *
 * @par Confirmed flow (from the callees and the field use)
 * 1. func_80010820 / func_80010750 load the node matrix, func_8001006C transforms
 *    the vertices in bulk and func_8001047C projects them, returning the clip
 *    flags.
 * 2. func_8001A100 provides the radius test that rejects nodes outside the view.
 * 3. The per-vertex colours are computed in the GTE (NCCS via func_80015F14 in
 *    the sibling path) and written into the packet from the D_8009F9F0 pool; the
 *    ordering-table tag is the depth word taken from the camera.
 *
 * @par Likely role
 * The expensive path: func_80073700 picks it for nodes that carry geometry
 * pointers, and func_80072F7C (flat shading, single colour) handles the rest.
 *
 * @par Callers
 * func_80073700 only.
 *
 * @par Environment path
 * The full model renderer for one node: per entry it runs the radius test
 * func_8001A100, builds the model or camera matrix, loads it with
 * func_80010750 or func_80010820, transforms the vertices with func_8001006C,
 * projects them into gPreparedVertexTable with func_8001047C and emits
 * per-segment Gouraud quads, plus a two-point line when a projected segment
 * has no width, from the D_8009F9F0 pool. It is called only by the object
 * dispatcher func_80073700 (line 42), which gRunViewScenePasses
 * (func_8003F6E4, line 83) runs inside every environment view. It is
 * therefore the model layer of the environment pass, and func_80072F7C is the
 * flat-shaded fallback for nodes without geometry pointers.
 */
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
                gPreparedVertexTable[prep][0] = node->ents[i].verts[prep * 3 + 0] >> 4;
                gPreparedVertexTable[prep][1] = node->ents[i].verts[prep * 3 + 1] >> 4;
                gPreparedVertexTable[prep][2] = node->ents[i].verts[prep * 3 + 2] >> 4;
            }
        } else {
            for (prep = 0; prep < node->ents[i].cnt + 1; prep++) {
                gPreparedVertexTable[prep][0] = (node->ents[i].verts[prep * 3 + 0] - gCamera->ox) >> 4;
                gPreparedVertexTable[prep][1] = (node->ents[i].verts[prep * 3 + 1] - gCamera->oy) >> 4;
                gPreparedVertexTable[prep][2] = (node->ents[i].verts[prep * 3 + 2] - gCamera->oz) >> 4;
            }
        }

        }

        if (node->flags & 8) {
            v[0] = node->a->sub->f28 - gCamera->ox;
            v[1] = node->a->sub->f2C - gCamera->oy;
            v[2] = node->a->sub->f30 - gCamera->oz;
            gGteLoadRotation(&gCamera->mtx);
            gGteTransformWords((u8 *)node->a->sub + 4, &m, 3);
            gGteLoadRotation(&gCamera->mtx);
            gGteTransformWords(v, &m.m[9], 1);
        } else {
            s32 first;
            first = gCamera->mtx.m[0];
            m.m[1] = gCamera->mtx.m[1];
            m.m[2] = gCamera->mtx.m[2];
            m.m[3] = gCamera->mtx.m[3];
            m.m[4] = gCamera->mtx.m[4];
            m.m[5] = gCamera->mtx.m[5];
            m.m[6] = gCamera->mtx.m[6];
            m.m[7] = gCamera->mtx.m[7];
            m.m[8] = gCamera->mtx.m[8];
            m.m[9] = gCamera->mtx.m[9];
            m.m[10] = gCamera->mtx.m[10];
            m.m[11] = gCamera->mtx.m[11];
            m.m[0] = first;
        }
        if (node->flags & 8) gGteLoadMatrix(&m);
        else gGteLoadRotation(&m);

        func_8001047C(gPreparedVertexTable, gPreparedVertexTable, D_800AB388, node->ents[i].cnt + 1);

        {
            s32 a3 = 0, a2 = 1;
            s32 t3 = node->a->f1C << 4;
            s32 step = ((node->b->f1C - node->a->f1C) << 4) / (node->ents[i].cnt + 1);
            for (j = 0; j < node->ents[i].cnt + 1; j++) {
                if (!((D_800AB388[j] | D_800AB388[a3] | D_800AB388[a2]) & 1)) {
                    s32 sc = ((t3 >> 4) << 10) / gPreparedVertexTable[a3][2];
                    s32 ax, ay, t, len, rawax;
                    s32 raway;
                    s32 product;
                    if (sc < 3) sc = 3;
                    D_800A7840[j] = gPreparedVertexTable[a2][1] - gPreparedVertexTable[a3][1];
                    D_800A7898[j] = gPreparedVertexTable[a3][0] - gPreparedVertexTable[a2][0];
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
            z = ((gPreparedVertexTable[j][2] + gPreparedVertexTable[j + 1][2]) >> 1) - 0x100;
            if (D_800A7840[j] == 0 && D_800A7898[j] == 0) {
                s16 dx = gPreparedVertexTable[j + 1][0] - gPreparedVertexTable[j][0];
                s16 dy = gPreparedVertexTable[j + 1][1] - gPreparedVertexTable[j][1];
                if (dx < 0) {
                    if (dy < 0) {
                        if (dx < dy) gPreparedVertexTable[j][0]--; else gPreparedVertexTable[j][1]--;
                    } else if (dy < -dx) gPreparedVertexTable[j][0]--; else gPreparedVertexTable[j][1]++;
                } else {
                    if (dy < 0) {
                        if (-dy < dx) gPreparedVertexTable[j][0]++; else gPreparedVertexTable[j][1]--;
                    } else if (dy < dx) gPreparedVertexTable[j][0]++; else gPreparedVertexTable[j][1]++;
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
                p->x0 = gPreparedVertexTable[j][0];
                p->y0 = gPreparedVertexTable[j][1];
                ((s16 *)p)[6] = gPreparedVertexTable[j + 1][0];
                ((s16 *)p)[7] = gPreparedVertexTable[j + 1][1];
            } else {
                ALLOC_T(p);
                p->x0 = gPreparedVertexTable[j][0];
                p->y0 = gPreparedVertexTable[j][1];
                p->x1 = gPreparedVertexTable[j][0] - D_800A7840[j];
                p->y1 = gPreparedVertexTable[j][1] - D_800A7898[j];
                p->x3 = gPreparedVertexTable[j + 1][0] - D_800A7840[j + 1];
                p->y3 = gPreparedVertexTable[j + 1][1] - D_800A7898[j + 1];
                p->x2 = gPreparedVertexTable[j + 1][0];
                p->y2 = gPreparedVertexTable[j + 1][1];
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
                p->x0 = gPreparedVertexTable[j][0];
                p->y0 = gPreparedVertexTable[j][1];
                p->x1 = gPreparedVertexTable[j + 1][0];
                p->y1 = gPreparedVertexTable[j + 1][1];
                p->x3 = gPreparedVertexTable[j + 1][0] + D_800A7840[j + 1];
                p->y3 = gPreparedVertexTable[j + 1][1] + D_800A7898[j + 1];
                p->x2 = gPreparedVertexTable[j][0] + D_800A7840[j];
                p->y2 = gPreparedVertexTable[j][1] + D_800A7898[j];
                if (!(INB(p->x0, p->y0) || INB(p->x1, p->y1) || INB(p->x2, p->y2) || INB(p->x3, p->y3))) {
reject_segment:
                    gPrimPoolNext = (Prim *)((u8 *)gPrimPoolNext - 0x34);
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
