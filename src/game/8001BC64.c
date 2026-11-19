// Categories: Graphics > Color
#include "game/types.h"
void gScaleRgb24(u32 color, s32 scale, s32 *out) __asm__("func_8001BC64");


/**
 *  @brief Multiplies packed RGB components by a scale.
 *
 *  Extracts the red, green, and blue bytes from a packed color and
 *  writes their individual products to a three-element output array.
 *  @param color Packed color with red in bits 16-23 and blue in bits 0-7.
 *  @param scale Signed multiplier applied to each component.
 *  @param out Destination for the red, green, and blue products.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Computes and stores all three components without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes three signed integers to out.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Pure arithmetic helper not on the environment path: extracts the red,
 * green, and blue bytes from a 24-bit color, multiplies each by signed scale,
 * and writes the products to out[0..2]. Touches no globals, makes no calls,
 * and has no direct callers found in src/.
 */
void gScaleRgb24(u32 color, s32 scale, s32 *out)
{
    s32 r, g, b;
    r = (s32)((color >> 16) & 255) * scale;
    g = (s32)((color >> 8) & 255) * scale;
    b = (s32)(color & 255) * scale;
    out[0] = r;
    out[1] = g;
    out[2] = b;
}
