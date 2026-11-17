// Categories: Platform > Transfer Management
#include "game/types.h"

extern s32 gSpuClearFlag __asm__("D_8009EED8");
extern s32 gSpuCompareWord __asm__("D_8009EEDC");
extern void func_80097760(s32 arg0, u32 arg1);

/**
 *  @brief Caps a requested size and passes it to a transfer-related function.
 *
 *  The size is limited to 0x7EFF0 before the call. Afterward, a global state
 *  value is cleared only if the other global state value is zero.
 *  @param arg0 First argument forwarded to func_80097760.
 *  @param arg1 Requested size, capped at 0x7EFF0.
 *  @return The capped size passed to func_80097760.
 *  @par Globals
 *  D_8009EEDC controls whether D_8009EED8 is cleared after the call.
 *  @par Control flow
 *  A size check precedes the call; a state check selects the return path.
 *  @par Calls
 *  func_80097760 receives arg0 and the capped size.
 *  @par Side effects
 *  May clear D_8009EED8; the called function may have additional effects.
 *  @par Callers
 *  Confirmed: func_80021490.
 */
u32 func_80098E28(s32 arg0, u32 arg1)
{
    u32 size;

    size = arg1;
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }

    func_80097760(arg0, size);

    if (gSpuCompareWord != 0) {
        return size;
    }
    gSpuClearFlag = 0;
    return size;
}
