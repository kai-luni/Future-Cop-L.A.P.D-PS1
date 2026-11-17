// Categories: Graphics > Effects > Disc Primitives | Graphics > Renderer > Primitive Submission | Utilities > Mathematics > Trigonometry
#include "game/types.h"

struct Src {
    s16 x;
    s16 y;
    s16 len;
};

struct Poly {
    union {
        u32 word;
        struct {
            u8 b0;
            u8 b1;
            u8 b2;
            u8 b3;
        } byte;
    } color;
    u32 f04;
    s16 x0;
    s16 y0;
    u32 f0C;
    s16 x1;
    s16 y1;
    u32 f14;
    s16 x2;
    s16 y2;
};

extern u8 *D_8009FA90;
extern u8 * volatile gPacketLimit __asm__("D_8009F9C0");
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

/**
 * @brief Emits the rotating variant of the translucent disc: one draw-mode
 *        packet followed by a ring of semi-transparent Gouraud triangles whose
 *        colours are swept around the centre.
 *
 * This is the sibling of func_8001484C (same packet form, same constants); the
 * difference is that each triangle of a quadrant is built from an angle that is
 * advanced per iteration, so the colour split rotates instead of staying fixed.
 *
 * @param src  Centre and depth: s16 x at +0x0, s16 y at +0x2, s16 depth at
 *             +0x4. The +0x4 field is read with lh and is the same value that
 *             ends up in the low 24 bits of the packet tag, so it behaves as a
 *             depth and not as a length.
 * @param p1   Colour word for the two outer vertices; only the top 24 bits are
 *             used (p1 >> 8), stored at +0xC and +0x14 of every packet.
 * @param p2   Polygon command colour: (p2 >> 8) | 0x32000000 is stored at +0x4
 *             of every triangle packet.
 * @param p3   Requested triangle count; rounded up to the next multiple of four
 *             (minimum four) before use.
 * @param p4   Radius scale, pre-multiplied by 0x200 and divided by the depth.
 * @param p5   Near-depth offset: subtracted from the depth for the early-out
 *             test and for the tag word.
 * @param p6   Angle and sub-angle shift packed as (angle << 8) | shift; the
 *             shift is the extra right shift applied to the radius, so the
 *             caller encodes a fractional angle step in the low byte.
 *
 * @return Nothing.
 *
 * @par Globals
 * - D_8009FA90 (0x8009FA90): the GPU packet cursor, loaded and stored eight
 *   times per call.
 * - D_8009F9C0 (0x8009F9C0): the packet limit, declared volatile in the
 *   translation unit; every allocation compares the cursor against it.
 *
 * @par Control flow
 * Confirmed, in order:
 * 1. 0x80014490: frame is 0x50 bytes. guard = src->depth - p5; if guard <= 0
 *    the function returns from 0x80014818 without touching any global.
 * 2. quotient = (p4 << 9) / src->depth (sll by 9 at 0x4CD8, hardware div at
 *    0x4CDC including the divide-by-zero and overflow break 7 / break 6 guards).
 * 3. Allocate an 0x1C-byte header packet: if the cursor is below the limit it
 *    is advanced by 0x1C. The packet gets 0xE10006A0 at +0x4, 0x01 in the top
 *    byte of word 0 and (depth - p5) & 0xFFFFFF in the low 24 bits.
 * 4. count = max(quotient << 2, 4); if count < p3 the count is raised to p3
 *    (the sra by 31 followed by and at 0x4D84..0x4D8C adds min(0, count - p3),
 *    i.e. a signed max against zero).
 * 5. A do-while loop fills (p3 - 1) further packets, each 0x1C bytes:
 *    word 0 = (depth - p5) | 0x06000000, +0x4 = 0x32000000 | (p2 >> 8),
 *    +0xC = +0x14 = p1 >> 8. One read of the cursor and one store per packet,
 *    and the cursor is only advanced while it is below the limit.
 * 6. The loop rewinds to the first filled packet and derives four cursors at
 *    p3/4, p3/2 and 3*p3/4 (the *7*4 arithmetic is the 0x1C stride).
 * 7. step = 0x20000 / p3; for i = 1 while p3/4 >= i: one call to
 *    func_8001846C with angle + (i * step) / 32, then four triangles are
 *    written from the sine/cosine pair, each with six vertex half-words
 *    (x0,y0,x1,y1,x2,y2) stored at a base offset of +0x1A and advanced by 0x1C.
 *    sin and cos are multiplied by the quotient, shifted right by 16 and then
 *    by the low byte of p6, which is what turns the scale into a radius.
 * 8. Within one triangle the second and third vertices are point-reflections of
 *    each other about (sx, sy), e.g. (sx - vb, sy + va) and (sx + vb, sy - va),
 *    so every triangle spans the centre and the disc closes.
 *
 * @par Calls
 * func_8001846C (sine/cosine from the 4096-step table), once per iteration -
 * the only call in the function.
 *
 * @par Side effects
 * Appends 1 + (p3 - 1) packets of 0x1C bytes (a header plus the triangle
 * payload) to the packet area and moves D_8009FA90 accordingly. When the
 * cursor is already at or above D_8009F9C0 the code still writes the packet
 * body but does not advance the cursor, so the last slot is overwritten
 * instead of overrun.
 *
 * @par Likely role
 * A soft circular glow (light pool, shadow blob or muzzle flash) drawn in
 * screen space. Evidence: the packet form and every constant match
 * func_8001484C, and func_80070158 selects between the two on a node flag
 * (bit 0x2 calls func_8001484C, bit 0x4 calls this function) while passing
 * node->unk78 / node->unk7C as the two colours, node->unk8E as the count,
 * node->unk12 as the scale, 0x200 as the depth offset and
 * (node->unk90 << 8) | node->unk8F as the packed angle.
 * Interpretation that is not confirmed: 0x32 is read as the GP0 opcode for a
 * semi-transparent Gouraud triangle, which fixes the per-vertex colour order.
 *
 * @par Unknown
 * - Why the low byte of p6 is applied as a shift rather than as an angle
 *   fraction; only the effect (a smaller radius) is visible in the code.
 * - Whether the two colours are fog-shaded before the call or used verbatim.
 *
 * @par Matching note
 * The $3/$2/$6/$4 register pins and the empty asm statements only exist to
 * reproduce the retail register allocation; the listing marks the function as
 * nonmatching.
 *
 * @par Callers
 * func_80070158, recorded at 0x80070158 (direct call at 0x8007033C).
 */
void func_80014490(struct Src *src, u32 p1, u32 p2, s32 p3, s32 p4, s32 p5, s32 p6)
{
    s32 sine;
    s32 cosine;
    struct Poly *poly;
    struct Poly *q0;
    struct Poly *q1;
    struct Poly *q2;
    struct Poly *q3;
    u8 *cursor;
    register u8 *current __asm__("$3");
    register s32 len __asm__("$8");
    register s32 guard __asm__("$2");
    register s32 dividend __asm__("$3");
    register s32 inner __asm__("$9");
    s32 fp;
    s32 quot;
    s32 angle;
    s32 step;
    s32 shift;
    register s32 argangle __asm__("$13");
    register s32 sx __asm__("$23");
    register s32 sy __asm__("$22");
    register s32 delta __asm__("$3");
    register s32 clipped __asm__("$2");
    register s32 cosprod __asm__("$3");
    register s32 sinprod __asm__("$2");
    register s32 va __asm__("$4");
    register s32 vb __asm__("$3");
    s32 accu;
    s32 i;
    register u32 header __asm__("$3");
    u32 c0;
    u32 c1;
    register u32 c2 __asm__("$6");
    register u16 x0_3 __asm__("$5");
    register u16 y0_3 __asm__("$6");

    quot = p4;
    __asm__("" : "=r" (quot) : "0" (quot));
    inner = p5;
    len = src->len;
    argangle = p6;
    fp = p3;
    guard = len - inner;
    if (guard <= 0) {
        return;
    }
    __asm__("" : "=r" (inner) : "0" (inner));
    dividend = quot << 9;
    quot = dividend / len;
    sx = src->x;
    sy = src->y;

    cursor = D_8009FA90;
    if (cursor < gPacketLimit) {
        D_8009FA90 = cursor + 0x1C;
    }
    poly = (struct Poly *)cursor;
    header = 0xE10006A0;
    poly->f04 = header;
    poly->color.byte.b3 = 1;
    poly->color.word = (poly->color.word & 0xFF000000) | ((len - inner) & 0xFFFFFF);

    i = quot << 2;
    if (i < 4) {
        i = 4;
    }
    delta = i - fp;
    clipped = (delta >> 31) & delta;
    __asm__("" : "=r" (clipped) : "0" (clipped));
    fp = fp + clipped;
    i = fp - 1;

    c2 = p2 >> 8;
    __asm__("" : "=r" (c2) : "0" (c2));
    c2 |= 0x32000000;
    c1 = p1 >> 8;
    c0 = (len - inner) | 0x06000000;

    do {
        current = D_8009FA90;
        __asm__("" : "=r" (current) : "0" (current));
        if (current >= gPacketLimit) {
            cursor = current;
            __asm__ volatile ("");
            i--;
            goto write_poly;
        }
        cursor = current;
        D_8009FA90 = cursor + 0x1C;
        i--;
write_poly:
        poly = (struct Poly *)cursor;
        poly->color.word = c0;
        poly->f04 = c2;
        poly->f14 = c1;
        poly->f0C = c1;
    } while (i >= 0);

    poly = poly - (fp - 1);
    q1 = poly + fp / 4;
    q2 = poly + fp / 2;
    q3 = poly + (fp * 3) / 4;
    q0 = poly;

    step = 0x20000 / fp;
    accu = step;
    i = 1;
    __asm__("" : "=r" (argangle) : "0" (argangle));
    angle = argangle >> 8;
    shift = argangle & 0xFF;
    while (fp / 4 >= i) {
        func_8001846C(angle + accu / 32, &sine, &cosine);
        sinprod = cosine;
        cosprod = sinprod * quot;
        va = cosprod >> 16;
        cosine = va;
        va = va >> shift;
        sinprod = sine * quot;
        vb = sinprod >> 16;
        sine = vb;
        vb = vb >> shift;
        i++;
        accu = accu + step;
        q0->x0 = cosine + sx;
        q0->y0 = sine + sy;
        q0->x1 = sx - vb;
        q0->y1 = sy + va;
        q0->x2 = sx + vb;
        q0->y2 = sy - va;
        q0++;
        q1->x0 = sx - sine;
        q1->y0 = cosine + sy;
        q1->x1 = sx + va;
        q1->y1 = sy + vb;
        q1->x2 = sx - va;
        q1->y2 = sy - vb;
        q1++;
        q2->x0 = sx - cosine;
        q2->y0 = sy - sine;
        q2->x1 = sx + vb;
        q2->y1 = sy - va;
        q2->x2 = sx - vb;
        q2->y2 = sy + va;
        q2++;
        x0_3 = sine + sx;
        y0_3 = sy - cosine;
        q3->x0 = x0_3;
        q3->y0 = y0_3;
        q3->x1 = sx - va;
        q3->y1 = sy - vb;
        q3->x2 = sx + va;
        q3->y2 = sy + vb;
        q3++;
    }
}
