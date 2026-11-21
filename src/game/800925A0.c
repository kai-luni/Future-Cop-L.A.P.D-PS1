// Categories: Utilities > Byte Operations
#include "game/types.h"
void gSetLineG2(u8 *arg0) __asm__("func_800925A0");


/**
 *  @brief Writes two fixed byte values into a buffer.
 *
 *  Sets offsets 3 and 7 to 4 and 80, respectively.
 *  The meaning of these fields is not established by the source.
 *  @param arg0 Buffer with writable bytes at offsets 3 and 7.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Performs two unconditional stores.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies two bytes of the supplied buffer.
 *  @par Callers
 *  Unknown: No direct callers are listed.
 */
void gSetLineG2(u8 *arg0)
{
    arg0[3] = 4;
    arg0[7] = 80;
}
