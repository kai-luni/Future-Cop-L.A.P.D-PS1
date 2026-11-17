// Categories: Graphics > Renderer > Ribbon Pass | Graphics > GTE > Projection | Graphics > GPU Primitives
#include "game/types.h"

#define W(p,o) (*(s32 *)((u8 *)(p)+(o)))
#define H(p,o) (*(s16 *)((u8 *)(p)+(o)))
#define UH(p,o) (*(u16 *)((u8 *)(p)+(o)))
#define B(p,o) (*(s8 *)((u8 *)(p)+(o)))
#define UB(p,o) (*(u8 *)((u8 *)(p)+(o)))
#define PTR(p,o) (*(u8 **)((u8 *)(p)+(o)))

typedef struct { s16 x, y, z, pad; } Point;
typedef struct { s32 x, y, z; } Vec;
typedef struct { s32 e[9]; } Matrix;
typedef struct { s32 page, x, y, w, h; } Texture;
extern Texture D_8009B234[];
extern u8 *gPrimPoolNext __asm__("D_8009F9F0");
extern u8 *gPrimPoolEnd __asm__("D_8009FA28");
extern u8 D_800A3548[];
extern Point D_800A6418[];
extern u8 D_800AB388[];
extern s16 D_800A7840[], D_800A7898[];
extern u8 *gCamera __asm__("D_8009F9E4");
extern u8 *func_8003D9F8();
extern u8 *func_8003D9C4();
extern s32 func_8001A100();
extern void func_8001A81C();
extern void func_80010820();
extern void func_8001006C();
extern void func_80010750();
extern void func_8001047C();

/**
 * @brief Renders one object list as camera-facing polygon ribbons: every list
 *        entry contributes a polyline whose segments become two screen-space
 *        quads (or one textured quad) of perspective-correct width.
 *
 * @param input Head of the singly linked object list; next is at +0x00.
 * @param pos   Camera/eye position (3 x s32) in the same 1/16 world units as
 *              the node coordinates: it is subtracted from every node
 *              coordinate before the shift into screen space.
 *
 * @return Nothing. The function only fills the primitive pool.
 *
 * @par Globals (confirmed object fields) (offsets into input and its successors)
 * - +0x0F bit 3 (0x08): set means the entry is skipped for this frame.
 * - +0x1C: holder/parent pointer handed to func_8003D9F8.
 * - +0x1A (u16) flags: 0x0100 = "coordinates are already in view space" (no
 *   camera subtraction, func_80010750 instead of func_80010820 for the basis),
 *   0x2000 = second flag used to pick the transform source of the extra point.
 * - +0x8E (u16) flags: 0x2000 = "no header point" (the strip starts at the
 *   node chain itself instead of the D_800A3548 header point), 0x0010 =
 *   position is taken verbatim from +0x28..+0x30, 0x0004 = force untextured.
 * - +0x10 (u8) bit 0: skip the distance cull.
 * - +0x12 (u16), +0x73, +0x76, +0x79 (u8): copied into the D_800A3548 header
 *   record at +0x10, +0x15, +0x16 and +0x17.
 * - +0x68: pointer to the first vertex node of the strip.
 * - +0x72 (s8): texture index; >= 0 selects the entry in D_8009B234 (the
 *   5-word texture table page/x/y/w/h), < 0 selects per-segment textures from
 *   node +0x2A instead.
 * - +0x28/+0x2C/+0x30: the object position, used when 0x8E bit 4 is set.
 * - +0x58: 3-word vector transformed with the object matrix when 0x1A bit
 *   0x100 is set.
 * Vertex node: next +0x00, x/y/z (s32) +0x04/+0x08/+0x0C, half-word +0x10
 * (strip half-width, shifted left by 9 in the width formula), colour word
 * +0x14 (>> 8 gives the three RGB bytes), per-segment texture byte +0x2A,
 * and bit 0 of the half-word at +0x18 marks the end of the chain.
 *
 * @par Control flow (confirmed)
 * 1. Outer loop over the list; invisible entries jump straight to the advance
 *    label at the bottom.
 * 2. func_8003D9F8 builds the base transform from the entry at +0x1C (or a
 *    null holder) and func_8003D9C4 supplies the mesh slot. The result is
 *    only used when the node transform pointer is non-null.
 * 3. Unless +0x10 bit 0 is set, the vector from pos to the transform
 *    translation (+0x24/+0x28/+0x2C) is measured with func_8001A100 and the
 *    entry is dropped above 0x1FFFF - a hard far-distance cull.
 * 4. Unless +0x8E bit 0x2000 is set, a header point is written to
 *    D_800A6418[0]: either the transformed +0x58 vector, the raw +0x28
 *    position, or the object position plus the matrix translation. All three
 *    are converted with (value - camera) >> 4 for the camera-space paths and
 *    then count++.
 * 5. The vertex chain is appended to D_800A6418, one entry (4 half-words) per
 *    node, using the same camera subtraction, until the end marker or null.
 *    Fewer than two points abort the entry.
 * 6. The basis is rebuilt (camera matrix from D_8009F9E4, or the object
 *    matrix) and func_8001047C projects the point array while writing one
 *    clip-flag byte per point into D_800AB388.
 * 7. Per segment i: t = flag[i] | flag[previous] | flag[next]; only when bit 0
 *    is clear and t == 0xC0 (all four clip bits set, i.e. fully inside) is the
 *    segment drawn. The width is (node_halfwidth << 9) / depth, clamped to a
 *    minimum of 3 pixels; the segment normal is (dy, -dx) and is rescaled by
 *    an integer approximation of the reciprocal hypotenuse
 *    (q = a + (b >> 2) - (b >> 6) after the (a + b) * 0x49 / 0x80 step), so the
 *    ribbon keeps a roughly constant on-screen thickness. The scaled dx/dy
 *    land in D_800A7840 / D_800A7898.
 * 8. Emission per segment: with a texture, one 0x34-byte textured quad whose
 *    tpage/CLUT words are built inline (Sony getTPage/getClut shape with the
 *    0x180 / 0xA0 bases) and whose UVs come from the texture table or from the
 *    per-segment index; without a texture, two flat quads (code written as
 *    0x3A000000 in the colour word, 0x08000000 in the header) covering the
 *    left and the right half of the band. Both paths reject a quad only when
 *    all four corners are off screen (x >= 320 or y >= 240 - there is no
 *    negative bound in the test) and then roll the cursor back by 0x34.
 *
 * @par Globals read/written
 * D_8009F9F0 / D_8009FA28: packet cursor and end of the primitive pool
 * (bump-allocated, last slot reused when full). D_800A6418: scratch array of
 * projected points. D_800A7840 / D_800A7898: per-segment dx/dy. D_800AB388:
 * clip-flag bytes. D_800A3548: the header primitive record filled from the
 * object (+0x10, +0x15, +0x16, +0x17 and the node pointer at +0x00).
 * D_8009F9E4: camera matrix block (matrix at +0x28, translation at
 * +0x7C/+0x80/+0x84). D_8009B234: texture table.
 *
 * @par Calls
 * func_8003D9F8 (build the strip transform), func_8003D9C4 (mesh slot),
 * func_8001A100 (3D length for the far cull), func_8001A81C (matrix times
 * vector for the extra point), func_80010820 / func_80010750 (load a rotation
 * matrix with / without translation into the GTE), func_8001006C (batch
 * transform), func_80010234 (transform with 0xC output stride), func_8001047C
 * (RTPS projection plus clip flags).
 *
 * @par Side effects
 * Consumes up to 0x34 bytes per emitted primitive from the shared pool, may
 * leave the cursor exactly at D_8009FA28 when the pool is exhausted, and
 * overwrites the shared scratch arrays D_800A6418, D_800A7840, D_800A7898 and
 * D_800AB388 (all of them fully rebuilt per list entry, so nothing survives
 * across calls).
 *
 * @par Callers
 * func_8003F6E4 (direct) - the per-class render dispatch: it resolves one
 * world position and then calls the sibling list renderers in sequence; this
 * one receives D_800A1070.unk14 and the position triple.
 *
 * @par Likely
 * The pass draws light trails / ribbon effects (rocket exhaust, skid marks,
 * beam or road edges): the strip width is derived from a per-node radius,
 * colours are interpolated between the two nodes of each segment, and the whole
 * geometry is rebuilt from a polyline every frame.
 *
 * @par Unknown
 * The exact meaning of the length byte at +0x03 of each packet and of the
 * 0x24 / 0x2C / 0x3A / 0x3E codes written into the colour word: only their
 * positions inside the 0x34-byte block are confirmed. The purpose of the two
 * flags 0x2000 (in +0x1A and in +0x8E) is not established beyond which code
 * path they select.
 */
void func_80074104(u8 *input, s32 *pos)
{
    register u8 *obj asm("$23") = input;
    register u8 *owner asm("$19");
    register s32 count asm("$20");
    register u8 *base asm("$21");
    register s32 next asm("$22");
    struct { Vec v; s32 pad0; Matrix mat; Vec projected, trans; s32 pad1; u32 texpage0, texpage1, uv0, uv1; u16 uv2; u8 pad2[6]; u16 uv3; } frame;
    u8 *node, *m, *p, *cursor;
    register u8 *prim asm("$6");
    register s32 z asm("$2");
    register s32 *pp asm("$25");
    Texture *tex;
    register Texture *textures asm("$15");
    s32 previous, i, a, b, width, q, t, depth, mat0, original_dx;
    register s32 offset asm("$10");
    u32 ux, uy, uw, uh;
    s16 *dx, *dy;
    register Point *out asm("$4");
    register Point *cur asm("$7");
    register Point *following asm("$4");
    Point *points;
    while (obj) {
        if (obj[0xF] & 8) goto advance;
        next = 1;
        count = 0;
        previous = 0;
        node = PTR(obj, 0x68);
        owner = obj;
        if (!node) goto advance;
        p = PTR(obj, 0x1c);
        base = func_8003D9F8(p ? W(p, 0x34) : 0, UH(obj, 0x1a), B(obj, 0xe), 1, p, obj + 0x34);
        m = func_8003D9C4(obj);
        if (!(UB(obj,0x10) & 1)) {
            if (m) {
                __asm__("" : "=r"(pp) : "0"(pos));
                frame.v.x = W(m,0x24) - pp[0];
                frame.v.y = W(m,0x28) - pp[1];
                z = W(m,0x2c);
            } else {
                __asm__("" : "=r"(pp) : "0"(pos));
                frame.v.x = W(base,0x24) - pp[0];
                frame.v.y = W(base,0x28) - pp[1];
                z = W(base,0x2c);
            }
            frame.v.z = z - pp[2];
            if (func_8001A100(&frame.v) > 0x1ffff) goto advance;
        }
        if (!(UH(owner,0x8e) & 0x2000)) {
            UH(D_800A3548,0x10) = UH(owner,0x12);
            UB(D_800A3548,0x15) = UB(owner,0x73);
            UB(D_800A3548,0x16) = UB(owner,0x76);
            PTR(D_800A3548,0) = node;
            UB(D_800A3548,0x17) = UB(owner,0x79);
            if (UH(owner,0x1a) & 0x100) {
                func_8001A81C(m, owner+0x58, &frame.trans);
                D_800A6418[count].x = frame.trans.x >> 4;
                D_800A6418[count].y = frame.trans.y >> 4;
                D_800A6418[count].z = frame.trans.z >> 4;
            } else if (UH(owner,0x8e) & 0x10) {
                frame.trans = *(Vec *)(owner+0x28);
                D_800A6418[count].x = (frame.trans.x-W(gCamera,0x7c)) >> 4;
                D_800A6418[count].y = (frame.trans.y-W(gCamera,0x80)) >> 4;
                D_800A6418[count].z = (frame.trans.z-W(gCamera,0x84)) >> 4;
            } else {
                func_80010820(m);
                func_8001006C(owner+0x28, &frame.trans, 1);
                frame.trans.x = W(owner,0x28)+W(m,0x24);
                frame.trans.y = W(owner,0x2c)+W(m,0x28);
                frame.trans.z = W(owner,0x30)+W(m,0x2c);
                D_800A6418[count].x = (frame.trans.x-W(gCamera,0x7c)) >> 4;
                D_800A6418[count].y = (frame.trans.y-W(gCamera,0x80)) >> 4;
                D_800A6418[count].z = (frame.trans.z-W(gCamera,0x84)) >> 4;
            }
            count++;
        }
        __asm__("" : "+r"(count));
        out = &D_800A6418[count];
        while (node) {
            if (UH(node,0x18) & 1) break;
            if (UH(owner,0x1a) & 0x100) {
                out->x = W(node,4) >> 4;
                out->y = W(node,8) >> 4;
                out->z = W(node,12) >> 4;
            } else {
                out->x = (W(node,4) - W(gCamera,0x7c)) >> 4;
                out->y = (W(node,8) - W(gCamera,0x80)) >> 4;
                out->z = (W(node,12) - W(gCamera,0x84)) >> 4;
            }
            ++out;
            node = PTR(node,0);
            count++;
        }
        if (count < 2) goto advance;
        __asm__("" : "=r"(points) : "0"(D_800A6418), "r"(count));
        if (UH(owner,0x1a) & 0x100) {
            frame.v.x = W(m,0x24)-W(gCamera,0x7c);
            frame.v.y = W(m,0x28)-W(gCamera,0x80);
            frame.v.z = W(m,0x2c)-W(gCamera,0x84);
            func_80010820(gCamera+0x28);
            func_8001006C(m, &frame.mat, 3);
        } else {
            frame.v.x = W(base,0x24)-W(gCamera,0x7c);
            frame.v.y = W(base,0x28)-W(gCamera,0x80);
            frame.v.z = W(base,0x2c)-W(gCamera,0x84);
            mat0 = W(gCamera,0x28);
            frame.mat.e[1] = W(gCamera,0x2c);
            frame.mat.e[2] = W(gCamera,0x30);
            frame.mat.e[3] = W(gCamera,0x34);
            frame.mat.e[4] = W(gCamera,0x38);
            frame.mat.e[5] = W(gCamera,0x3c);
            frame.mat.e[6] = W(gCamera,0x40);
            frame.mat.e[7] = W(gCamera,0x44);
            frame.mat.e[8] = W(gCamera,0x48);
            frame.mat.e[0] = mat0;
        }
        func_80010820(gCamera+0x28);
        func_8001006C(&frame.v, &frame.projected, 1);
        node = (u8 *)&frame.mat;
        if (UH(owner,0x1a) & 0x100) func_80010750(node);
        else func_80010820(node);
        func_8001047C(D_800A6418, D_800A6418, D_800AB388, count);
        if (!(UH(owner,0x8e) & 0x2000)) node = D_800A3548;
        else node = PTR(owner,0x68);
        dx = D_800A7840; dy = D_800A7898;
        for (i=0; i<count; i++) {
            t = D_800AB388[i] | D_800AB388[previous] | D_800AB388[next];
            if (!(t & 1) && t == 0xc0) {
                width = (H(node,0x10) << 9) / D_800A6418[previous].z;
                if (width < 3) width = 3;
                *dx = D_800A6418[next].y - D_800A6418[previous].y;
                *dy = D_800A6418[previous].x - D_800A6418[next].x;
                original_dx = *dx; a = original_dx; if (a < 0) a = -a;
                b = *dy; if (b < 0) b = -b;
                if (a < b) { a ^= b; b ^= a; a ^= b; }
                if ((a >> 1) < b) {
                    q = ((a+b)<<6) + ((a+b)<<3) + ((a+b)<<1) + a + (b<<5);
                    b = a-b;
                    a = q >> 7;
                }
                q = a + (b >> 2) - (b >> 6);
                if (q > 0) { width = (width << 10) / q; *dx = (original_dx*width)>>10; *dy = (*dy*width)>>10; }
                else { *dy = 0; *dx = 0; }
                __asm__("" : : "r"(next));
            }
            next = i+2;
            node = PTR(node,0);
            previous = i;
            if (next >= count) next = count-1;
            dx++; dy++;
        }
        if (!(UH(owner,0x8e) & 0x2000)) node = D_800A3548;
        else node = PTR(owner,0x68);
        if (!(UH(owner,0x8e) & 4)) {
        if (B(owner,0x72) >= 0) {
            s32 tx, ty, tw;
            tex = D_8009B234;
            if (B(owner,0x72) < 2) tex += B(owner,0x72);
            t = tex->page;
            frame.uv1 = (((t & 1) << 4) | (((((t >> 1) << 7) + 0x180) & 0x3ff) >> 6) | 0xa0) << 16;
            frame.uv0 = (((((t & 3) + ((t << 5) | 0xfc)) << 6) | (((u32)(t << 6 & 0x100) + 0x180) >> 4))) << 16;
            ty = tex->y; tx = tex->x; tw = tex->w;
            ty <<= 8;
            frame.uv0 = *(volatile u32 *)&frame.uv0 | (tx | ty);
            frame.uv1 = *(volatile u32 *)&frame.uv1 | ((tx + tw - 1) | ty);
            ux = UH(tex,4); uw = UH(tex,12); uy = UH(tex,8); uh = UH(tex,16);
            frame.uv2 = (ux + uw - 1) | ((uy + uh - 1) << 8);
            frame.uv3 = ux | ((uy + uh - 1) << 8);
        }
        } else tex = 0;
        textures = D_8009B234;
        cur = D_800A6418;
        offset = 8;
        for (i=0; i<count-1; ++i, ++cur, offset+=8) {
            t = D_800AB388[i] | D_800AB388[i+1];
            if ((t & 1) || t != 0xc0) goto next_node;
            if (!(UH(owner,0x8e) & 4) && B(owner,0x72) < 0) {
                p = PTR(node,0);
                t = p ? B(p,0x2a) : B(node,0x2a);
                tex = &textures[t];
                t = tex->page;
                frame.texpage1 = (((t & 1) << 4) | (((((t >> 1) << 7) + 0x180) & 0x3ff) >> 6) | 0xa0) << 16;
                frame.texpage0 = (((((t & 3) + ((t << 5) | 0xfc)) << 6) | (((u32)(t << 6 & 0x100) + 0x180) >> 4))) << 16;
            }
            depth = ((cur->z + H((u8 *)D_800A6418+offset,4)) >> 1) - 0x100;
            if (tex) {
                cursor = gPrimPoolNext;
                if (cursor < gPrimPoolEnd) gPrimPoolNext = cursor + 0x34;
                __asm__("" : "=r"(prim) : "0"(cursor));
                H(prim,8) = cur->x + D_800A7840[i];
                H(prim,0x14) = cur->x - D_800A7840[i];
                H(prim,0xa) = cur->y + D_800A7898[i];
                H(prim,0x16) = cur->y - D_800A7898[i];
                __asm__("" : "=r"(following) : "0"(D_800A6418), "r"(offset));
                following = (Point *)((u8 *)following + offset);
                H(prim,0x2c) = following->x - D_800A7840[i+1];
                H(prim,0x20) = following->x + D_800A7840[i+1];
                H(prim,0x2e) = following->y - D_800A7898[i+1];
                H(prim,0x22) = following->y + D_800A7898[i+1];
                if (!((H(prim,8)<320 && H(prim,0xa)<240) ||
                      (H(prim,0x14)<320 && H(prim,0x16)<240) ||
                      (H(prim,0x20)<320 && H(prim,0x22)<240) ||
                      (H(prim,0x2c)<320 && H(prim,0x2e)<240))) {
                    gPrimPoolNext -= 0x34;
                    goto next_node;
                }
                {
                u32 c0 = W(node,0x14);
                u8 *nn2 = PTR(node,0);
                u32 c1 = W(nn2,0x14);
                __asm__("" : "+r"(c0), "+r"(c1));
                c0 = (c0 >> 8) | 0x3e000000;
                W(prim,0x10) = c0; W(prim,4) = c0;
                W(prim,0x28) = c1 >> 8; W(prim,0x1c) = c1 >> 8;
                }
                if (B(owner,0x72) < 0) {
                    W(prim,0xc) = (u16)(UH(tex,4) | (UH(tex,8)<<8)) | frame.texpage0;
                    {
                    u32 tx = UH(tex,4), tw = UH(tex,12), page;
                    __asm__("lw %0,%1" : "=r"(page) : "m"(frame.texpage1), "r"(tx), "r"(tw));
                    W(prim,0x18) = (u16)((tx+tw-1) | (UH(tex,8)<<8)) | page;
                    }
                    UH(prim,0x30) = (UH(tex,4)+UH(tex,12)-1) | ((UH(tex,8)+UH(tex,16)-1)<<8);
                    UH(prim,0x24) = UH(tex,4) | ((UH(tex,8)+UH(tex,16)-1)<<8);
                } else {
                    W(prim,0xc) = frame.uv0;
                    W(prim,0x18) = frame.uv1;
                    UH(prim,0x30) = frame.uv2;
                    UH(prim,0x24) = frame.uv3;
                }
                W(prim,0) = (s16)depth | 0x0c000000;
            } else {
                register u32 cmdhi asm("$4");
                s32 room;
                cursor = gPrimPoolNext;
                if (cursor >= gPrimPoolEnd) {
                    __asm__("" : "=r"(prim) : "0"(cursor));
                    __asm__("lui %0,0xe100" : "=r"(cmdhi) : "r"(cursor));
                } else {
                    __asm__("" : "=r"(prim) : "0"(cursor));
                    gPrimPoolNext = prim + 0x34;
                    __asm__("lui %0,0xe100" : "=r"(cmdhi) : "r"(prim));
                }
                {
                s32 tagdepth;
                __asm__("" : "=r"(tagdepth) : "0"(depth), "r"(cmdhi));
                W(prim,0) = (s16)tagdepth | 0x01000000;
                }
                cursor = gPrimPoolNext;
                room = cursor < gPrimPoolEnd;
                W(prim,4) = cmdhi | 0x6a0;
                if (!room) {
                    __asm__("" : "+r"(cursor));
                    prim = cursor;
                } else {
                    __asm__("" : "+r"(cursor));
                    prim = cursor;
                    gPrimPoolNext = prim + 0x34;
                }
                H(prim,8) = cur->x + D_800A7840[i];
                H(prim,0xa) = cur->y + D_800A7898[i];
                H(prim,0x10) = cur->x;
                H(prim,0x12) = cur->y;
                __asm__("" : "=r"(following) : "0"(D_800A6418), "r"(offset));
                following = (Point *)((u8 *)following + offset);
                H(prim,0x18) = following->x + D_800A7840[i+1];
                H(prim,0x1a) = following->y + D_800A7898[i+1];
                H(prim,0x20) = following->x;
                H(prim,0x22) = following->y;
                if ((H(prim,8)<320 && H(prim,0xa)<240) ||
                    (H(prim,0x10)<320 && H(prim,0x12)<240) ||
                    (H(prim,0x18)<320 && H(prim,0x1a)<240) ||
                    (H(prim,0x20)<320 && H(prim,0x22)<240)) {
                    {
                    u32 c0 = W(node,0x14);
                    u8 *nn = PTR(node,0);
                    u32 c1 = W(nn,0x14);
                    __asm__("" : "+r"(c0), "+r"(c1));
                    W(prim,4) = 0x3a000000;
                    W(prim,0xc) = (c0 >> 8) | 0x3a000000;
                    W(prim,0x14) = 0;
                    W(prim,0) = (s16)depth | 0x08000000;
                    W(prim,0x1c) = c1 >> 8;
                    }
                    cursor = gPrimPoolNext;
                    if (cursor < gPrimPoolEnd) gPrimPoolNext = cursor + 0x34;
                    __asm__("" : "=r"(prim) : "0"(cursor));
                    H(prim,8) = cur->x;
                    H(prim,0xa) = cur->y;
                    H(prim,0x10) = cur->x - D_800A7840[i];
                    H(prim,0x12) = cur->y - D_800A7898[i];
                    __asm__("" : "=r"(following) : "0"(D_800A6418), "r"(offset));
                following = (Point *)((u8 *)following + offset);
                H(prim,0x18) = following->x;
                    H(prim,0x1a) = following->y;
                    H(prim,0x20) = following->x - D_800A7840[i+1];
                    H(prim,0x22) = following->y - D_800A7898[i+1];
                    if ((H(prim,8)<320 && H(prim,0xa)<240) ||
                        (H(prim,0x10)<320 && H(prim,0x12)<240) ||
                        (H(prim,0x18)<320 && H(prim,0x1a)<240) ||
                        (H(prim,0x20)<320 && H(prim,0x22)<240)) {
                        W(prim,0xc) = 0;
                        W(prim,0x1c) = 0;
                        W(prim,4) = ((u32)W(node,0x14) >> 8) | 0x3a000000;
                        W(prim,0) = (s16)depth | 0x08000000;
                        W(prim,0x14) = (u32)W(PTR(node,0),0x14) >> 8;
                    } else gPrimPoolNext -= 0x34;
                } else gPrimPoolNext -= 0x34;
            }
next_node:
            node = PTR(node,0);
        }
advance:
        obj = PTR(obj,0);
    }
}
