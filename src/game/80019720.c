// Categories: Utilities > Random Numbers
#include "game/types.h"

extern s32 gRandSeed __asm__("D_8009F214");

/* generates a random number, 31 bit */

/**
 *  @brief Advances the global random-number state and returns its new value.
 *
 *  Multiplies the state by 16807 using separate high and low halves and
 *  folds the result into a 31-bit random-number sequence.
 *  @return The updated random-number state.
 *  @par Globals
 *  D_8009F214 supplies the previous state and receives the updated state.
 *  @par Control flow
 *  If the computed signed state is negative, subtracts 0x7FFFFFFF.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites D_8009F214.
 *  @par Callers
 *  Confirmed: func_800197A8.
 */
s32 func_80019720(void)
{
    u32 high;
    u32 low;

    high = ((u32)gRandSeed >> 16) * 16807;
    low = (gRandSeed & 0xFFFF) * 16807;
    gRandSeed = low + ((high & 0x7FFF) << 16) + (high >> 15);
    if (gRandSeed < 0) {
        gRandSeed -= 0x7FFFFFFF;
    }
    return gRandSeed;
}
