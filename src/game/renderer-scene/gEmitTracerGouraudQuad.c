// Categories: Graphics > Model Renderer > Primitives | Graphics > GPU Primitives
#include "game/types.h"
typedef struct {
    u16 vx;
    u16 vy;
} DVECTOR;

typedef struct {
    union { u32 word; u8 bytes[4]; } hdr;
    u32 unk04;
    s16 x0, y0;
    u32 unk0C;
    s16 x1, y1;
    u32 unk14;
    s16 x2, y2;
    u32 unk1C;
    s16 x3, y3;
    u8 pad24[0x34 - 0x24];
} Prim;
extern Prim *gPrimPoolNext __asm__("D_8009F9F0");
extern Prim *gPrimPoolEnd __asm__("D_8009FA28");
void gEmitTracerGouraudQuad(DVECTOR v0, DVECTOR v1, DVECTOR v2, DVECTOR v3, s32 dx, s32 dy_in, u32 unk04, u32 unk1C, u16 tag_in) __asm__("func_8006F21C");


/**
 * @brief Emits a 52-byte semi-transparent Gouraud quad packet (code 0x3A)
 *        from four screen vertices.
 *
 * @param v0..v3 The four vertices as 16-bit pairs.
 * @param dx, dy Offset applied to the first two vertices (the far edge of the
 *        band), while v2/v3 are also stored unshifted as the near edge.
 * @param unk04, unk1C Words written into the packet's two spare slots.
 * @param tag_in The 16-bit part of the ordering-table link tag.
 * @return Nothing; the packet is taken from the D_8009F9F0 pool (last slot when
 *         full).
 *
 * @par Confirmed
 * Length byte 8, the four XY pairs written in the order x2/y2 (v0), x0/y0 (v0+dx,
 * v0+dy), x1/y1 (v2+dx, v2+dy) and x3/y3 (v2), unk14/unk0C zeroed, and the tag
 * word rebuilt as (old & 0xFF000000) | (tag & 0xFFFFFF). There is no texpage or
 * UV word in the packet: length 8 is one code plus four colour vertices, i.e. a
 * single-colour-per-vertex Gouraud quad, and the caller supplies 0x3A (semi
 * transparent, Gouraud, untextured).
 *
 * @par Likely role
 * The quad emitter of the model renderer: the same four vertices are used twice,
 * once offset and once raw, so one packet carries both the far edge (offset) and
 * the near edge of the band, fading from a caller colour to black.
 *
 * @par Callers
 * func_8006F310 (the model draw entry).
 *
 * @par Environment path
 * Takes a 0x34-byte packet from the gPrimPoolNext pool, sets length 8 and the
 * low 24 link bits to the tag argument, then lays out a Gouraud quad: rgb0
 * plus code (from the caller) with xy0 = v0 + d, rgb1 = 0 with xy1 = v2 + d,
 * rgb2 = 0 with xy2 = v0, rgb3 = caller value with xy3 = v2; the v1 and v3
 * parameters are unused. func_8006F310 calls it four times (offset plus and
 * minus d, for two endpoint pairs) right after emitting texpage E10006A0
 * (blend bits 5-6 = 1, additive), so the quads form an additive band that
 * fades to black along the offset edge.
 */
void gEmitTracerGouraudQuad(DVECTOR v0, DVECTOR v1, DVECTOR v2, DVECTOR v3, s32 dx, s32 dy_in, u32 unk04, u32 unk1C, u16 tag_in) {
    register s32 dy __asm__("$8") = dy_in;
    register u16 tag __asm__("$7") = tag_in;
    Prim *prim;

    if (gPrimPoolNext < gPrimPoolEnd) goto L_alloc;
    __asm__("");
    prim = gPrimPoolNext;
    goto L_done;
L_alloc:
    prim = gPrimPoolNext;
    gPrimPoolNext = (Prim *)((u8 *)prim + 0x34);
L_done:

    prim->hdr.bytes[3] = 8;
    prim->unk04 = unk04;
    prim->unk1C = unk1C;
    prim->unk14 = 0;
    prim->unk0C = 0;

    prim->x2 = v0.vx;
    prim->y2 = v0.vy;
    {
        register u32 mask __asm__("$5") = 0x00FFFFFF;
        register s32 v __asm__("$2");
        s32 y3;
        v = v0.vx + dx; prim->x0 = v;
        v = v0.vy + dy; prim->y0 = v;
        v = v2.vx + dx; prim->x1 = v;
        v = v2.vy + dy; prim->y1 = v;
        v = v2.vx;      prim->x3 = v;
        v = (s16)tag;
        y3 = v2.vy;
        prim->hdr.word = (prim->hdr.word & 0xFF000000) | (v & mask);
        prim->y3 = y3;
    }
}
