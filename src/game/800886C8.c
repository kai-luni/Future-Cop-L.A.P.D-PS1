// Categories: Utilities > Math
#include "game/types.h"

s32 gMinS32(s32 a, s32 b) __asm__("func_800886C8");

/**
 *  @brief Returns the smaller of two signed integers.
 *
 *  Compares the arguments using signed ordering.
 *  Equal arguments produce the same value through the second branch.
 *  @param a First value to compare.
 *  @param b Second value to compare.
 *  @return The smaller value, or b when the values are equal.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Returns a if a is less than b; otherwise returns b.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
s32 gMinS32(s32 a, s32 b) {
    if (a < b) {
        return a;
    }
    return b;
}
