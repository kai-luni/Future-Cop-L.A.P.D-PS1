// Categories: Graphics > Mesh Generation | Graphics > Effects
#include "game/types.h"

extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);

/**
 * @brief Generates the 3D s16 vertex grid for a spherical octant dome-patch mesh.
 *
 * Writes apex vertex 0 at (0, total, 0) with starting radius 0x800 (when count != 0) or 0x1000,
 * then sweeps latitude row = step..0x400 and longitude col = 0..0x200 via Q12 sine/cosine
 * pairs from func_8001846C, storing (x, y, z) in dst[n*4 + 0..2] and growing total by inc when count != 0.
 *
 * @param dst Output s16 vertex buffer (8-byte / 4-s16 stride per vertex).
 * @param step Angular step (divisor of 0x200) for both latitude and longitude loops.
 * @param count Expanding-radius selector (non-zero starts radius at 0x800 and grows by 0x800/(0x400/step) per row; 0 uses fixed 0x1000).
 *
 * @par Globals
 * None.
 * @par Control flow
 * Apex setup branching on count != 0, outer latitude for loop (row <= 0x400), and count-branched inner longitude loops (col <= 0x200).
 * @par Calls
 * func_8001846C - computes sine/cosine for row and col angles (shifted right by 4 into Q12).
 * @par Side effects
 * Populates vertex coordinates in dst (up to 3 s16 words per 4-s16 entry).
 * @par Callers
 * Confirmed: func_8006C84C (dome-patch mesh builder, passing flags & 0x10 as count).
 */
void func_8006C4C4(s16 *dst, s16 step, s16 count)
{
    s32 sine;
    s32 cosine;
    s32 s;
    s32 c;
    s16 total;
    u16 inc;
    s16 row;
    s16 col;
    s32 n;

    n = 0;
    if (count != 0) {
        inc = 0x800 / (0x400 / step);
        total = 0x800;
        dst[0] = 0;
        dst[1] = total;
        dst[2] = 0;
    } else {
        dst[0] = 0;
        dst[1] = 0x1000;
        dst[2] = 0;
    }

    n++;
    for (row = step; row < 0x401; row += step) {
        func_8001846C(row, &sine, &cosine);
        sine >>= 4;
        cosine >>= 4;
        if (count != 0) {
            for (col = 0; col < 0x201; col += step) {
                func_8001846C(col, &s, &c);
                s >>= 4;
                c >>= 4;
                dst[n * 4 + 0] = (s16)((sine * s) >> 12);
                dst[n * 4 + 1] = (s16)((total * cosine) >> 12);
                dst[n * 4 + 2] = (s16)((sine * c) >> 12);
                n++;
            }
            total += inc;
        } else {
            for (col = 0; col < 0x201; col += step) {
                func_8001846C(col, &s, &c);
                s >>= 4;
                c >>= 4;
                dst[n * 4 + 1] = (s16)((0x1000 * cosine) >> 12);
                dst[n * 4 + 0] = (s16)((sine * s) >> 12);
                dst[n * 4 + 2] = (s16)((sine * c) >> 12);
                n++;
            }
        }
    }
}
