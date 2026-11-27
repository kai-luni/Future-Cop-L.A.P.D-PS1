// Categories: Utilities > Mathematics > Fixed Point
#include "game/types.h"

extern int gSolveLobVector();

void gScaleVector2Q16(s32 scale, s32 *p) __asm__("func_8001ABE0");

/**
 * @brief Scales two fixed-point components in place by a signed factor.
 *
 * @param scale Signed 16.16 scale factor.
 * @param p Two writable signed 16.16 components.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads each component, multiplies it by $a0 with signed MULT, extracts
 * product bits 16 through 47, and stores the result. The second store is in
 * the return delay slot.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gNormalizeVector2Q16,
 * func_80051EB8, gTickHoverPhysics, func_80054590, gSolveLobVector.
 *
 * @par Role and path
 * Shared motion and direction code uses this helper to scale and normalize
 * two-component vectors.
 *
 * @par Unknown
 * The caller defines the units carried by the vector.
 */
void gScaleVector2Q16(s32 scale, s32 *p)
{
    register u32 lo0 __asm__("$8");
    register u32 hi0 __asm__("$9");
    register s32 x __asm__("$2");
    register s32 r __asm__("$3");

    x = p[0];
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x), "r"(scale));
    hi0 <<= 16;
    lo0 >>= 16;
    r = hi0 | lo0;
    x = p[1];
    p[0] = r;
    __asm__ volatile("mult %2,%3\n\tmflo %0\n\tmfhi %1" : "=r"(lo0), "=r"(hi0) : "r"(x), "r"(scale));
    hi0 <<= 16;
    lo0 >>= 16;
    r = hi0 | lo0;
    p[1] = r;
}
