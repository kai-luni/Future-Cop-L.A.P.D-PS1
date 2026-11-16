// Categories: Game Engine > State
#include "game/types.h"

extern s32 D_8009F7B0;

/**
 *  @brief Clears a global signed integer.
 *
 *  This function sets a shared integer to zero through a callable entry point.
 *  The meaning of the integer is not established by the source.
 *  @par Globals
 *  D_8009F7B0 is written with zero.
 *  @par Control flow
 *  Performs one assignment and returns without branching or looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites D_8009F7B0.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8004EFD4(void)
{
    D_8009F7B0 = 0;
}
