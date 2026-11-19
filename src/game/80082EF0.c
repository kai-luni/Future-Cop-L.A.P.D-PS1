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
void gEmitFlatQuadRect(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b) __asm__("func_80082EF0");


/**
 * @brief Emits an axis-aligned flat-shaded quad (POLY_F4, code 0x28) from a
 *        rectangle.
 *
 * @param x, y Top-left corner.
 * @param w, h Size.
 * @param link Ordering-table link value (stored in the top three bytes).
 * @param r, g, b Single colour for all four corners.
 * @return Nothing; the 0x34-byte packet comes from D_8009F9F0 (the last slot is
 *         reused when the pool is exhausted).
 *
 * @par Confirmed
 * Length 5, code 0x28 - the fourth of the seven 0x20-family opcodes, i.e. an
 * untextured, unshaded quad, not a Gouraud quad (0x38) - with one rgb for the
 * whole primitive, the four corners in the order (x, y), (x+w, y), (x, y+h),
 * (x+w, y+h), and `addr = link`.
 *
 * @par Compare
 * func_80082FA4 is the same with a caller-supplied code byte (OR-ed with 0x28).
 *
 * @par Environment path
 * Takes a 0x34-byte packet from the gPrimPoolNext pool (overwriting the last
 * slot when gPrimPoolEnd is reached) and builds an untextured single-colour
 * quad: code 0x28 with rgb, corners (x,y), (x+w,y), (x,y+h), (x+w,y+h) and
 * the caller ordering link in the low 24 bits. Callers include func_8007ABBC,
 * func_8007C0F8, func_8007D7CC, func_80080038 and func_80083280.
 */
void gEmitFlatQuadRect(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b)
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
    p->code = 0x28;
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
