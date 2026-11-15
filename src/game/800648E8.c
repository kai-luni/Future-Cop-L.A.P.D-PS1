// Categories: Graphics > Renderer > Map Cell Geometry | Graphics > GPU Primitives | Graphics > Clipping
#include "game/types.h"

extern u32 D_8009AFE8[];
extern u32 *D_8009F9F0;
extern u32 *D_8009FA28;
extern u32 D_800A8E80[];
extern u8 D_800A2630[];
extern u16 D_8009F83C;
extern s32 func_800648BC(s32 a, s32 b, s32 c);

typedef struct { s32 w0; s16 h4; u8 b6; u8 b7; } V;

/**
 * @brief Emits one precomputed map-cell polygon: decodes the cell's 16-bit
 *        primitive word into three or four vertex indices, applies the
 *        per-vertex visibility and facing tests, takes the average of the four
 *        vertex shading values as the ordering key and writes a textured
 *        triangle or quad packet into the shared primitive pool.
 *
 * @param base  Vertex array base. One vertex occupies 8 bytes: word +0x00 is the
 *              packed screen XY, half-word +0x04 is the shading/depth key, bytes
 *              +0x06 and +0x07 are flag bytes (bit 0 of +0x07 = hidden,
 *              bit 1 = "swallow", bit 2 = "degenerate").
 * @param arg1  The 16-bit primitive word from the cell stream. Bits 15..21 are
 *              the index into D_8009AFE8 (the four-byte vertex index table),
 *              bits 21..31 and 0..9 index the two UV tables, bit 0 is the
 *              "run another primitive" marker, bit 0x800 / 0x1000 are the
 *              clipping / semi-transparency flags used by the caller's filter.
 * @param tbl   Three-word table: tbl[0] is the CLUT source used by the palette
 *              variants, tbl[1] the table of per-primitive descriptor half-words
 *              and tbl[2] the table of UV half-words.
 * @param arg3  Cell sub-index (low nibble = column, high nibble = row) used only
 *              by the D_800A2630 ramp variant of the k == 6 branch, where it is
 *              also advanced by one column and one row to fetch the three
 *              neighbouring ramp values.
 *
 * @return Nothing. Several paths return early (hidden vertex, degenerate quad,
 *         failing facing test, unknown kind), and the k == 6 untextured ramp
 *         branch returns as soon as the packet has been filled.
 *
 * @par Control flow (confirmed)
 * 1. w = D_8009AFE8[(arg1 >> 15) & 0x7F] is split into four byte indices b0, b1,
 *    b2 and w (bits 0..7, 8..15, 16..23, 24..31). Each index selects a vertex
 *    at base + (index << 3); v0 = b0, v1 = b1, v2 = b2, v3 = w.
 * 2. Visibility is decided from byte +0x07 of the four vertices: t is the AND,
 *    f the OR, x2 = v3.b7 & t. Any vertex with bit 0 set cancels the primitive;
 *    when no vertex has bit 1 set and all four have bit 2 set it is cancelled as
 *    well.
 * 3. With bit 0 of @p arg1 set the winding is tested for real:
 *    func_800648BC(v0->w0, v1->w0, v2->w0) is the GTE NCLIP instruction and
 *    returns 1 for a back-facing triangle, which cancels the primitive.
 * 4. The ordering key is the mean of the four +0x04 shading half-words; when
 *    v0 == v3 (a triangle stored as a degenerate quad) it uses the weighted form
 *    (v3 + v1 + v2) * 5 / 16 instead of the plain (v0 + v1 + v2 + v3) / 4.
 * 5. A packet is taken from D_8009F9F0 (0x34 bytes, cursor advanced by 13 words;
 *    the last slot is reused when the cursor has reached D_8009FA28) and its
 *    header word receives the mean in the low 24 bits while byte 3 gets the
 *    length code.
 * 6. p5 = tbl[1] + ((arg1 >> 21) & 0x7FE) is the descriptor half-word and
 *    a1 = tbl[2] + (arg1 & 0x7FE) the UV half-word. When the descriptor has bit
 *    0x800 set, four half-words are read from the UV table, re-based by
 *    D_8009F83C and staged in the scratchpad palette at 0x1F800070, which then
 *    replaces a1 - a per-cell palette animation.
 * 7. a0 = D_800A8E80[(descriptor >> 8) & 7] is the texture page base and
 *    k = descriptor >> 13 selects the packet form:
 *    - k == 0: three vertices, length 7, code 0x24 (flat textured triangle);
 *    - k == 1: four vertices, length 9, code 0x2C (flat textured quad);
 *    - k == 2 / 4 / 6: four vertices, length 9, code 0x34 (gouraud textured),
 *      with per-vertex colours from the gouraud words (k == 2), the CLUT (k == 4)
 *      or the D_800A2630 ramp indexed by the arg3 nibbles and the low three bits
 *      of the descriptor (k == 6, which returns immediately);
 *    - k == 3 / 5 / 7: five corner writes, length 0x0C, code 0x3C (gouraud
 *      textured quad with an extra corner word), same three colour variants.
 *    In all cases bit 1 of the code byte is the semi-transparency flag taken from
 *    descriptor bit 0x1000, which turns 0x24/0x2C/0x34/0x3C into 0x26/0x2E/0x36/
 *    0x3E.
 *
 * @par Globals read/written
 * D_8009AFE8 (4 vertex indices per primitive index, read), D_8009F9F0 /
 * D_8009FA28 (packet cursor and limit), D_800A8E80 (texture page base per
 * (descriptor >> 8) & 7), D_800A2630 (16x16 shading ramp), D_8009F83C (palette
 * phase added to the four staged CLUT entries), 0x1F800070 (scratchpad palette
 * staging area, written).
 *
 * @par Calls
 * func_800648BC only - the GTE NCLIP facing test. Everything else is inline
 * table work.
 *
 * @par Side effects
 * Advances D_8009F9F0 by 0x34 per emitted packet, writes the scratchpad palette
 * at 0x1F800070, and leaves the first word of each packet holding a mean
 * shading value in the field that the packet header declares as a 24-bit
 * address.
 *
 * @par Callers
 * func_80065134 (direct) - the cell stream walker: it keeps a running 16-entry
 * counter over the cell's word list, rewrites bit 0 of the word and the running
 * index, applies its own per-kind depth filter and calls
 * func_800648E8(list + index * 24, word, position).
 *
 * @par Likely
 * The function draws the precomputed ground/level polygons of a map cell: the
 * triangle table D_8009AFE8 maps a primitive code to four vertex indices, the
 * vertex flags carry the hidden/facing bits, and the mean shading value is used
 * as the sort key so that darker polygons are ordered first - the same key
 * convention as the icon emitter func_80065CA8.
 *
 * @par Unknown
 * - The meaning of byte +0x06 of a vertex and of the exact bit assignment of
 *   +0x07 beyond the three bits that are tested.
 * - Why the k == 6 variant returns before the depth/ordering word is written
 *   twice; the branch does write the header first, so the difference is only
 *   that the CLUT stage is skipped.
 * - The reason the caller passes only three arguments while this function
 *   declares four: on the k == 6 path a value left in the fourth argument
 *   register is used as the cell sub-index, so that path depends on the
 *   caller's scratch state.
 */
void func_800648E8(u8 *base, u32 arg1, u32 *tbl, u32 arg3)
{
    u32 w, x0, x1, x2, x3;
    register u32 b0 __asm__("$6");
    register u32 b1 __asm__("$3");
    register u32 b2 __asm__("$5");
    V *v0, *v1, *v2, *v3;
    u32 *pkt;
    s32 avg;
    u16 *pa;
    u16 *pb;
    u32 f;
    u32 col;
    u32 op;
    register u32 t __asm__("$5");
    u8 *o;
    u16 *p5;
    u16 *a1;
    u32 t3;
    u32 a0;
    u32 k;
    u8 c;
    u16 *cl;
    u32 e;

    w = D_8009AFE8[(arg1 >> 15) & 0x7F];
    b0 = w & 0xFF;
    w >>= 8;
    b1 = w & 0xFF;
    w >>= 8;
    b2 = w & 0xFF;
    w >>= 8;
    __asm__("" : "+r"(b0), "+r"(b1), "+r"(b2), "+r"(w));
    v1 = (V *)(base + (b1 << 3));
    v0 = (V *)(base + (b0 << 3));
    v2 = (V *)(base + (b2 << 3));
    x1 = v1->b7;
    x0 = v0->b7;
    t = x0 & x1;
    f = x0 | x1;
    x1 = v2->b7;
    t &= x1;
    f |= x1;
    v3 = (V *)(base + (w << 3));
    x1 = v3->b7;
    f |= x1;
    x2 = x1 & t;
    if (f & 1) return;
    if (!(f & 2) && x2) return;
    if (arg1 & 1) {
        register s32 pa0 __asm__("$4");
        register s32 pa1 __asm__("$5");
        register s32 pa2 __asm__("$6");
        pa0 = v0->w0;
        __asm__ __volatile__("");
        pa1 = v1->w0;
        __asm__ __volatile__("");
        pa2 = v2->w0;
        if (func_800648BC(pa0, pa1, pa2)) return;
    }
    if (v0 == v3) {
        avg = (v3->h4 + v1->h4 + v2->h4);
        avg += v3->h4 + avg * 4;
        avg >>= 4;
    } else {
        avg = (v0->h4 + v1->h4 + v2->h4 + v3->h4) >> 2;
    }
    if (D_8009F9F0 >= D_8009FA28) {
        o = (u8 *)D_8009F9F0;
        __asm__ __volatile__("");
    } else {
        o = (u8 *)D_8009F9F0;
        D_8009F9F0 = (u32 *)o + 13;
    }
    *(u32 *)o = (*(u32 *)o & 0xFF000000) | (avg & 0xFFFFFF);
    p5 = (u16 *)((u8 *)tbl[1] + ((arg1 >> 21) & 0x7FE));
    a1 = (u16 *)((u8 *)tbl[2] + (arg1 & 0x7FE));
    t3 = *p5;
    if (t3 & 0x800) {
        volatile u16 *d = (volatile u16 *)0x1F800070;
        u16 off = D_8009F83C;
        d[0] = ((volatile u16 *)a1)[0] + off;
        d[1] = ((volatile u16 *)a1)[1] + off;
        d[2] = ((volatile u16 *)a1)[2] + off;
        d[3] = ((volatile u16 *)a1)[3] + off;
        a1 = (u16 *)0x1F800070;
    }
    a0 = D_800A8E80[(t3 >> 8) & 7];
    k = t3 >> 13;
    switch (k) {
    case 0:
    case 1:
        c = t3 & 0xFC;
        o[4] = c; o[5] = c; o[6] = c;
        *(u32 *)(o + 0xC) = a1[0] | (a0 & 0xFFFF0000);
        *(u32 *)(o + 0x14) = a1[1] | (a0 << 16);
        *(u32 *)(o + 8) = v0->w0;
        *(u32 *)(o + 0x10) = v1->w0;
        if (k == 0) {
            o[3] = 7;
            o[7] = ((t3 & 0x1000) >> 11) | 0x24;
            *(u32 *)(o + 0x18) = v2->w0;
            *(u16 *)(o + 0x1C) = a1[2];
        } else {
            o[3] = 9;
            o[7] = ((t3 & 0x1000) >> 11) | 0x2C;
            *(u16 *)(o + 0x24) = a1[2];
            *(u16 *)(o + 0x1C) = a1[3];
            *(u32 *)(o + 0x20) = v2->w0;
            *(u32 *)(o + 0x18) = v3->w0;
        }
        break;
    case 2:
    case 4:
    case 6:
        o[3] = 9;
        o[7] = ((t3 & 0x1000) >> 11) | 0x34;
        *(u32 *)(o + 0xC) = a1[0] | (a0 & 0xFFFF0000);
        *(u32 *)(o + 0x18) = a1[1] | (a0 << 16);
        *(u16 *)(o + 0x24) = a1[2];
        *(u32 *)(o + 8) = v0->w0;
        *(u32 *)(o + 0x14) = v1->w0;
        *(u32 *)(o + 0x20) = v2->w0;
        if (k == 2) {
            t3 = (t3 << 16) | p5[1];
            c = (t3 >> 16) & 0xFC;
            o[4] = c; o[5] = c; o[6] = c;
            c = (t3 >> 10) & 0xFC;
            o[0x10] = c; o[0x11] = c; o[0x12] = c;
            c = (t3 >> 4) & 0xFC;
            o[0x1C] = c; o[0x1D] = c; o[0x1E] = c;
            break;
        } else if (k == 4) {
            u32 sh, m;
            cl = (u16 *)tbl[0];
            e = cl[t3 & 0xFF];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[4] = (e >> (8 - sh)) & m;
            o[5] = (e >> (3 - sh)) & m;
            o[6] = (e << (sh + 2)) & m;
            t3 = p5[1];
            e = cl[t3 >> 8];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x10] = (e >> (8 - sh)) & m;
            o[0x11] = (e >> (3 - sh)) & m;
            o[0x12] = (e << (sh + 2)) & m;
            e = cl[t3 & 0xFF];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x1C] = (e >> (8 - sh)) & m;
            o[0x1D] = (e >> (3 - sh)) & m;
            o[0x1E] = (e << (sh + 2)) & m;
            break;
        } else {
            u8 *tab = D_800A2630;
            u32 ty = t3 & 7, lo = arg3 & 0xF, hi = arg3 & 0xF0, lo1 = (lo + 1) & 0xF, hi1 = (hi + 0x10) & 0xF0;
            u8 *p;
            if (ty != 1) p = tab + arg3;
            else p = tab + (hi | lo1);
            c = *p;
            o[4] = c; o[5] = c; o[6] = c;
            c = tab[ty == 3 ? (hi1 | lo1) : (hi1 | lo)];
            o[0x1C] = c; o[0x1D] = c; o[0x1E] = c;
            c = tab[(ty == 3 || ty == 0) ? (hi | lo1) : (hi1 | lo1)];
            o[0x10] = c; o[0x11] = c; o[0x12] = c;
            return;
        }
    case 3:
    case 5:
    case 7:
        o[3] = 0xC;
        o[7] = ((t3 & 0x1000) >> 11) | 0x3C;
        *(u32 *)(o + 0xC) = a1[0] | (a0 & 0xFFFF0000);
        *(u32 *)(o + 0x18) = a1[1] | (a0 << 16);
        *(u16 *)(o + 0x30) = a1[2];
        *(u16 *)(o + 0x24) = a1[3];
        *(u32 *)(o + 8) = v0->w0;
        *(u32 *)(o + 0x14) = v1->w0;
        *(u32 *)(o + 0x2C) = v2->w0;
        *(u32 *)(o + 0x20) = v3->w0;
        if (k == 3) {
            t3 = (t3 << 16) | p5[1];
            c = (t3 >> 16) & 0xFC;
            o[4] = c; o[5] = c; o[6] = c;
            c = (t3 >> 10) & 0xFC;
            o[0x10] = c; o[0x11] = c; o[0x12] = c;
            c = (t3 >> 4) & 0xFC;
            o[0x28] = c; o[0x29] = c; o[0x2A] = c;
            c = (t3 & 0x3F) << 2;
            o[0x1C] = c; o[0x1D] = c; o[0x1E] = c;
            break;
        } else if (k == 5) {
            u32 sh, m;
            cl = (u16 *)tbl[0];
            e = cl[t3 & 0xFF];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[4] = (e >> (8 - sh)) & m;
            o[5] = (e >> (3 - sh)) & m;
            o[6] = (e << (sh + 2)) & m;
            t3 = p5[1];
            e = cl[t3 >> 8];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x10] = (e >> (8 - sh)) & m;
            o[0x11] = (e >> (3 - sh)) & m;
            o[0x12] = (e << (sh + 2)) & m;
            e = cl[t3 & 0xFF];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x28] = (e >> (8 - sh)) & m;
            o[0x29] = (e >> (3 - sh)) & m;
            o[0x2A] = (e << (sh + 2)) & m;
            t3 = p5[2];
            e = cl[t3 >> 8];
            sh = e >> 15;
            m = 0x1F << (sh + 2);
            o[0x1C] = (e >> (8 - sh)) & m;
            o[0x1D] = (e >> (3 - sh)) & m;
            o[0x1E] = (e << (sh + 2)) & m;
            break;
        } else {
            u32 lo = arg3 & 0xF, hi = arg3 & 0xF0, lo1 = (lo + 1) & 0xF, hi1 = (hi + 0x10) & 0xF0;
            c = D_800A2630[arg3];
            o[4] = c; o[5] = c; o[6] = c;
            c = D_800A2630[hi | lo1];
            o[0x10] = c; o[0x11] = c; o[0x12] = c;
            c = D_800A2630[hi1 | lo1];
            o[0x28] = c; o[0x29] = c; o[0x2A] = c;
            c = D_800A2630[hi1 | lo];
            o[0x1C] = c; o[0x1D] = c; o[0x1E] = c;
            break;
        }
    default:
        return;
    }
}
