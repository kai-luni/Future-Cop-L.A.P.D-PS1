// Categories: Graphics > Display Setup | Platform > GPU
#include "game/types.h"

extern u8 gDisplayEnabled __asm__("D_8009F608");
void gSetDisplayEnabled(u8 value) __asm__("func_8001BFD8");


/**
 * @brief Sets the display-control byte the render set-up reads.
 *
 * @param value New value (0..255), stored verbatim.
 * @return Nothing.
 *
 * @par Confirmed
 * Single store to D_8009F608; the readers are the display/render set-up helpers
 * (func_8001C034, func_8001C040).
 *
 * @par Role and path
 * Stores the supplied u8 flag verbatim into gDisplayEnabled (D_8009F608),
 * which controls whether fade layers are enabled on layer flips. Driven by
 * func_80028618 and func_800259D8; makes no outgoing calls.
 */
void gSetDisplayEnabled(u8 value)
{
    gDisplayEnabled = value;
}
