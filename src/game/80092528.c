// Categories: Graphics > Primitive Initialization
#include "game/types.h"
void gSetSpritePacket(u8 *arg0) __asm__("func_80092528");


/**
 *  @brief Initializes two bytes of a graphics packet.
 *
 *  Sets the packet length byte to 4 and the command byte to 100.
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
 * Primitive initialiser for a free-size sprite: sets length 4 (colour/code,
 * xy, uv plus CLUT, wh) and code 0x64, opaque and colour-modulated, the same
 * header that func_8002549C and func_800485AC write inline. No caller
 * references it in the sources.
 */
void gSetSpritePacket(u8 *arg0)
{
    arg0[3] = 4;
    arg0[7] = 100;
}
