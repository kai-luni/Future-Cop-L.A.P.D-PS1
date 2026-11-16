// Categories: Game Engine > Dispatch
#include "game/types.h"

extern void func_8008A5A8(void);

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
 */
void func_80089DE0(void)
{
    func_8008A5A8();
}
