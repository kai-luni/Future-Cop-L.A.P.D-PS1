// Categories: Utilities > Address Calculation
#include "game/types.h"

extern s32 D_8009F980;
extern s32 D_8009F984;
extern s32 D_8009F988;

/**
 *  @brief Stores three inputs and computes a combined offset.
 *
 *  The result combines a base of 0x57C with scaled first and third inputs.
 *  The original inputs are retained in globals for subsequent use.
 *  @param a0 First input, scaled by 2048 in the result.
 *  @param a1 Second input, added directly to the result.
 *  @param a2 Third input, scaled by 56 in the result.
 *  @return The computed sum of 0x57C, a0 times 2048, a1, and a2 times 56.
 *  @par Globals
 *  D_8009F980, D_8009F984, and D_8009F988 receive a0, a1, and a2 respectively.
 *  @par Control flow
 *  Performs the stores and arithmetic without branches or loops.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Updates the three input globals.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_800896D4(s32 a0, s32 a1, s32 a2)
{
    s32 t;

    D_8009F980 = a0;
    a0 <<= 11;
    a0 += 0x57C;
    D_8009F984 = a1;
    a1 += a0;
    t = a2 * 56;
    D_8009F988 = a2;
    return a1 + t;
}
