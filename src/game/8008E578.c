// Categories: Game Engine > Dispatch
#include "game/types.h"

extern s32 func_8008F650(u8 *arg0);

/**
 *  @brief Passes a byte pointer to func_8008F650 and returns 1.
 *
 *  The callee's return value is discarded; this wrapper reports the
 *  same result regardless of that value.
 *  @param arg0 Byte pointer passed to the callee.
 *  @return Always 1.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Makes one unconditional call, then returns a constant.
 *  @par Calls
 *  func_8008F650 receives arg0 and performs the delegated operation.
 *  @par Side effects
 *  Any effects of func_8008F650 are not specified by this source.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_8008E578(u8 *arg0)
{
    func_8008F650(arg0);
    return 1;
}
