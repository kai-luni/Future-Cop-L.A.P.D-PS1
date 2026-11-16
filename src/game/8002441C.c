// Categories: Game Engine > Dispatch
#include "game/types.h"

extern void func_80022654(void);

/**
 *  @brief Calls func_80022654 without arguments.
 *
 *  This wrapper delegates its entire operation to the called function.
 *  The purpose of the delegated operation is not established here.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Makes one unconditional call and returns.
 *  @par Calls
 *  func_80022654 performs the delegated operation.
 *  @par Side effects
 *  Any effects of func_80022654 are not specified by this source.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8002441C(void)
{
    func_80022654();
}
