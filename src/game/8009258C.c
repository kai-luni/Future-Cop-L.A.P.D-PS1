// Categories: Utilities > Byte Operations
#include "game/types.h"

/**
 *  @brief Writes two fixed byte values into a buffer.
 *
 *  Sets offsets 3 and 7 to 3 and 64, respectively.
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
void func_8009258C(u8 *arg0)
{
    arg0[3] = 3;
    arg0[7] = 64;
}
