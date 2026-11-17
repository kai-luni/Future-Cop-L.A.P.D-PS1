// Categories: Graphics > Renderer > Map Tile Icons | Graphics > GPU Primitives | Graphics > 2D Drawing
#include "game/types.h"

extern u8 gVertexIndexTable[][4] __asm__("D_8009AFE8");
extern u8 D_8009B1A4[];
extern u8 *gPrimPoolNext __asm__("D_8009F9F0");
extern u8 *gPrimPoolEnd __asm__("D_8009FA28");
extern u32 D_800A8E80[];
extern u8 D_800A2630[];
extern u16 D_8009F83C;

#define PK ((u8 *)t0)
#define SRC ((u16 *)a0)
#define TEX ((u16 *)t4)

#define PAL(off, c)                                    \
    a1 = (c);                                          \
    a0 = a1 >> 15;                                     \
    PK[(off)] = (a1 >> (8 - a0)) & (t1 = 0x1F << (a0 + 2)); \
    PK[(off) + 1] = (a1 >> (3 - a0)) & t1;             \
    PK[(off) + 2] = (a1 << (a0 + 2)) & t1;

#define GRAY(off, c)                                   \
    gr = (c);                                          \
    PK[(off)] = gr;                                    \
    PK[(off) + 1] = gr;                                \
    PK[(off) + 2] = gr;

/**
 * @brief Emits the 10x10 pixel icon quads of one map cell: walks the cell's
 *        primitive word list and writes one textured, flat or gouraud packet
 *        per word into the shared primitive pool.
 *
 * @param ctx  Base of the cell record. Confirmed sub-structure: +0x004 and
 *             +0x008 are two table base pointers (the first supplies the
 *             per-primitive descriptor half-words, the second the UV/colour
 *             half-words), +0x3CC is the array of 16-bit cell descriptors and
 *             +0x5D0 is the primitive word stream. The first descriptor's low
 *             nibble is the primitive count of the cell and its upper bits are
 *             the byte offset of the first word inside the +0x5D0 stream.
 * @param arg1 Sub-index of the primitive block inside the cell: it selects the
 *             16-bit descriptor at ctx + 0x3CC and the 3-byte shading triple
 *             at ctx + 0x0C + ((arg1 >> 4) + arg1) * 3. Its two nibbles double
 *             as texture-atlas coordinates in the ramp variants (low nibble =
 *             column, high nibble = row of what the k == 6 branch treats as a
 *             16x16 tile atlas).
 * @param x    Screen X in 16.16 fixed point; the high half is used.
 * @param y    Screen Y in 16.16 fixed point; the high half is used.
 *
 * @return Nothing. The function returns early - without touching the remaining
 *         words - as soon as a primitive index >= 0x47 appears, which is how a
 *         cell list terminates.
 *
 * @par Control flow (confirmed)
 * 1. count = descriptor & 0xF, wp = ctx + (descriptor >> 4) + 0x5D0,
 *    base = ctx + (((arg1 >> 4) + arg1) * 3 + 0xC). Loop while count > 0.
 * 2. wd = next word; t = (wd >> 15) & 0x7F. A value >= 0x47 returns.
 * 3. The four bytes of D_8009AFE8[t] index the 3-byte shading table; their sum
 *    a1 is the quad's brightness.
 * 4. switch (D_8009B1A4[t] >> 3) builds the vertex set v[0..7] as two
 *    half-words per corner of a 10x10 pixel square: modes 0..4 place the
 *    trailing corner (x+10, y+10) at different positions, which flips the
 *    triangle/quad split and the winding of the icon.
 * 5. A packet is taken from the D_8009F9F0 pool (0x34 bytes, the last slot is
 *    reused when the cursor has reached D_8009FA28). The header word receives
 *    (0x400 - a1) in its low 24 bits - a darkness key - and byte 3 receives the
 *    length (7, 9 or 0x0C) in the switch below.
 * 6. t4 = table1[t] (the descriptor half-word) and a0 = table2[t] (the UV
 *    half-word). When the descriptor has bit 0x800 set, four half-words are
 *    read from the UV table, re-based by D_8009F83C and copied to the
 *    scratchpad palette at 0x1F800070, which then replaces a0 - a per-cell
 *    palette animation.
 * 7. The page base a1 = D_800A8E80[(descriptor >> 8) & 7] and the kind
 *    k = descriptor >> 13 select the packet form:
 *    - k == 0: 3 corners, flat colour from (descriptor & 0xFC), length 7,
 *      code 0x24;
 *    - k == 1: 4 corners, length 9, code 0x2C;
 *    - k == 2 / 4 / 6: 4 corners, length 9, code 0x34, with per-corner colours
 *      taken from the gouraud words (k == 2), from the palette through the low
 *      byte / next half-word (k == 4), or from the D_800A2630 ramp indexed by
 *      the arg1 nibbles and the low 3 bits of the descriptor (k == 6);
 *    - k == 3 / 5 / 7: 5 corner writes, length 0x0C, code 0x3C, same three
 *      colour variants with one more corner.
 *    In every case the low bit of the code byte is the semi-transparency flag
 *    taken from descriptor bit 0x1000.
 *
 * @par Globals read/written
 * D_8009AFE8 (4 vertex-shading bytes per primitive index),
 * D_8009B1A4 (one flag byte per index; >> 3 is the corner layout),
 * D_8009F9F0 / D_8009FA28 (packet cursor and limit),
 * D_800A8E80 (texture-page base per (descriptor >> 8) & 7),
 * D_800A2630 (16x16 shading ramp used by the k == 6 variant),
 * D_8009F83C (palette phase added to the four scratchpad entries),
 * 0x1F800070 (scratchpad palette staging area).
 *
 * @par Calls
 * None - the edge table lists no direct callee; the function is pure data
 * marshalling into the packet buffer.
 *
 * @par Side effects
 * Advances D_8009F9F0 by 0x34 per emitted packet, writes into scratchpad memory
 * at 0x1F800070, and clobbers the caller's x and y arguments (both are shifted
 * in place).
 *
 * @par Callers
 * func_8006680C (direct) - the overhead map pass: for every visible grid cell
 * it resolves the tile record, sets D_8009F83C from the tile's colour bytes and
 * then calls this function once per cell sub-index with
 * (tile, (i << 4) + j, ((xx * 5) << 17) - zfrac, ((yy * 5) << 17) - x), i.e. a
 * 16-step loop in both directions with a 5:1 pixel scale.
 *
 * @par Likely
 * The function draws the small tile icons of the tactical/overhead map: the
 * 10x10 extent, the 16.16 incoming coordinates, the per-cell palette rotation
 * and the caller's pixel-window clamp all fit a scaled top-down
 * map. The (0x400 - brightness) value in the header is a sort key so darker
 * icons are ordered first.
 *
 * @par Unknown
 * - What the two table bases at ctx + 4 and ctx + 8 point at in the file
 *   format; only their use as half-word tables with 2-byte granularity is
 *   confirmed.
 * - The exact meaning of D_8009B1A4 bits other than >> 3, and of the k == 6
 *   variant's nibble arithmetic.
 * - Whether the header's low 24 bits are later replaced by an ordering-table
 *   link (the same word is the addr:24 field of the packet header used by
 *   func_8001C148).
 */
void func_80065CA8(u8 *ctx, s32 arg1, s32 x, s32 y)
{
    s32 idx;
    u32 *wp;
    s8 *base;
    s32 count;
    s16 v[8];
    s32 t0;
    u32 t1;
    u32 t2;
    s32 t3;
    s32 t4;
    s32 a0;
    u32 a1;
    s32 lo, hi, lo1, hi1;
    u32 gr;
    u32 wd;

    t2 = arg1 >> 4;
    idx = arg1;
    a1 = ((u16 *)(ctx + 0x3CC))[idx];
    t2 = t2 + idx;
    x = x >> 16;
    y = y >> 16;
    wp = (u32 *)(ctx + (((s32)a1 >> 4) + 0x5D0));
    count = a1 & 0xF;
    base = (s8 *)(ctx + (t2 * 3 + 0xC));

    for (; count > 0; count--) {
        wd = *wp++;
        t0 = (wd >> 15) & 0x7F;
        if (t0 >= 0x47) {
            return;
        }
        { u8 *e = gVertexIndexTable[t0];
        a1 = base[e[0]] + base[e[1]] + base[e[2]] + base[e[3]]; }
        switch (D_8009B1A4[t0] >> 3) {
        case 0:
            v[4] = x; v[0] = x; v[3] = y; v[1] = y; v[2] = x + 10;
            v[5] = y + 10;
            break;
        case 1:
            v[2] = x + 10; v[0] = x + 10; v[1] = y; v[4] = x;
            v[5] = y + 10; v[3] = y + 10;
            break;
        case 2:
            v[4] = x; v[0] = x; v[1] = y; v[2] = x + 10;
            v[5] = y + 10; v[3] = y + 10;
            break;
        case 3:
            v[0] = x; v[3] = y; v[1] = y; v[4] = x + 10; v[2] = x + 10;
            v[5] = y + 10;
            break;
        case 4:
            v[6] = x; v[0] = x; v[3] = y; v[1] = y; v[4] = x + 10; v[2] = x + 10;
            v[7] = y + 10; v[5] = y + 10;
            break;
        }
        if (gPrimPoolNext < gPrimPoolEnd) {
            t0 = (s32)gPrimPoolNext;
            gPrimPoolNext = PK + 0x34;
        } else {
            t0 = (s32)gPrimPoolNext;
        }
        *(u32 *)PK = (*(u32 *)PK & 0xFF000000) | ((0x400 - a1) & 0xFFFFFF);
        t4 = *(s32 *)(ctx + 4) + ((wd >> 21) & 0x7FE);
        a0 = *(s32 *)(ctx + 8) + (wd & 0x7FE);
        t2 = *TEX;
        if (t2 & 0x800) {
            u16 g = D_8009F83C;
            u16 *d = (u16 *)0x1F800070;
            d[0] = *(u16 *)(a0 + 0) + g;
            d[1] = *(u16 *)(a0 + 2) + g;
            d[2] = *(u16 *)(a0 + 4) + g;
            d[3] = *(u16 *)(a0 + 6) + g;
            a0 = 0x1F800070;
        }
        a1 = D_800A8E80[(t2 >> 8) & 7];
        t1 = t2 >> 13;
        switch (t1) {
        case 0:
            gr = t2 & 0xFC;
            PK[3] = 7;
            PK[7] = ((t2 & 0x1000) >> 11) | 0x24;
            PK[4] = gr;
            PK[5] = gr;
            PK[6] = gr;
            *(u32 *)(PK + 0xC) = SRC[0] | (a1 & 0xFFFF0000);
            *(u32 *)(PK + 0x14) = SRC[1] | (a1 << 16);
            *(u16 *)(PK + 0x1C) = SRC[2];
            *(u32 *)(PK + 0x8) = *(u32 *)&v[0];
            *(u32 *)(PK + 0x10) = *(u32 *)&v[2];
            *(u32 *)(PK + 0x18) = *(u32 *)&v[4];
            break;
        case 1:
            gr = t2 & 0xFC;
            PK[3] = 9;
            PK[7] = ((t2 & 0x1000) >> 11) | 0x2C;
            PK[4] = gr;
            PK[5] = gr;
            PK[6] = gr;
            *(u32 *)(PK + 0xC) = SRC[0] | (a1 & 0xFFFF0000);
            *(u32 *)(PK + 0x14) = SRC[1] | (a1 << 16);
            *(u16 *)(PK + 0x24) = SRC[2];
            *(u16 *)(PK + 0x1C) = SRC[3];
            *(u32 *)(PK + 0x8) = *(u32 *)&v[0];
            *(u32 *)(PK + 0x10) = *(u32 *)&v[2];
            *(u32 *)(PK + 0x20) = *(u32 *)&v[4];
            *(u32 *)(PK + 0x18) = *(u32 *)&v[6];
            break;
        case 2:
        case 4:
        case 6:
            PK[3] = 9;
            PK[7] = ((t2 & 0x1000) >> 11) | 0x34;
            *(u32 *)(PK + 0xC) = SRC[0] | (a1 & 0xFFFF0000);
            *(u32 *)(PK + 0x18) = SRC[1] | (a1 << 16);
            *(u16 *)(PK + 0x24) = SRC[2];
            *(u32 *)(PK + 0x8) = *(u32 *)&v[0];
            *(u32 *)(PK + 0x14) = *(u32 *)&v[2];
            *(u32 *)(PK + 0x20) = *(u32 *)&v[4];
            if (t1 == 2) {
                t2 = (t2 << 16) | TEX[1];
                GRAY(4, (t2 >> 16) & 0xFC);
                GRAY(0x10, (t2 >> 10) & 0xFC);
                GRAY(0x1C, (t2 >> 4) & 0xFC);
            } else if (t1 == 4) {
                t3 = *(s32 *)ctx;
                PAL(4, ((u16 *)t3)[t2 & 0xFF]);
                t2 = TEX[1];
                PAL(0x10, ((u16 *)t3)[t2 >> 8]);
                PAL(0x1C, ((u16 *)t3)[t2 & 0xFF]);
            } else {
                a1 = t2 & 7;
                t3 = idx & 0xF;
                t2 = idx & 0xF0;
                a0 = (t3 + 1) & 0xF;
                t1 = (t2 + 0x10) & 0xF0;
                t4 = (s32)D_800A2630;
                GRAY(4, *(a1 != 1 ? (u8 *)t4 + idx : (u8 *)t4 + (t2 | a0)));
                GRAY(0x1C, ((u8 *)t4)[a1 != 3 ? (t1 | t3) : (t1 | a0)]);
                GRAY(0x10, ((u8 *)t4)[(a1 == 3 || a1 == 0) ? (t2 | a0) : (t1 | a0)]);
            }
            break;
        case 3:
        case 5:
        case 7:
            PK[3] = 0xC;
            PK[7] = ((t2 & 0x1000) >> 11) | 0x3C;
            *(u32 *)(PK + 0xC) = SRC[0] | (a1 & 0xFFFF0000);
            *(u32 *)(PK + 0x18) = SRC[1] | (a1 << 16);
            *(u16 *)(PK + 0x30) = SRC[2];
            *(u16 *)(PK + 0x24) = SRC[3];
            *(u32 *)(PK + 0x8) = *(u32 *)&v[0];
            *(u32 *)(PK + 0x14) = *(u32 *)&v[2];
            *(u32 *)(PK + 0x2C) = *(u32 *)&v[4];
            *(u32 *)(PK + 0x20) = *(u32 *)&v[6];
            if (t1 == 3) {
                t2 = (t2 << 16) | TEX[1];
                GRAY(4, (t2 >> 16) & 0xFC);
                GRAY(0x10, (t2 >> 10) & 0xFC);
                GRAY(0x28, (t2 >> 4) & 0xFC);
                GRAY(0x1C, (t2 & 0x3F) << 2);
            } else if (t1 == 5) {
                t3 = *(s32 *)ctx;
                PAL(4, ((u16 *)t3)[t2 & 0xFF]);
                t2 = TEX[1];
                PAL(0x10, ((u16 *)t3)[t2 >> 8]);
                PAL(0x28, ((u16 *)t3)[t2 & 0xFF]);
                t2 = TEX[2];
                PAL(0x1C, ((u16 *)t3)[t2 >> 8]);
            } else {
                lo = idx & 0xF;
                hi = idx & 0xF0;
                lo1 = (lo + 1) & 0xF;
                hi1 = (hi + 0x10) & 0xF0;
                GRAY(4, D_800A2630[idx]);
                GRAY(0x10, D_800A2630[hi | lo1]);
                GRAY(0x28, D_800A2630[hi1 | lo1]);
                GRAY(0x1C, D_800A2630[hi1 | lo]);
            }
            break;
        }
    }
}
