// Categories: Utilities > Fixed-point Math
#include "game/types.h"
#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

void gTransformPointQ16(volatile s32 *m, volatile s32 *v, s32 *out) __asm__("func_8001A970");

/**
 * @brief Applies a fixed-point affine transform to one three-component point.
 *
 * @param m Twelve matrix words, including the three translations.
 * @param v Three signed input components.
 * @param out Destination for three transformed components.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Multiplies the point by the matrix columns at word indices (0,3,6), (1,4,7),
 * and (2,5,8). Shifts each product by 16 before adding. Adds translation words
 * at +0x24, +0x28, and +0x2C to the three outputs.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gTestPartPairHit, func_8006320C.
 *
 * @par Role and path
 * World-space callers use this affine point transform in their geometry paths.
 *
 * @par Unknown
 * The routine does not identify the input and output coordinate frames.
 */
void gTransformPointQ16(volatile s32 *m, volatile s32 *v, s32 *out)
{
    register s32 x __asm__("$12");
    register s32 y __asm__("$11");
    register s32 z __asm__("$10");
    register s32 a __asm__("$3");
    register s32 b __asm__("$7");
    register s32 b2 __asm__("$5");
    register s32 c __asm__("$2");
    x = v[0];
    c = m[0];
    FMUL(a, x, c);
    y = v[1];
    c = m[3];
    FMUL(b, y, c);
    z = v[2];
    c = m[6];
    FMUL(c, z, c);
    out[0] = a + b + c;
    c = m[1];
    FMUL(a, x, c);
    c = m[4];
    FMUL(b2, y, c);
    c = m[7];
    FMUL(c, z, c);
    out[1] = a + b2 + c;
    c = m[2];
    FMUL(a, x, c);
    c = m[5];
    FMUL(b2, y, c);
    c = m[8];
    FMUL(c, z, c);
    out[2] = a + b2 + c;
    out[0] += m[9];
    out[1] += m[10];
    out[2] += m[11];
}
