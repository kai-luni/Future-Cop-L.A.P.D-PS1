// Categories: Platform > Register Access
#include "game/types.h"

extern u32 gSpuEncodeOrWriteAddr(s32 register_index, u32 address) __asm__("func_80097808");
u32 gSetSpuIrqAddress(u32 address) __asm__("func_80098BB8");


/**
 *  @brief Validates an address before passing it to a register routine.
 *
 *  Accepts addresses through 0x7FFF8 and uses register index 0xD2.
 *  Larger addresses are rejected without calling the underlying routine.
 *  @param address Address to validate and pass to the register routine.
 *  @return Zero if rejected; otherwise the result of func_80097808.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Branches on the upper bound and calls the routine only for valid addresses.
 *  @par Calls
 *  func_80097808 receives register index 0xD2 and the address.
 *  @par Side effects
 *  Any effects for an accepted address are those of the called routine.
 *  @par Callers
 *  Confirmed: func_80022F44, func_80022FA8.
 */
u32 gSetSpuIrqAddress(u32 address)
{
    if (address > 0x7FFF8) {
        return 0;
    }
    return gSpuEncodeOrWriteAddr(0xD2, address);
}
