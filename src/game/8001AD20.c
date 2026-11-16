// Categories: Utilities > Fixed-point Math
#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

/**
 *  @brief Computes the fixed-point cross product of two three-component vectors.
 *
 *  Each component is a difference of signed products shifted right by 16 bits.
 *  The result is a cross b, with each product shifted before subtraction.
 *  @param a First input vector of three signed components.
 *  @param b Second input vector of three signed components.
 *  @param out Destination for the three result components.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Computes and stores the three components sequentially without branches.
 *  @par Calls
 *  None; multiplication is performed by inline assembly.
 *  @par Side effects
 *  Writes three values to out; overlapping inputs can affect later calculations.
 *  @par Callers
 *  Confirmed: func_80053A74.
 */
void func_8001AD20(s32 *a, s32 *b, s32 *out)
{
    register s32 p __asm__("$3");
    register s32 q __asm__("$2");
    register s32 x __asm__("$7");
    register s32 last __asm__("$5");

    p = b[2];
    q = a[1];
    FMUL(p, p, q);
    x = b[1];
    q = a[2];
    FMUL(q, x, q);
    p -= q;
    out[0] = p;

    p = b[0];
    q = a[2];
    FMUL(p, p, q);
    x = b[2];
    q = a[0];
    FMUL(q, x, q);
    p -= q;
    out[1] = p;

    p = b[1];
    q = a[0];
    FMUL(p, p, q);
    last = b[0];
    __asm__ volatile("" : "+r"(last));
    q = a[1];
    FMUL(q, last, q);
    p -= q;
    out[2] = p;
}
