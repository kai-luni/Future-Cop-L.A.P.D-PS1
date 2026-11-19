// Categories: Graphics > Model Renderer > Gouraud Quad | Graphics > Effects
#include "game/types.h"

typedef struct {
    union { u32 word; u8 bytes[4]; } hdr;
    u8 c0, c1, c2, c3;
    s16 x0, y0;
    u8 a0, a1, a2;
    s16 x1, y1;
    u8 e0, e1, e2;
    s16 x2, y2;
    u8 d0, d1, d2;
    s16 x3, y3;
} Prim;

extern Prim *gPrimPoolNext __asm__("D_8009F9F0");
extern Prim *gPrimPoolEnd __asm__("D_8009FA28");
extern u8 gFrameTime __asm__("D_8009FAA4");

extern void gEmitTranslucentTileRect(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend) __asm__("func_8001C214");
void gEmitHudGouraudBar(s32 x, s32 y_in, s32 z, s32 p3, s32 p4, s32 p5, s32 flag) __asm__("func_80047BE8");


/**
 * @brief Draws a vertical gouraud quad (light beam / glow column).
 *
 * @param x, y_in Screen position.
 * @param p3, p4, p5 The three colour components of the top edge.
 * @param z, width, flag Extra geometry and a "halve when busy" switch.
 * @return Nothing.
 *
 * @par Confirmed
 * 1. A backdrop rectangle is drawn first with func_8001C214(0x2000, x*2,
 *    y*2 - 0x60, 0x10, 0x60, 1) - the blended overlay primitive.
 * 2. The colours are scaled by 2 and divided by 4; when the flag is set and
 *    D_8009FAA4 >= 0x80 they are halved again (the "many effects on screen"
 *    dimming).
 * 3. The quad packet (length 8, code 0x38) gets x on both top and bottom edges,
 *    y - z on the top, the three colours in both edges (c0..c2 and a0..a2/d0..d2)
 *    - a vertical strip whose two ends share the colour.
 *
 * @par Likely role
 * The energy/smoke column effect: one quad per effect, with the backdrop adding
 * the soft halo.
 *
 * @par Environment path
 * Draws a vertical HUD health/energy gauge at (x, y_in) with height (z+1)/2
 * (called by func_80047F30). Emits a blended backdrop via func_8001C214, two
 * side-by-side 4px-wide Gouraud quads (code 0x38) into
 * gPrimPoolNext/gPrimPoolEnd with color halved when flag != 0 and gFrameTime
 * >= 0x80, and five white tick rectangles via func_8001C214.
 */
void gEmitHudGouraudBar(s32 x, s32 y_in, s32 z, s32 p3, s32 p4, s32 p5, s32 flag)
{
    Prim *prim;
    register s32 y __asm__("$22");
    s32 t3, t2, t1;
    register s32 yy __asm__("$18");
    register s32 xx __asm__("$17");
    s32 right;
    register s32 xarg __asm__("$5");
    register s32 draw_color __asm__("$4");
    register s32 width __asm__("$7");
    register s32 color __asm__("$2");
    u32 w;

    y = y_in;
    gEmitTranslucentTileRect(0x2000, x * 2, y * 2 - 0x60, 0x10, 0x60, 1);

    p3 = p3 * 2;
    p4 = p4 * 2;
    p5 = p5 * 2;
    t3 = p3 / 4;
    t2 = p4 / 4;
    t1 = p5 / 4;

    if (flag != 0) {
        s32 v = gFrameTime;
        if (v >= 0x80) {
            p3 /= 2;
            p4 /= 2;
            p5 /= 2;
            t3 /= 2;
            t2 /= 2;
            t1 /= 2;
        }
    }

    z = (z + 1) / 2;

    if (gPrimPoolNext < gPrimPoolEnd) goto L_alloc1;
    prim = gPrimPoolNext;
    goto L_done1;
L_alloc1:
    prim = gPrimPoolNext;
    gPrimPoolNext = (Prim *)((u8 *)prim + 0x34);
L_done1:
    prim->x0 = x;
    prim->x2 = x;
    prim->hdr.bytes[3] = 8;
    prim->c3 = 0x38;
    prim->y0 = y - z;
    prim->y1 = y - z;
    prim->c0 = t3;
    prim->c1 = t2;
    prim->c2 = t1;
    prim->a0 = p3;
    prim->a1 = p4;
    prim->a2 = p5;
    prim->d0 = p3;
    prim->d1 = p4;
    prim->d2 = p5;
    prim->e0 = t3;
    prim->e1 = t2;
    prim->e2 = t1;
    prim->x1 = x + 4;
    prim->x3 = x + 4;
    x = x + 4;
    prim->y3 = y;
    prim->y2 = y;
    prim->hdr.word = prim->hdr.word & 0xFF000000;

    if (gPrimPoolNext < gPrimPoolEnd) goto L_alloc2;
    prim = gPrimPoolNext;
    goto L_done2;
L_alloc2:
    prim = gPrimPoolNext;
    gPrimPoolNext = (Prim *)((u8 *)prim + 0x34);
L_done2:
    right = x + 4;
    prim->x0 = x;
    prim->x2 = x;
    x = x - 2;
    draw_color = 0x7FFF;
    __asm__ volatile ("" : "=r" (draw_color) : "0" (draw_color));
    xx = x * 2;
    xarg = xx;
    __asm__ volatile ("" : "=r" (xarg) : "0" (xarg));
    yy = y * 2;
    __asm__ volatile ("" : : : "memory");
    width = 8;
    __asm__ volatile ("" : "=r" (color) : "0" (width), "r" (xx), "r" (yy));
    prim->hdr.bytes[3] = color;
    prim->c3 = 0x38;
    prim->y0 = y - z;
    prim->y1 = y - z;
    prim->y3 = y;
    prim->y2 = y;
    prim->c0 = p3;
    prim->c1 = p4;
    prim->c2 = p5;
    prim->a0 = t3;
    prim->a1 = t2;
    prim->a2 = t1;
    prim->d0 = t3;
    prim->d1 = t2;
    prim->d2 = t1;
    prim->e0 = p3;
    prim->e1 = p4;
    prim->e2 = p5;
    prim->x1 = right;
    prim->x3 = right;
    prim->hdr.word = prim->hdr.word & 0xFF000000;

    gEmitTranslucentTileRect(draw_color, xarg, yy - 0x60, width, 2, 0);
    gEmitTranslucentTileRect(0x7FFF, xx, yy - 0x30, 8, 2, 0);
    gEmitTranslucentTileRect(0x7FFF, xx, yy - 2, 8, 2, 0);
    __asm__ volatile ("" : "=r" (x) : "0" (x));
    x = x + 1;
    gEmitTranslucentTileRect(0x7FFF, x * 2, yy - 0x48, 4, 2, 0);
    gEmitTranslucentTileRect(0x7FFF, x * 2, yy - 0x18, 4, 2, 0);
}
