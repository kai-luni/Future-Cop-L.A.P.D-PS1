// Categories: Game Engine > State
#include "../../include/game/types.h"

extern s32 D_800A0974;

/**
 *  @brief Sets a global state value to one.
 *
 *  This function provides a direct write to D_800A0974; the meaning of
 *  that state value is not established by the source.
 *  @par Globals
 *  D_800A0974 is assigned the value 1.
 *  @par Control flow
 *  Performs one unconditional assignment.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Changes D_800A0974.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80022700(void)
{
    D_800A0974 = 1;
}
