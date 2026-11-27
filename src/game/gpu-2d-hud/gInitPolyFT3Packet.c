// Categories: Graphics > Primitive Initialization
#include "game/types.h"
void gInitPolyFT3Packet(u8 *arg0) __asm__("func_8009253C");


/**
 *  @brief Initializes two bytes of a graphics packet.
 *
 *  Sets the packet length byte to 2 and the command byte to 104.
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
 *
 * @par Environment path
 * Initializes a graphics primitive packet header with length 4 and GP0
 * command code 0x24 (POLY_FT3).
 */
void gInitPolyFT3Packet(u8 *arg0)
{
    arg0[3] = 2;
    arg0[7] = 104;
}
