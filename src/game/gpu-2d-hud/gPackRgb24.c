// Categories: Graphics > Color
#include "game/types.h"
u32 gPackRgb24(u32 r, u32 g, u32 b) __asm__("func_8001BC44");


/**
 *  @brief Packs the low bytes of three color components into an RGB word.
 *
 *  Places red in bits 16 through 23, green in bits 8 through 15,
 *  and blue in bits 0 through 7; the upper byte is zero.
 *  @param r Red component; only its low byte is used.
 *  @param g Green component; only its low byte is used.
 *  @param b Blue component; only its low byte is used.
 *  @return The packed 0x00RRGGBB value.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Masks, shifts, and combines the components without branching or looping.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_8001BD70, func_8001BDE4.
 *
 * @par Role and path
 * Pure arithmetic helper not on the environment path: masks r, g, and b to 8
 * bits and packs them into 0x00RRGGBB order with no global accesses. Called
 * by func_8001BD70 and func_8001BDE4; makes no outgoing calls.
 */
u32 gPackRgb24(u32 r, u32 g, u32 b)
{
    return ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
}
