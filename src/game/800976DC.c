// Categories: Platform > Dispatch
#include "game/types.h"

extern s32 D_8009EEC0;
extern u16 gSpuTransferAddr __asm__("D_8009EEBC");
extern s32 D_8009EECC;

extern void func_80097138(s32 arg0, s32 arg1);
extern void func_8009745C(s32 arg0, ...);

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
 *  func_8009745C handles the three operations; func_80097138 handles the
 *  alternative path.
 *  @par Side effects
 *  Invokes the selected helper or helpers; their effects are not shown.
 *  @par Callers
 *  Confirmed: func_80098E88.
 */
s32 func_800976DC(s32 arg0, s32 arg1)
{
    if (D_8009EEC0 == 0) {
        func_8009745C(2, gSpuTransferAddr << D_8009EECC);
        func_8009745C(1);
        func_8009745C(3, arg0, arg1);
    } else {
        func_80097138(arg0, arg1);
    }
    return arg1;
}
