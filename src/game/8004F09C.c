// Categories: Utilities > Coordinate Calculations
#include "game/types.h"
struct Pair { s32 x; s32 y; };
extern struct Pair *gFreedPointer __asm__("D_8009F7B0");

/**
 *  @brief Computes clamped coordinate intermediates and returns the next pair.
 *
 *  When the global pair pointer is non-null, two input components are shifted
 *  right by 14, offset by the pair's fields, and clamped to zero.
 *  The computed values are not stored or returned by the C source.
 *  @param arg Pointer to an array whose elements 0 and 2 supply the components.
 *  @return The pair after D_8009F7B0, or null if that pointer is null.
 *  @par Globals
 *  D_8009F7B0 supplies the pair pointer and coordinate offsets.
 *  @par Control flow
 *  A null check skips the input reads; otherwise each intermediate is clamped.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  No C-visible state is modified.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
struct Pair *func_8004F09C(s32 *arg)
{
    register struct Pair *p __asm__("$5") = gFreedPointer;
    register struct Pair *next __asm__("$7") = p + 1;
    register s32 x __asm__("$4") = (s32)arg;
    register s32 y __asm__("$6");
    register s32 t __asm__("$2");
    register s32 u __asm__("$3");
    struct Pair *result;
    if (p == 0) {
        __asm__ __volatile__("" : "+r"(p));
        result = 0;
    } else {
        __asm__ __volatile__("" : "+r"(x));
        t = ((s32 *)x)[2];
        u = ((s32 *)x)[0];
        __asm__ __volatile__("" : "+r"(t), "+r"(u));
        x = t >> 14;
        y = u >> 14;
        __asm__ __volatile__("" : : "r"(x), "r"(y) : "memory");
        x += p->y;
        y += p->x;
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        result = next;
    }
    __asm__ __volatile__("" : : "r"(x), "r"(y));
    return result;
}
