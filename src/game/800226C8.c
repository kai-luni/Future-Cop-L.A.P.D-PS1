// Categories: Game Engine > State
#include "../../include/game/types.h"

extern u8 D_800A0968;

/**
 *  @brief Returns the current value of a global byte.
 *
 *  This accessor reads D_800A0968 without modifying it; the byte's
 *  meaning is not established by the source.
 *  @return The byte value, widened to u32.
 *  @par Globals
 *  D_800A0968 is read.
 *  @par Control flow
 *  Returns the value directly, without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
u32 func_800226C8(void)
{
    return D_800A0968;
}
