// Categories: Graphics > Primitive Initialization
#include "game/types.h"
void gInitPolyF3Packet(u8 *arg0) __asm__("func_80092514");


/**
 *  @brief Initializes two bytes of a graphics packet.
 *
 *  Sets the packet length byte to 3 and the command byte to 124.
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
 * Initializes a graphics primitive packet header with length 3 and GP0
 * command code 0x20 (POLY_F3).
 */
void gInitPolyF3Packet(u8 *arg0)
{
    arg0[3] = 3;
    arg0[7] = 124;
}
