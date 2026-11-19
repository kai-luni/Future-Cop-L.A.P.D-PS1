// Categories: Graphics > HUD > Selection Frame | Graphics > GPU Primitives > Gouraud Quad
#include "game/types.h"

extern volatile s32 gFrameTickCounter __asm__("D_8009FA2C");
extern u8 gHighlightShadeTable[8] __asm__("D_8009F3F8");
extern void gEmitFlatQuadRect(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b) __asm__("func_80082EF0");
void gDrawBlinkSelectFrame(s32 x, s32 y, s32 w, s32 h) __asm__("func_80083280");


/**
 * @brief Draws a 2-pixel selection frame centred on a rectangle, in the
 *        cycling palette colour.
 *
 * @param x, y Centre of the frame.
 * @param w, h Inner width/height (the frame is w + 1 wide).
 * @return Nothing.
 *
 * @par Confirmed
 * The colour is D_8009F3F8[(D_8009FA2C >> 4) & 7] + 0x80 (the same palette
 * cycle the HUD uses). Four gouraud rectangles at link 0x12C form the top,
 * left, right and bottom borders, each 2 pixels thick, around
 * (x - (w + 1) / 2, y - h / 2).
 *
 * @par Likely role
 * The cursor frame of a front-end widget: the palette cycle makes the
 * selection blink without redrawing the content.
 *
 * @par Callers
 * No direct callers in the call graph and no address reference in the
 * sources; dispatch unconfirmed.
 *
 * @par Environment path
 * 2D overlay helper: computes a grey level
 * gHighlightShadeTable[(gFrameTickCounter shifted right 4) and 7] + 0x80 and
 * draws the top, left, right and bottom 2-pixel borders of a w + 1 by h frame
 * centred on x,y as four opaque POLY_F4 rectangles (code 0x28) via
 * func_80082EF0, all with ordering link 0x12C. No caller references this
 * address in the sources, so how it is reached is unconfirmed.
 */
void gDrawBlinkSelectFrame(s32 x, s32 y, s32 w, s32 h)
{
    register s32 w1 __asm__("$19") = w + 1;
    s32 x0 = x - (w1 >> 1);
    s32 y0 = y - (h >> 1);
    s32 y1 = y0 + h;
    s32 idx;
    s32 color;
    register s32 t __asm__("$2");
    register u8 *pal __asm__("$3");

    t = gFrameTickCounter;
    __asm__("lui %0,%%hi(D_8009F3F8)\n\taddiu %0,%0,%%lo(D_8009F3F8)"
            : "=r"(pal) : "r"(t));
    idx = (t >> 4) & 7;
    color = *(u8 *)(idx + (s32)pal) + 0x80;

    gEmitFlatQuadRect(x0, y0 - 2, w1, 2, 0x12C, color, color, color);
    gEmitFlatQuadRect(x0, y0, 2, h, 0x12C, color, color, color);
    gEmitFlatQuadRect(x0 + w1 - 2, y0, 2, h, 0x12C, color, color, color);
    gEmitFlatQuadRect(x0, y1, w1, 2, 0x12C, color, color, color);
}
