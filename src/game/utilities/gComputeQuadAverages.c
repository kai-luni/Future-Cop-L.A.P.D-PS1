// Categories: Utilities > Numeric Processing
#include "game/types.h"
void gComputeQuadAverages(u32 a0, u32 a1, u32 a2, u32 a3, u32 *arg) __asm__("func_8004F00C");


/**
 *  @brief Computes and transforms four unsigned values into an output array.
 *
 *  The initial values are a0, two rounded pair averages, and a rounded
 *  four-input average; each is then transformed using unsigned arithmetic.
 *  @param a0 First input value.
 *  @param a1 Second input value.
 *  @param a2 Third input value.
 *  @param a3 Fourth input value.
 *  @param arg Output array of at least four unsigned words.
 *  @par Globals
 *  None referenced directly.
 *  @par Control flow
 *  Initializes four words, then loops over them applying the shift and mask expression.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the four output words; unsigned arithmetic wraps as defined by C.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void gComputeQuadAverages(u32 a0, u32 a1, u32 a2, u32 a3, u32 *arg)
{
    register u32 *start __asm__("$2") = arg;
    register u32 *p __asm__("$8");
    register u32 i __asm__("$9");
    register u32 v __asm__("$2");
    register u32 x __asm__("$3");
    u32 s;
    __asm__ __volatile__("" : "+r"(start));
    i = 0;
    p = start;
    __asm__ __volatile__("" : "+r"(p), "+r"(i));
    p[1] = (a0 + a1 + 1) >> 1;
    __asm__ __volatile__("" : : : "memory");
    v = (a0 + a2 + 1) >> 1;
    s = (a2 + a3 + a0 + a1 + 2) >> 2;
    p[0] = a0;
    p[2] = v;
    p[3] = s;
    do {
        v = *p;
        i = i + 1;
        v = ((v + 7) >> 4) - 15;
        __asm__("" : "+r"(v));
        x = v >> 31;
        __asm__("" : "+r"(x));
        x = x & v;
        __asm__("" : "+r"(x));
        x = x + 15;
        __asm__("" : "+r"(x));
        *p = x;
        p = p + 1;
    } while (i < 4);
}
