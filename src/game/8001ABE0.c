// Categories: Utilities > Mathematics > Fixed Point
#include "game/types.h"

extern int gSolveLobVector();

/**
 * @brief Scales a 2-element 16.16 fixed-point vector in place by a signed 16.16 factor.
 *
 * Multiplies p[0] and p[1] by scale using 32x32->64 signed multiplication and extracts the middle 32 bits ((hi << 16) | (lo >> 16)), writing results back to p[0] and p[1].
 *
 * @param scale Signed 16.16 fixed-point scale factor.
 * @param p Pointer to a two-element s32 vector modified in place.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line inline-assembly mult/mflo/mfhi scaling for p[0] and p[1]; no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Overwrites p[0] and p[1] in place.
 *
 * @par Callers
 * Confirmed: gNormalizeVector2Q16 (0x8001A7F8), func_80051EB8 (0x80051F70), func_80053A74 (0x80054020, 0x80054214), func_80054590 (0x80054800), and gSolveLobVector (0x8005E980).
 */
void func_8001ABE0(s32 scale, s32 *p)
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
