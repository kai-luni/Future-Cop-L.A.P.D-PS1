// Categories: Game Engine > Audio
#include "game/types.h"

extern s32 func_80046B04(s32 arg0, s32 arg1, s32 arg2, s32 slot);

/**
 *  @brief Calls the sound routine with a fixed set of arguments.
 *
 *  Supplies 0x7F, 0xA, 2, and slot 0xFF to the underlying routine.
 *  This wrapper provides a fixed-argument entry point for its caller.
 *  @return The result returned by func_80046B04.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Makes one unconditional call and returns its result.
 *  @par Calls
 *  func_80046B04 performs the operation with the fixed arguments.
 *  @par Side effects
 *  Any effects are those of the called routine.
 *  @par Callers
 *  Confirmed: func_800453B8.
 */
s32 func_80046AD8(void)
{
    return func_80046B04(0x7F, 0xA, 2, 0xFF);
}
