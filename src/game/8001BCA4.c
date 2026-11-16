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
 */
void func_8001BCA4(unsigned int color, unsigned int *dst)
{
    dst[0] = color & 0xFF0000;
    dst[1] = ((color >> 8) & 0xFF) << 16;
    dst[2] = (color & 0xFF) << 16;
}
