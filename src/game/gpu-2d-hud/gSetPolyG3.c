// Categories: Graphics > Primitive Initialization
#include "game/types.h"
void gSetPolyG3(u8 *arg0) __asm__("func_80092488");


/**
 *  @brief Initializes two bytes of a graphics packet.
 *
 *  Sets the packet length byte to 6 and the command byte to 48.
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
void gSetPolyG3(u8 *arg0)
{
    arg0[3] = 6;
    arg0[7] = 48;
}
