// Categories: Game Engine > Dispatch
#include "game/types.h"

extern s32 D_8009F64C;

/**
 *  @brief Forwards values in the range three through four to a helper.
 *
 *  This wrapper filters its argument before dispatching it unchanged.
 *  Values outside the accepted range cause no call.
 *  @param arg0 Signed value to test and potentially forward.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Checks for a value below five and then for a value at least three.
 *  @par Calls
 *  func_800246A8 receives arg0 when both checks pass.
 *  @par Side effects
 *  No direct writes; the helper's effects are not specified here.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80024470(s32 arg0)
{
    if (arg0 < 5) {
        if (arg0 >= 3) {
            func_800246A8(arg0);
        }
    }
}
