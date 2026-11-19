// Categories: Game Engine > Data Selection
#include "../../include/game/types.h"

extern u8 gCompareAddress[] __asm__("D_800A3C20");
u8 *gLookupPadPort(u32 flags) __asm__("func_8008C3EC");


/**
 *  @brief Selects one of two addresses in a global byte array.
 *
 *  Returns the array base when flags bits 4 through 7 are clear,
 *  or the address 0xF0 bytes past the base when any are set.
 *  @param flags Bits used to select the returned address.
 *  @return The selected address in D_800A3C20.
 *  @par Globals
 *  D_800A3C20 supplies the base address.
 *  @par Control flow
 *  Tests flags against 0xF0 and conditionally adds an offset.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Returns gCompareAddress (D_800A3C20) when (flags & 0xF0) is zero, or
 * gCompareAddress + 0xF0 when any of bits 4 through 7 are set. Bound to
 * D_8009C73C by func_8008BDB0 and called indirectly by pad slot helpers.
 */
u8 *gLookupPadPort(u32 flags)
{
    u8 *result;

    result = gCompareAddress;
    if ((flags & 0xF0) != 0) {
        result += 0xF0;
    }
    return result;
}
