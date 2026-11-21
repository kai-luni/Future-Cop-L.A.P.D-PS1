// Categories: Graphics > Primitive Initialization
#include "game/types.h"
void gSetPolyGT4Packet(u8 *arg0) __asm__("func_800924EC");


/**
 *  @brief Initializes two bytes of a graphics packet.
 *
 *  Sets the packet length byte to 12 and the command byte to 60.
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
void gSetPolyGT4Packet(u8 *arg0)
{
    arg0[3] = 12;
    arg0[7] = 60;
}
