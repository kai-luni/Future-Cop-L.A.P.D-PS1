// Categories: Graphics > Primitive Initialization
#include "game/types.h"

/**
 *  @brief Initializes two bytes of a graphics packet.
 *
 *  Sets the packet length byte to 9 and the command byte to 44.
 *  Other packet fields are left unchanged.
 *  @param arg0 Writable packet buffer of at least 8 bytes.
 *  @par Globals
 *  None accessed.
 *  @par Control flow
 *  Two sequential stores; no branches or loops.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes arg0[3] and arg0[7].
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_800924C4(u8 *arg0)
{
    arg0[3] = 9;
    arg0[7] = 44;
}
