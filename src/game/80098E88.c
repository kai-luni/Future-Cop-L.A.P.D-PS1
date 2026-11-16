// Categories: Platform > Transfer
#include "game/types.h"

extern s32 D_8009EED8;
extern s32 D_8009EEDC;
extern void func_800976DC(s32 arg0, u32 arg1);

/**
 *  @brief Starts a transfer with its size capped at 0x7EFF0 bytes.
 *
 *  Passes the capped size and caller-supplied argument to the transfer helper.
 *  It also clears a state flag when the associated status is zero.
 *  @param arg0 Argument forwarded to the transfer helper.
 *  @param arg1 Requested transfer size.
 *  @return The capped transfer size.
 *  @par Globals
 *  D_8009EEDC is tested; D_8009EED8 is cleared when it is zero.
 *  @par Control flow
 *  Caps the size, calls the helper, then branches on the status.
 *  @par Calls
 *  func_800976DC performs the operation using the capped size.
 *  @par Side effects
 *  May clear D_8009EED8; the helper may have additional effects.
 *  @par Callers
 *  Confirmed: func_80021490, func_8008684C.
 */
u32 func_80098E88(s32 arg0, u32 arg1)
{
    u32 size;

    size = arg1;
    if (size > 0x7EFF0) {
        size = 0x7EFF0;
    }

    func_800976DC(arg0, size);

    if (D_8009EEDC != 0) {
        return size;
    }
    D_8009EED8 = 0;
    return size;
}
