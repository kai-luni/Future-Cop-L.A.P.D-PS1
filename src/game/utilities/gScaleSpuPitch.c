// Categories: Utilities > Fixed-point Arithmetic
#include "game/types.h"
u32 gScaleSpuPitch(s32 a0, u32 a1) __asm__("func_80099968");


/**
 *  @brief Computes a fixed-point scaled value using iterative growth and interpolation.
 *
 *  The upper bits of the second argument select repeated multiplication by
 *  4155 in 12-bit fixed point; its low five bits select an interpolation fraction.
 *  @param a0 Initial signed value to scale.
 *  @param a1 Step count and five-bit interpolation fraction.
 *  @return The interpolated result shifted right by 12 bits.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Iterates a1 shifted right by five times, then interpolates between adjacent values.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
u32 gScaleSpuPitch(s32 a0, u32 a1)
{
    s32 prev;
    s32 cur;
    u32 x;
    s32 steps;
    s32 frac;
    s32 i;

    prev = a0 << 12;
    steps = a1 >> 5;
    frac = a1 & 0x1F;
    x = 0x103B;
    cur = a0 * x;

    for (i = 0; i < steps; i++) {
        prev = a0 * x;
        x = 4155 * x;
        x >>= 12;
        cur = a0 * x;
    }

    return (prev + (((u32)(cur - prev) >> 5) * frac)) >> 12;
}
