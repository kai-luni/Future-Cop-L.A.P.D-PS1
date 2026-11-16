// Categories: Game Engine > State Access
#include "game/types.h"

extern u16 *D_8009DD18;

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
 */
u16 func_800915E4(void)
{
    return *D_8009DD18;
}
