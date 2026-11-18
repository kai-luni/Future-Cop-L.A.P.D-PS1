// Categories: Utilities > Mathematics > Vector
#include "game/types.h"

extern int gGuidedProjectileUpdate();

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

#define FMULN(d, a, b) __asm__("nop\n\tnop\n\tmult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

extern s32 func_80095900(s32 value);

/**
 * @brief Vector length with in-place normalisation, range-folded for the
 *        fixed-point square root.
 *
 * @param vec Three s32 components; normalised in place when the length is
 *        representable.
 * @return The length in the original scale (range-compensated).
 *
 * @par Confirmed
 * m = max(|x|, |y|, |z|). m < 0x200 shifts the vector << 10, m > 0x800000
 * >> 10 before the sum of squares (FMUL 16.16-style products); len =
 * func_80095900(sum). With t = len << 2 >= 2 the scale 0x80000000 / (t >> 1)
 * normalises all three components. The return compensates the fold:
 * t >> 10 for the small case, len << 12 for the large case, else t.
 *
 * @par Likely role
 * The length+normalise workhorse of the motion and camera code (the mover
 * physics, the path segment constructor func_8006EEF8, the beam builder
 * func_80070E48): the fold keeps the squared sum inside the reciprocal
 * table's domain.
 *
 * @par Unknown
 * The exact domain of func_80095900 (reciprocal square root table).
 *
 * @par Callers
 * gGuidedProjectileUpdate, func_8004B180, func_80053A74, func_800549F0, func_800569EC,
 * func_8006EEF8, func_80070E48.
 */
s32 func_8001A314(s32 *vec)
{
    s32 x, y, z, ax, ay, az, d, m, sum, len, t, scale, r;
    s32 p0, p1, p2;

    x = vec[0];
    y = vec[1];
    z = vec[2];
    ax = x;
    ay = y;
    az = z;
    if (ax < 0) ax = -ax;
    if (ay < 0) ay = -ay;
    if (az < 0) az = -az;

    d = ay - ax;
    m = ay - ((d >> 31) & d);
    d = az - m;
    m = az - ((d >> 31) & d);

    if (m < 0x200) {
        x <<= 10; y <<= 10; z <<= 10;
    } else if (m > 0x800000) {
        x >>= 10; y >>= 10; z >>= 10;
    }

    FMUL(p0, x, x);
    FMUL(p1, y, y);
    FMUL(p2, z, z);
    len = func_80095900(p0 + p1 + p2);
    t = len << 2;
    if (t >= 2) {
        scale = (s32)(0x80000000U / (u32)(t >> 1));
        FMULN(r, x, scale);
        vec[0] = r;
        FMUL(r, y, scale);
        vec[1] = r;
        FMUL(r, z, scale);
        vec[2] = r;
    }
    if (m < 0x200) {
        t >>= 10;
    } else if (m > 0x800000) {
        t = len << 12;
    }
    return t;
}
