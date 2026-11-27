void gSplitRgb24Shifted(unsigned int color, unsigned int *dst) __asm__("func_8001BCA4");

// Categories: Graphics > Color
/**
 *  @brief Splits a packed color into three shifted component words.
 *
 *  Extracts the low three bytes of color and places each in bits 16 through 23
 *  of a separate destination word, in high-to-low byte order.
 *  @param color Packed color value whose low three bytes are used.
 *  @param dst Destination array of at least three unsigned integers.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Writes the three components unconditionally.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites dst[0] through dst[2].
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Pure arithmetic helper not on the environment path: extracts the red,
 * green, and blue bytes from color and writes each shifted to bits 16-23 into
 * dst[0..2]. Touches no globals, makes no calls, and has no direct callers
 * found in src/.
 */
void gSplitRgb24Shifted(unsigned int color, unsigned int *dst)
{
    dst[0] = color & 0xFF0000;
    dst[1] = ((color >> 8) & 0xFF) << 16;
    dst[2] = (color & 0xFF) << 16;
}
