// Categories: Platform > Command Dispatch
#include "game/types.h"

extern u16 gSpuTransferAddr __asm__("D_8009EEBC");
extern s32 gSpuAddrShift __asm__("D_8009EECC");

extern void func_8009745C(s32 arg0, ...);

/**
 *  @brief Issues a three-command sequence and returns the supplied length.
 *
 *  The sequence uses a shifted shared value followed by the caller's two
 *  arguments; the underlying command protocol is not identified here.
 *  @param arg0 First argument passed with command 3.
 *  @param arg1 Second argument passed with command 3 and returned unchanged.
 *  @return The value of arg1.
 *  @par Globals
 *  D_8009EEBC supplies the value shifted by D_8009EECC for command 2.
 *  @par Control flow
 *  Dispatches commands 2, 0, and 3 in that order without branching.
 *  @par Calls
 *  func_8009745C receives each command and its associated arguments.
 *  @par Side effects
 *  The dispatched commands may modify state or perform external operations.
 *  @par Callers
 *  Confirmed: func_80098E28.
 */
s32 func_80097760(s32 arg0, s32 arg1)
{
    func_8009745C(2, gSpuTransferAddr << gSpuAddrShift);
    func_8009745C(0);
    func_8009745C(3, arg0, arg1);
    return arg1;
}
