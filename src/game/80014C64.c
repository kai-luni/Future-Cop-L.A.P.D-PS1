// Categories: Graphics > Effects > Screen-Space Sprites | Graphics > GPU Primitives
#include "game/types.h"

typedef struct { u8 pad0[0x28]; u8 m[0x54]; s32 cx, cy, cz; } Cam;
typedef struct { u32 c0, c4; s32 a8, aC; u8 b10, b11, b12, b13, tex, pad15[3]; u32 flags; } Eff;
typedef struct { u8 b[0x34]; } Prim;

extern Cam *gCamera __asm__("D_8009F9E4");
extern Prim *gPrimPoolNext __asm__("D_8009F9F0");
extern Prim *gPrimPoolEnd __asm__("D_8009FA28");
extern void gGteLoadRotation(void *) __asm__("func_80010820");
extern void gProjectVertexBatch(u16 *, u16 *, u8 *, s32) __asm__("func_8001047C");

#define H(p, o) (*(s16 *)((u8 *)(p) + (o)))
#define W(p, o) (*(u32 *)((u8 *)(p) + (o)))
#define B3(p) (*((u8 *)(p) + 3))
#define IN(x, y) (H(p, x) < 0x140 && H(p, y) < 0xF0)
#define ALLOC0(p) do { Prim *h = gPrimPoolNext; if (h < gPrimPoolEnd) gPrimPoolNext = h + 1; p = h; } while (0)
#define ALLOC(p, under, done) do { Prim *h = gPrimPoolNext; if (h < gPrimPoolEnd) goto under; p = h; asm volatile("" : "+r"(p)); goto done; under: p = h; gPrimPoolNext = h + 1; done: asm volatile("" : "+r"(p)); } while (0)
#define FIN(p) W(p, 0) = (W(p, 0) & 0xFF000000) | ((s16)t6 & 0xFFFFFF)
void gDrawBeamQuad(s32 *from, s32 *to, Eff *e) __asm__("func_80014C64");


/**
 * @brief Draws an effect as an axis-aligned screen quad between two projected
 *        points.
 *
 * @param from World start point (three 32-bit components).
 * @param to   World end point.
 * @param e    Effect record: two words at +0/+4, two integers at +0xA8/+0xAC, five
 *             bytes at +0x10..+0x14 (the last one is the texture/page id) and
 *             the flags word.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * The camera block D_8009F9E4 supplies the matrix (func_80010820 loads it) and the
 * centre; both points are projected with func_8001047C, which also returns the
 * clip bytes. The eight resulting half-words are then clipped against the
 * 0x140/0xF0 screen (`IN(x, y)`) and, when visible, a 0x34-byte packet from the
 * D_8009F9F0 pool receives the four corners, the UV word and the ordering-table
 * tag (the FIN macro writes `(tag & 0xFF000000) | (t6 & 0xFFFFFF)`).
 *
 * @par Likely role
 * The tracer/laser-beam primitive: two world points become one stretched screen
 * quad, which is why the geometry is projected and clipped instead of transformed.
 *
 * @par Callers
 * func_800596C4.
 *
 * @par Environment path
 * Projects two world points relative to the camera centre, loads the camera
 * matrix with func_80010820, projects both points with func_8001047C, and
 * emits a stretched screen-space quad of 0x34 bytes from the D_8009F9F0 pool,
 * clipped against the 0x140 by 0xF0 screen through the IN macro. The flags
 * word of the effect record selects the packet shape and the three texture
 * codes 0x2E, 0x3A and 0x3E, and the FIN macro writes the ordering-table tag.
 * Its only caller is the effect emitter func_800596C4, which takes the
 * segment endpoints from a model part, so this is the beam layer of the frame
 * and not the environment tile pass.
 */
void gDrawBeamQuad(s32 *from, s32 *to, Eff *e)
{
    u16 pp[4];
    u16 qq[4];
    u8 out[8];
    volatile u16 tmp;
    u16 uv;
    Prim *p;
    u8 s1;
    register s32 t3 asm("$11"), t5 asm("$13");
    register s32 t0 asm("$8"), t1 asm("$9");
    s32 t2, a3, t6;
    s32 a0, a1, a2, v1;
    register u32 c0 asm("$12"), c4 asm("$10");
    u32 code, t7, a0w;
    register u32 colorcode asm("$2");

    s1 = (e->flags & 5) == 4;
    pp[0] = (from[0] - gCamera->cx) >> 4;
    pp[1] = (from[1] - gCamera->cy) >> 4;
    pp[2] = (from[2] - gCamera->cz) >> 4;
    qq[0] = (to[0] - gCamera->cx) >> 4;
    qq[1] = (to[1] - gCamera->cy) >> 4;
    qq[2] = (to[2] - gCamera->cz) >> 4;
    gGteLoadRotation(gCamera->m);
    gProjectVertexBatch(pp, pp, out, 2);
    if ((out[0] | out[1]) & 1) return;

    a0 = e->a8;
    t2 = (a0 << 9) / (s16)pp[2];
    a1 = (s16)qq[2];
    a3 = e->aC;
    t6 = (((s16)pp[2] + a1) >> 1) - 200;
    t0 = (s16)qq[1] - (s16)pp[1];
    t3 = t0;
    a2 = t0;
    t1 = (s16)pp[0] - (s16)qq[0];
    a3 = (a3 << 9) / a1;
    t5 = t1;
    if (t0 < 0) a2 = -t0;
    a0 = t1;
    if (t1 < 0) a0 = -t1;
    if (a2 < a0) { a2 ^= a0; a0 ^= a2; a2 ^= a0; }
    if ((a2 >> 1) < a0) {
        a1 = a2 + a0;
        a1 = (a1 << 6) + (a1 << 3) + (a1 << 1) + a2 + (a0 << 5);
        a0 = a2 - a0;
        a2 = a1 >> 7;
    }
    v1 = a2 + (a0 >> 2) - (a0 >> 6);
    if (v1 > 0) {
        a0 = (t2 << 10) / v1;
        a3 = (a3 << 10) / v1;
        t0 = (t0 * a0) >> 10;
        t1 = (t1 * a0) >> 10;
        t3 = (t3 * a3) >> 10;
        t5 = (t5 * a3) >> 10;
    } else {
        t5 = 0; t1 = 0; t3 = 0; t0 = 0;
    }

    c0 = e->c0;
    c4 = e->c4;
    if (e->flags & 1) {
        if (e->flags & 2) code = 0x3E; else code = 0x2E;
    } else {
        code = 0x3A;
    }
    colorcode = code << 24;
    c4 = (c4 >> 8) | colorcode;
    c0 = (c0 >> 8) | colorcode;

    if ((e->flags & 3) == 2) {
        ALLOC(p, under1, done1);
        B3(p) = 1;
        W(p, 4) = 0xE10006A0;
        FIN(p);
    }
    ALLOC(p, under2, done2);
    if (s1) {
        H(p, 0x10) = pp[0];
        H(p, 0x12) = pp[1];
        H(p, 0x20) = pp[0] - t0;
        H(p, 0x22) = pp[1] - t1;
        H(p, 0x18) = qq[0] - t3;
        H(p, 0x1A) = qq[1] - t5;
        H(p, 0x8) = qq[0];
        H(p, 0xA) = qq[1];
        if (IN(8, 0xA) || IN(0x10, 0x12) || IN(0x18, 0x1A) || IN(0x20, 0x22)) {
            B3(p) = 8;
            W(p, 0xC) = c0;
            W(p, 4) = c4;
            W(p, 0x14) = 0;
            W(p, 0x1C) = 0;
            FIN(p);
            goto second;
        }
        gPrimPoolNext -= 1;
        goto second;
    }
    if (e->flags & 1) {
        register u32 tex asm("$4");
        u32 texMasked;
        tex = e->tex;
        asm("" : "+r"(tex));
        texMasked = tex & 255;
        t7 = ((((tex & 3) + ((texMasked << 5) | 0xFC)) << 6) | ((((texMasked << 6) & 0x100) + 0x180) >> 4)) << 16;
        if (e->flags & 2) {
            H(p, 0x14) = pp[0] + t0;
            H(p, 0x16) = pp[1] + t1;
            H(p, 0x2C) = pp[0] - t0;
            H(p, 0x2E) = pp[1] - t1;
            H(p, 0x20) = qq[0] - t3;
            H(p, 0x22) = qq[1] - t5;
            a0w = ((((tex & 1) << 4) | (((((texMasked >> 1) << 7) + 0x180) & 0x3FF) >> 6) | 0xA0)) << 16;
            H(p, 0x8) = qq[0] + t0;
            H(p, 0xA) = qq[1] + t1;
            if (IN(8, 0xA) || IN(0x14, 0x16) || IN(0x20, 0x22) || IN(0x2C, 0x2E)) {
                B3(p) = 0xC;
                W(p, 0x10) = c0;
                W(p, 4) = c4;
                W(p, 0x28) = c0;
                W(p, 0x1C) = c4;
                tmp = e->b10 | (e->b11 << 8);
                W(p, 0xC) = tmp | t7;
                tmp = uv = e->b12 | (e->b11 << 8);
                H(p, 0x24) = uv;
                tmp = uv = e->b12 | (e->b13 << 8);
                H(p, 0x30) = uv;
                tmp = e->b10 | (e->b13 << 8);
                W(p, 0x18) = tmp | a0w;
                goto fin;
            }
        } else {
            H(p, 0x10) = pp[0] + t0;
            H(p, 0x12) = pp[1] + t1;
            H(p, 0x20) = pp[0] - t0;
            H(p, 0x22) = pp[1] - t1;
            H(p, 0x18) = qq[0] - t3;
            H(p, 0x1A) = qq[1] - t5;
            a0w = ((((tex & 1) << 4) | (((((texMasked >> 1) << 7) + 0x180) & 0x3FF) >> 6) | 0x80)) << 16;
            H(p, 0x8) = qq[0] + t0;
            H(p, 0xA) = qq[1] + t1;
            if (IN(8, 0xA) || IN(0x10, 0x12) || IN(0x18, 0x1A) || IN(0x20, 0x22)) {
                B3(p) = 9;
                W(p, 4) = c0;
                tmp = e->b10 | (e->b11 << 8);
                W(p, 0xC) = tmp | t7;
                tmp = uv = e->b12 | (e->b11 << 8);
                H(p, 0x1C) = uv;
                tmp = uv = e->b12 | (e->b13 << 8);
                H(p, 0x24) = uv;
                tmp = e->b10 | (e->b13 << 8);
                W(p, 0x14) = tmp | a0w;
                goto fin;
            }
        }
        gPrimPoolNext -= 1;
        return;
    }
    H(p, 0x10) = pp[0] + t0;
    H(p, 0x12) = pp[1] + t1;
    H(p, 0x20) = pp[0] - t0;
    H(p, 0x22) = pp[1] - t1;
    H(p, 0x18) = qq[0] - t3;
    H(p, 0x1A) = qq[1] - t5;
    H(p, 0x8) = qq[0] + t0;
    H(p, 0xA) = qq[1] + t1;
    if (!(IN(8, 0xA) || IN(0x10, 0x12) || IN(0x18, 0x1A) || IN(0x20, 0x22))) {
        gPrimPoolNext -= 1;
        return;
    }
        B3(p) = 8;
        W(p, 0xC) = c0;
        W(p, 4) = c4;
        W(p, 0x1C) = c0;
        W(p, 0x14) = c4;

fin:
    FIN(p);
    return;
second:
    ALLOC0(p);
    H(p, 0x18) = pp[0];
    H(p, 0x1A) = pp[1];
    H(p, 0x8) = qq[0];
    H(p, 0xA) = qq[1];
    H(p, 0x10) = qq[0] + t3;
    H(p, 0x12) = qq[1] + t5;
    H(p, 0x20) = pp[0] + t0;
    H(p, 0x22) = pp[1] + t1;
    if (!(IN(8, 0xA) || IN(0x10, 0x12) || IN(0x18, 0x1A) || IN(0x20, 0x22))) {
        gPrimPoolNext -= 1;
        goto second;
    }
        B3(p) = 8;
        W(p, 0x14) = c0;
        W(p, 4) = c4;
        W(p, 0x1C) = 0;
        W(p, 0xC) = 0;
    FIN(p);
}
