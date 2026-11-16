// Categories: Graphics > Packet Setup
#include "game/types.h"

/**
 *  @brief Sets two bytes in a caller-provided buffer.
 *
 *  Writes fixed values at offsets 3 and 7, leaving the other bytes
 *  untouched. The source does not identify the buffer's format.
 *  @param arg0 Buffer with writable bytes at offsets 3 and 7.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Performs both stores unconditionally.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Sets arg0[3] to 7 and arg0[7] to 36.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80092474(u8 *arg0)
{
    arg0[3] = 7;
    arg0[7] = 36;
}
