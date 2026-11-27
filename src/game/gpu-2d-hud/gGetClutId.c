// Categories: Graphics > Coordinate Encoding
#include "game/types.h"

u16 gGetClutId(s32 x, s32 y) __asm__("func_8009226C");

/**
 *  @brief Packs two coordinates into a 16-bit value.
 *
 *  The result combines y shifted left by six with bits 4 through 9 of x.
 *  The final value is converted to an unsigned 16-bit result.
 *  @param x Horizontal coordinate supplying the low six packed bits.
 *  @param y Coordinate shifted into the upper portion of the result.
 *  @return The packed coordinate value.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Computes and returns the expression without branches or loops.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_80092078, func_800920DC.
 */
u16 gGetClutId(s32 x, s32 y)
{
    return (y << 6) | ((x >> 4) & 0x3F);
}
