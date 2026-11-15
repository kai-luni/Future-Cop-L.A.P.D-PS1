// Categories: Graphics > GPU Primitives > Textured Quad | Graphics > 2D Drawing
#include "game/types.h"

struct Poly {
    u32 addr : 24;
    u32 len : 8;
    u8 r, g, b, code;
    s16 xy[8];
    u8 pad[0x1C];
};

extern struct Poly *D_8009F9F0;
extern struct Poly *D_8009FA28;

/**
 * @brief Emits a four-corner quad from eight explicit coordinates.
 *
 * @param x0..y3 The four corners as separate x/y values.
 * @param link Ordering-table link.
 * @param r, g, b Colour.
 * @param code Primitive code; emitted as `code | 0x28`.
 * @return Nothing; the 0x34-byte packet comes from D_8009F9F0.
 *
 * @par Confirmed
 * Length 5, code `code | 0x28`, the eight half-words written in the given order
 * (no sorting), `addr = link`.
 *
 * @par Compare
 * func_80082EF0/`80082FA4 take a rectangle; this form exists for shapes whose
 * corners are already projected individually.
 */
void func_80083060(s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3, u32 link, s32 r, s32 g, s32 b, s32 code)
{
    struct Poly *p;

    if (D_8009F9F0 < D_8009FA28) goto L_alloc;
    __asm__("");
    p = D_8009F9F0;
    goto L_done;
L_alloc:
    p = D_8009F9F0;
    D_8009F9F0 = (struct Poly *)((u8 *)p + 0x34);
L_done:

    p->len = 5;
    p->code = code | 0x28;
    p->r = r;
    p->g = g;
    p->b = b;
    p->xy[0] = x0;
    p->xy[1] = y0;
    p->xy[2] = x1;
    p->xy[3] = y1;
    p->xy[4] = x2;
    p->xy[5] = y2;
    p->xy[6] = x3;
    p->xy[7] = y3;
    p->addr = link;
}
