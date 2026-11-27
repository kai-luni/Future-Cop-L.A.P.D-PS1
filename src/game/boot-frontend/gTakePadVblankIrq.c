// Categories: Game Engine > State Access
#include "../../include/game/types.h"

extern s32 gPadVblankIrqFlag __asm__("D_8009C790");
s32 gTakePadVblankIrq(void) __asm__("func_8008A5A8");


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
 *
 * @par Role and path
 * Reads gPadVblankIrqFlag, resets it to 0, and returns the previous value.
 * Called by func_80089DE0.
 */
s32 gTakePadVblankIrq(void)
{
    s32 previous;

    previous = gPadVblankIrqFlag;
    gPadVblankIrqFlag = 0;
    return previous;
}
