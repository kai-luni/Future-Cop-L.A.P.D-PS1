// Categories: Graphics > Color
#include "game/types.h"

/**
 *  @brief Accumulates scaled RGB components into three output integers.
 *
 *  The low 24 bits of the packed color supply red, green, and blue bytes.
 *  Each component is multiplied by the scale and added to its output slot.
 *  @param color Packed color with red in bits 16-23, green in 8-15, and blue in 0-7.
 *  @param scale Signed multiplier applied to each component.
 *  @param out Array of at least three accumulators in red, green, blue order.
 *  @par Globals
 *  None referenced.
 *  @par Control flow
 *  The three components are extracted and accumulated without branches or loops.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Adds the scaled components to out[0], out[1], and out[2].
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8001BCD0(u32 color, s32 scale, s32 *out)
{
    s32 r, g, b;

    r = ((color >> 16) & 0xff) * scale;
    g = ((color >> 8) & 0xff) * scale;
    b = (color & 0xff) * scale;
    out[0] += r;
    out[1] += g;
    out[2] += b;
}
