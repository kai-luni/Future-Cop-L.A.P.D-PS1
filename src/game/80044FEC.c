// Categories: Game Engine > State
#include "game/types.h"

extern u8 D_8009F340;

/**
 *  @brief Clears a global byte.
 *
 *  This function sets a shared byte to zero through a callable entry point.
 *  The meaning of the byte is not established by the source.
 *  @par Globals
 *  D_8009F340 is written with zero.
 *  @par Control flow
 *  Performs one assignment and returns without branching or looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites D_8009F340.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80044FEC(void)
{
    D_8009F340 = 0;
}
