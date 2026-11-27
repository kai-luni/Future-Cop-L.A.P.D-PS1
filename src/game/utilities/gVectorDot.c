// Categories: Utilities > Mathematics > Vector Dot Product | Utilities > Mathematics > Fixed Point
#include "game/types.h"

extern int gGuidedProjectileUpdate();
extern int gBeamPartUpdate();
s32 gVectorDot(s32 *a, s32 *b) __asm__("func_8001AB00");


/**
 * @brief Dot product of two three-component vectors in 16.16 fixed point.
 *
 * @param a First vector, three s32 components.
 * @param b Second vector, three s32 components.
 * @return Sum of the three middle-word products (a[i] * b[i] >> 16).
 *
 * @par Confirmed
 * Each component pair is multiplied with mult; the middle 32 bits are
 * assembled as (hi << 16) | (lo >> 16) and the three products are summed.
 * No saturation or rounding.
 *
 * @par Likely role
 * The projection/sign helper of the geometry code: the aiming routine
 * func_8005F018 uses it to decide whether the spread offset points away
 * from the target, and the attachment renderer func_8005CED4 uses it for
 * its orientation test.
 *
 * @par Callers
 * gGuidedProjectileUpdate, func_80037B28, func_80052550, func_8005B934, func_8005C344,
 * func_8005CED4, func_8005D8F4, func_8005F018, func_8005F6E8, gBeamPartUpdate.
 *
 * @par Environment path
 * Returns the 16.16 fixed-point dot product of two three-component vectors
 * (middle 32 bits of each 64-bit product, summed, no rounding). Callers are
 * gGuidedProjectileUpdate, gTakeArmedLobePoint, gBeamPartUpdate,
 * func_80037B28, func_80052550, func_8005B934, func_8005C344, func_8005D8F4,
 * func_8005F018 and func_8005F6E8. No camera unit calls it; it is shared
 * weapon/geometry math.
 */
s32 gVectorDot(s32 *a, s32 *b)
{
    register u32 lo0 __asm__("$8");
    register u32 hi0 __asm__("$9");
    register s32 x __asm__("$3");
    register s32 y __asm__("$2");
    register s32 p0 __asm__("$3");
    register s32 p1 __asm__("$2");
    register s32 x1 __asm__("$6");
    register s32 x2 __asm__("$4");

    x = a[0];
    y = b[0];
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x), "r"(y));
    hi0 <<= 16;
    lo0 >>= 16;
    p0 = hi0 | lo0;
    x1 = a[1];
    y = b[1];
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x1), "r"(y));
    hi0 <<= 16;
    lo0 >>= 16;
    p1 = hi0 | lo0;
    p0 += p1;
    x2 = a[2];
    y = b[2];
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x2), "r"(y));
    hi0 <<= 16;
    lo0 >>= 16;
    p1 = hi0 | lo0;
    return p0 + p1;
}
