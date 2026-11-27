// Categories: Graphics > GPU Primitives > Flat Quad | Graphics > 2D Drawing
#include "game/types.h"

struct Poly {
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    s16 xy[8];
    u8 pad[0x1C];
};
extern struct Poly *gPrimPoolNext __asm__("D_8009F9F0");
extern struct Poly *gPrimPoolEnd __asm__("D_8009FA28");
void gEmitFlatQuadRectMode(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b, s32 code) __asm__("func_80082FA4");


/**
 * @brief Emits a flat-shaded quad (POLY_F4 family) with a caller-selected
 *        primitive code.
 *
 * @param x, y, w, h Rectangle.
 * @param link Ordering-table link.
 * @param r, g, b Single colour for all four corners.
 * @param code Primitive code; the emitted byte is `code | 0x28`, so the low bits
 *        add the semi-transparency/texture bits of the 0x28 family.
 * @return Nothing; packet from D_8009F9F0.
 *
 * @par Confirmed
 * Same body as func_80082EF0, only the code byte differs: 0x28 alone is
 * POLY_F4 (opaque), the caller's 2 makes it 0x2A, the semi-transparent polygon
 * (func_8007CE04 does exactly that). Untextured and unshaded either way.
 *
 * @par Likely role
 * The variant the 3D layer uses for blended quads (0x2C) versus opaque (0x28).
 *
 * @par Environment path
 * Takes a 0x34-byte packet from the gPrimPoolNext pool and writes length 5,
 * code (code or 0x28) with the rgb colour, and the four corners (x,y),
 * (x+w,y), (x,y+h), (x+w,y+h), with the caller link in the low 24 bits. It is
 * a POLY_F4 (untextured, single colour), with bit 1 of the code selecting
 * semi-transparency. Callers include func_8007CE04 (code 2), func_8007D138,
 * func_8007D150 and func_8007D27C.
 */
void gEmitFlatQuadRectMode(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b, s32 code)
{
    struct Poly *p;
    s32 x0 = x;
    s32 x1 = x0 + w;
    s32 y2 = y + h;

    __asm__("" : : "r"(x1));

    if (gPrimPoolNext < gPrimPoolEnd) goto L_alloc;
    __asm__("");
    p = gPrimPoolNext;
    goto L_done;
L_alloc:
    p = gPrimPoolNext;
    gPrimPoolNext = (struct Poly *)((u8 *)p + 0x34);
L_done:
    p->len = 5;
    p->code = code | 0x28;
    p->r = r;
    p->g = g;
    p->b = b;
    p->xy[0] = x0;
    p->xy[1] = y;
    p->xy[2] = x1;
    p->xy[3] = y;
    p->xy[4] = x0;
    p->xy[5] = y2;
    p->xy[6] = x1;
    p->xy[7] = y2;
    p->addr = link;
}
