// Categories: Utilities > Table Access
#include "game/spu_globals.h"

extern unsigned short *gSpuRegBasePtr __asm__("D_8009EEA4");
extern int gSpuAddrShift __asm__("D_8009EECC");

/**
 *  @brief Reads a table entry with optional left shifting.
 *
 *  Fetches an unsigned 16-bit value from the indexed table and returns
 *  it unchanged for mode -1 or shifted for any other mode.
 *  @param index Index into the unsigned 16-bit table.
 *  @param mode Selects the unshifted result when equal to -1.
 *  @return The table value, optionally shifted left.
 *  @par Globals
 *  D_8009EEA4 points to the table; D_8009EECC supplies the shift count.
 *  @par Control flow
 *  Branches on mode to select the shifted or unshifted value.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
int func_800978AC(int index, int mode)
{
    register unsigned int value __asm__("$4") = gSpuRegBasePtr[index];
    int result;
    if (mode != -1)
        result = value << gSpuAddrShift;
    else
        result = value;
    return result;
}
