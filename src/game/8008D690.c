// Categories: Game Engine > State
#include "game/types.h"

extern void func_8008D6D0(s32);
extern void func_8008D670(s32, s32, s32);

/**
 *  @brief Calls two helpers in sequence for the supplied value.
 *
 *  The first helper receives the argument alone; the second receives it
 *  with the fixed values 0x3F and zero.
 *  @param arg0 Signed value passed to both helpers.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Calls the first helper and then the second without branching.
 *  @par Calls
 *  func_8008D6D0 receives arg0; func_8008D670 receives arg0, 0x3F, and 0.
 *  @par Side effects
 *  No direct writes; the helpers' effects are not specified here.
 *  @par Callers
 *  Confirmed: func_8001FD28.
 */
void func_8008D690(s32 arg0) {
    func_8008D6D0(arg0);
    func_8008D670(arg0, 0x3F, 0);
}
