// Categories: Platform > Dispatch
#include "game/types.h"

extern s32 gOpOrderFlag __asm__("D_8009EEC0");
extern u16 gSpuTransferAddr __asm__("D_8009EEBC");
extern s32 gSpuAddrShift __asm__("D_8009EECC");

extern void gSpuWriteMemBlock(s32 arg0, s32 arg1) __asm__("func_80097138");
extern void gSpuTransfer(s32 arg0, ...) __asm__("func_8009745C");
s32 gSpuWriteTransferBlock(s32 arg0, s32 arg1) __asm__("func_800976DC");


/**
 *  @brief Dispatches two arguments through the selected global path.
 *
 *  The zero-state path issues three ordered operations, including a shifted
 *  global value; the other path forwards the arguments directly.
 *  @param arg0 First argument forwarded to the selected path.
 *  @param arg1 Second argument forwarded and returned unchanged.
 *  @return The value of arg1.
 *  @par Globals
 *  D_8009EEC0 selects the path; D_8009EEBC and D_8009EECC supply the shift.
 *  @par Control flow
 *  If D_8009EEC0 is zero, dispatches operations 2, 1, and 3 in that order.
 *  @par Calls
 *  gSpuTransfer handles the three operations; gSpuWriteMemBlock handles the
 *  alternative path.
 *  @par Side effects
 *  Invokes the selected helper or helpers; their effects are not shown.
 *  @par Callers
 *  Confirmed: func_80098E88.
 */
s32 gSpuWriteTransferBlock(s32 arg0, s32 arg1)
{
    if (gOpOrderFlag == 0) {
        gSpuTransfer(2, gSpuTransferAddr << gSpuAddrShift);
        gSpuTransfer(1);
        gSpuTransfer(3, arg0, arg1);
    } else {
        gSpuWriteMemBlock(arg0, arg1);
    }
    return arg1;
}
