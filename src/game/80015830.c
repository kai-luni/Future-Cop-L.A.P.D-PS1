// Categories: Graphics > Model Renderer > Part Geometry | Graphics > Model Renderer > Primitives | Graphics > 2D Drawing
#include "game/types.h"
typedef struct { s16 x,y,z,pad; } Point;
typedef struct { u8 pad[4]; u8 a,b,c,d; } Input;
typedef struct { s16 xy[8]; s32 z; } Output;
extern Point *gPartVertexBuffer __asm__("D_8009F578");
extern u8 *gClipFlagBuffer __asm__("D_8009F57C");
/**
 * @brief Builds the screen-space quad of a type-7 model part: two projected
 *        vertices widened by a per-end width that scales with 1/z.
 *
 * @param in Part header whose byte +0x04 is the first endpoint's vertex index,
 *        +0x05 the second's, +0x06 the width index used at the first endpoint
 *        and +0x07 the width index used at the second.
 * @param out Work block: the four corners as eight s16 values at +0x00..+0x0E
 *        (x0, y0, x1, y1, x2, y2, x3, y3) and the depth at +0x10.
 * @param width Width table; element [index + 2] belongs to vertex index
 *        @c index, which matches the s16 data array that starts at +0x04 of the
 *        table the caller passes.
 *
 * @return 1 when the quad was written, 0 when either endpoint's visibility byte
 *         in D_8009F57C has bit 0 set.
 *
 * @par Confirmed
 * p = D_8009F578 + (in->a << 3) and q = D_8009F578 + (in->b << 3), records of
 * four s16 (x, y, z, pad). output->z = (p->z + q->z) / 2. The half sizes are
 * (width[c + 2] << 9) / p->z and (width[d + 2] << 9) / q->z, the same 0x200
 * perspective scale the billboard case of func_80015B68 uses. The perpendicular
 * of the edge is (q->y - p->y, p->x - q->x); its length is the octagonal
 * approximation (swap to a >= b, then a + (b >> 2) - (b >> 6) with the 0x7F/0x20
 * correction). With a non-zero length scale1 = (w1 << 10) / length and
 * scale2 = (w2 << 10) / length; the corners are p + (dy, dx) * scale1,
 * p - (dy, dx) * scale1, q + (dy, dx) * scale2 and q - (dy, dx) * scale2, all
 * shifted right by 10. A zero length zeroes all four offsets.
 *
 * @par Likely role
 * The line-shaped primitive of the model display list, i.e. case 7 of the
 * per-part switch func_80015B68: unlike the square billboard of types 5/6 it is
 * rotated along the projected edge and tapers by a different width at each end,
 * which is what rails, cables, wires and beam effects need. Both widths are
 * half-sizes, as in the billboard case.
 *
 * @par Unknown
 * Whether the widths are meant as half or full sizes, what the depth bias the
 * caller passes as its fourth argument is for (this body has only three
 * parameters and ignores it), and whether the part's +0x0F/+0x10 bytes carry
 * anything for this type - they are never read here.
 *
 * @par Callers
 * func_80015B68, case 7 of its part-type switch (the only direct call site).
 *
 * @par Environment path
 * Builds the screen-space quad of a type-7 model part from two transformed
 * vertices: it rejects the part when either endpoint's clip byte in
 * D_8009F57C has bit 0 set, takes the half width of each end from width[index
 * + 2] scaled by 0x200 / z, and offsets both endpoints along the normal of
 * the projected edge rescaled with the octagonal length approximation (larger
 * axis plus a quarter of the smaller, minus a 64th). The depth written to
 * out+0x10 is the mean of the two vertex depths. It emits no packet itself;
 * its only call site is case 7 of gPrepareModelPart (func_80015B68, line
 * 193), one level below gSubmitModelInstance (func_80017508) through
 * gDrawModelInstanceParts (func_80016A64), so it is part-geometry preparation
 * on the model render path.
 */
s32 func_80015830(Input *in, Output *out, s16 *width)
{
    register Point *p __asm__("$9");
    register Point *q __asm__("$10");
    register Output *result __asm__("$11") = out;
    s32 w1;
    register s32 w2 __asm__("$12");
    register s32 dx __asm__("$4"), dy __asm__("$6");
    register s32 a __asm__("$8"), b __asm__("$5");
    s32 t,ic,pz;
    register s32 qz __asm__("$5");
    register s32 x1 __asm__("$3"), y1 __asm__("$5"), x2 __asm__("$6"), y2 __asm__("$4");
    register s32 scale1 __asm__("$5"), scale2 __asm__("$7");
    register s32 dist __asm__("$3");
    register s32 num __asm__("$2");
    register s32 widthfirst __asm__("$5");
    {
      register s32 off __asm__("$3");
      s32 ia = in->a;
      register s32 ib __asm__("$5") = in->b;
      u8 *flags = gClipFlagBuffer;
      s32 fa = flags[ia], fb = flags[ib];
      __asm__ volatile ("" : "=r" (fa), "=r" (fb), "=r" (result), "=r" (ib) : "0" (fa), "1" (fb), "2" (result), "3" (ib));
      p = gPartVertexBuffer;
      __asm__ volatile ("" : "=r" (p) : "0" (p));
      if ((fa | fb) & 1) return 0;
      num = ib << 3;
      __asm__ volatile ("" : "=r" (num) : "0" (num));
      q = (Point *)((u8 *)p + num);
      off = ia << 3;
      __asm__ volatile ("" : "=r" (off) : "0" (off));
      ic = in->c;
      __asm__ volatile ("" : "=r" (q), "=r" (ic) : "0" (q), "1" (ic));
      p = (Point *)((u8 *)p + off);
    }
    pz = p->z;
    widthfirst = width[ic + 2];
    __asm__ volatile ("" : "=r" (widthfirst) : "0" (widthfirst));
    num = widthfirst << 9;
    __asm__ volatile ("" : "=r" (num) : "0" (num));
    w1 = num / pz;
    __asm__ volatile ("" : "=r" (w1) : "0" (w1));
    {
    register s32 widthsecond __asm__("$7"), sum __asm__("$3");
    register s16 *wp __asm__("$2");
    qz = q->z;
    wp = (s16 *)((in->d << 1) + (u32)width);
    sum = pz + qz;
    __asm__ volatile ("" : "=r" (wp), "=r" (sum) : "0" (wp), "1" (sum));
    widthsecond = wp[2];
    __asm__ volatile ("" : "=r" (widthsecond), "=r" (qz) : "0" (widthsecond), "1" (qz));
    result->z = sum / 2;
    {
        register s32 qy __asm__("$6") = q->y;
        register s32 py __asm__("$3") = p->y;
        register s32 px __asm__("$4") = p->x;
        register s32 qx __asm__("$2") = q->x;
        __asm__ volatile ("" : "=r" (qy), "=r" (py), "=r" (px), "=r" (qx) : "0" (qy), "1" (py), "2" (px), "3" (qx));
        dy = qy - py;
        dx = px - qx;
    }
    num = widthsecond << 9;
    __asm__ volatile ("" : "=r" (num) : "0" (num));
    w2 = num / qz;
    }
    if (dy < 0) {
        __asm__ volatile ("" : "=r" (dy) : "0" (dy));
        a = -dy;
    } else a = dy;
    if (dx < 0) {
        __asm__ volatile ("" : "=r" (dx) : "0" (dx));
        b = -dx;
    } else b = dx;
    if (a < b) { a ^= b; b ^= a; a ^= b; }
    if ((a >> 1) < b) {
        t = a + b;
        t = (t << 6) + (t << 3) + (t << 1) + a + (b << 5);
        b = a - b;
        a = t >> 7;
    }
    num = a + (b >> 2);
    __asm__ volatile ("" : "=r" (num) : "0" (num));
    dist = num - (b >> 6);
    if (dist > 0) {
        num = w1 << 10;
        __asm__ volatile ("" : "=r" (num) : "0" (num));
        scale1 = num / dist;
        num = w2 << 10;
        __asm__ volatile ("" : "=r" (num) : "0" (num));
        scale2 = num / dist;
        x1 = dy * scale1;
        __asm__ volatile ("" : "=r" (x1) : "0" (x1));
        num = dx * scale1;
        __asm__ volatile ("" : "=r" (num) : "0" (num));
        x2 = dy * scale2;
        __asm__ volatile ("" : "=r" (x2) : "0" (x2));
        y2 = dx * scale2;
        x1 >>= 10;
        y1 = num >> 10;
        x2 >>= 10;
        y2 >>= 10;
    } else { x1 = x2 = y1 = y2 = 0; }
    result->xy[0] = p->x + x1;
    result->xy[1] = p->y + y1;
    result->xy[2] = p->x - x1;
    result->xy[3] = p->y - y1;
    result->xy[4] = q->x + x2;
    result->xy[5] = q->y + y2;
    result->xy[6] = q->x - x2;
    result->xy[7] = q->y - y2;
    return 1;
}
