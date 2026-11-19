// Categories: Game Engine > Dispatch
#include "game/types.h"

extern void gTakePadVblankIrq(void) __asm__("func_8008A5A8");
void gTakePadVblankFlag(void) __asm__("func_80089DE0");


/**
 *  @brief Calls func_8008A5A8 without arguments.
 *
 *  This wrapper delegates its entire operation to the called function.
 *  The purpose of the delegated operation is not established here.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Makes one unconditional call and returns.
 *  @par Calls
 *  func_8008A5A8 performs the delegated operation.
 *  @par Side effects
 *  Any effects of func_8008A5A8 are not specified by this source.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Thin wrapper that calls func_8008A5A8 to read and clear gPadVblankIrqFlag
 * (D_8009C790). No direct callers are present in the codebase.
 */
void gTakePadVblankFlag(void)
{
    gTakePadVblankIrq();
}
