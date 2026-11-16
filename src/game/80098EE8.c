// Categories: Platform > Address Access
#include "game/types.h"

extern volatile u16 gSpuTransferAddr __asm__("D_8009EEBC");
extern u32 D_8009EECC;

extern u32 func_80097808(s32 register_index, u32 address);

/**
 *  @brief Reads and shifts a value for an address in the accepted range.
 *
 *  Validates the address against the inclusive range 0x1010 to 0x7FFF8.
 *  A valid read is stored in a shared 16-bit value before being shifted.
 *  @param address Address to validate and pass to the read helper.
 *  @return The shifted stored value, or zero for an invalid address.
 *  @par Globals
 *  D_8009EEBC receives the truncated read result; D_8009EECC supplies the shift.
 *  @par Control flow
 *  An unsigned offset check rejects out-of-range addresses before the call.
 *  @par Calls
 *  func_80097808 reads using register index -1 and the supplied address.
 *  @par Side effects
 *  Writes D_8009EEBC for valid addresses; the helper may have other effects.
 *  @par Callers
 *  Confirmed: func_80021490, func_8008684C.
 */
u32 func_80098EE8(u32 address)
{
    u32 offset;

    offset = address - 0x1010;
    if (offset > 0x7EFE8) {
        return 0;
    }

    gSpuTransferAddr = func_80097808(-1, address);
    return (u32)gSpuTransferAddr << D_8009EECC;
}
