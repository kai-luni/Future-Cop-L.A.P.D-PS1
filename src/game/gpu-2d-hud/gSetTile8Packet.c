// Categories: Graphics > Primitive Initialization
#include "game/types.h"
void gSetTile8Packet(u8 *arg0) __asm__("func_80092550");


/**
 *  @brief Initializes two bytes of a graphics packet.
 *
 *  Sets the packet length byte to 2 and the command byte to 112.
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
 * Primitive initialiser for an 8x8 tile: sets length 2 (colour/code word plus
 * xy word) and code 0x70, leaving colour and position to the caller. No
 * caller references it in the sources.
 */
void gSetTile8Packet(u8 *arg0)
{
    arg0[3] = 2;
    arg0[7] = 112;
}
