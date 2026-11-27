// Categories: Game Engine > State Access
#include "game/intc_globals.h"

#include "game/types.h"

extern u16 *gInterruptMaskPtr __asm__("D_8009DD18");
u16 gGetIrqMask(void) __asm__("func_800915E4");


/**
 *  @brief Returns the unsigned halfword referenced by a global pointer.
 *
 *  Reads the current value through D_8009DD18.
 *  The source does not identify the value's purpose.
 *  @return The unsigned 16-bit value at the pointed-to address.
 *  @par Globals
 *  D_8009DD18 supplies the address to read.
 *  @par Control flow
 *  Dereferences the pointer and returns without validation.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None beyond the memory read.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 *
 * @par Role and path
 * Reads and returns the current 16-bit PS1 hardware interrupt mask register
 * I_MASK (0x1F801074) through pointer D_8009DD18.
 */
u16 gGetIrqMask(void)
{
    return *gInterruptMaskPtr;
}
