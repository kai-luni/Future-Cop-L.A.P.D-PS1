// Categories: Game Engine > State
#include "../../include/game/types.h"

extern s32 D_800A09DC;

/**
 *  @brief Returns a global signed state value.
 *
 *  This accessor reads D_800A09DC without modifying it; the value's
 *  meaning is not established by the source.
 *  @return The current value of D_800A09DC.
 *  @par Globals
 *  D_800A09DC is read.
 *  @par Control flow
 *  Returns the value directly, without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_8002443C.
 */
s32 func_80022B74(void)
{
    return D_800A09DC;
}
