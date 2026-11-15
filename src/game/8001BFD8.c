// Categories: Graphics > Display Setup | Platform > GPU
#include "game/types.h"

extern u8 D_8009F608;

/**
 * @brief Sets the display-control byte the render set-up reads.
 *
 * @param value New value (0..255), stored verbatim.
 * @return Nothing.
 *
 * @par Confirmed
 * Single store to D_8009F608; the readers are the display/render set-up helpers
 * (func_8001C034, func_8001C040).
 */
void func_8001BFD8(u8 value)
{
    D_8009F608 = value;
}
