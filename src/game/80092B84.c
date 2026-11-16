// Categories: Game Engine > State Access
#include "game/types.h"

extern u8 D_8009DDF2;

/**
 *  @brief Returns a stored byte of global state.
 *
 *  Reads the byte without modifying it or interpreting its meaning.
 *  @return Current value of D_8009DDF2.
 *  @par Globals
 *  D_8009DDF2 is read.
 *  @par Control flow
 *  Returns the global value directly.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
u8 func_80092B84(void)
{
    return D_8009DDF2;
}
