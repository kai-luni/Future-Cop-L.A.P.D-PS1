// Categories: Platform > Command Dispatch
#include "game/types.h"

extern void func_8008DFF0(u32 arg0, s32 arg1);
void gLogCdReadCallback(void) __asm__("func_8008DFBC");


/**
 *  @brief Dispatches a fixed command with argument 0x40.
 *
 *  Provides a no-argument entry point for passing the constant command
 *  0xF0000003 and its associated value to the dispatch function.
 *  @par Globals
 *  None directly accessed.
 *  @par Control flow
 *  Makes one unconditional call and returns.
 *  @par Calls
 *  func_8008DFF0 receives 0xF0000003 and 0x40.
 *  @par Side effects
 *  Any effects are those of the called function.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Calls func_8008DFF0 with constant arguments 0xF0000003 and 0x40 and
 * returns. Accesses no globals directly; its address is installed as the
 * default D_8009C810 callback by func_8008DEA0.
 */
void gLogCdReadCallback(void)
{
    func_8008DFF0(0xF0000003, 0x40);
}
