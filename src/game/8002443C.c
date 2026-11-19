// Categories: Game Engine > State
#include "game/types.h"

extern int gGetAudioStreamBlockCount() __asm__("func_80022B74");

extern s32 D_8009F64C;

/**
 *  @brief Computes a scaled value from a global and a function result.
 *
 *  Subtracts the result of gGetAudioStreamBlockCount from three and multiplies it
 *  by the current value of D_8009F64C.
 *  @return The computed signed product.
 *  @par Globals
 *  D_8009F64C is read as the multiplier.
 *  @par Control flow
 *  Calls the helper and evaluates the arithmetic without branching.
 *  @par Calls
 *  gGetAudioStreamBlockCount supplies the value subtracted from three.
 *  @par Side effects
 *  No direct writes; the helper's effects are not specified here.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8002443C(void)
{
    return D_8009F64C * (3 - gGetAudioStreamBlockCount());
}
