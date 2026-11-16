// Categories: Game Engine > State Access
#include "../../include/game/types.h"

extern s32 D_8009C790;

/**
 *  @brief Returns and clears a signed global value.
 *
 *  Captures the current value before resetting the global to zero.
 *  @return The value of D_8009C790 before it was cleared.
 *  @par Globals
 *  D_8009C790 is read and then set to zero.
 *  @par Control flow
 *  Performs the read and reset unconditionally.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Clears D_8009C790.
 *  @par Callers
 *  Confirmed: func_80089DE0.
 */
s32 func_8008A5A8(void)
{
    s32 previous;

    previous = D_8009C790;
    D_8009C790 = 0;
    return previous;
}
