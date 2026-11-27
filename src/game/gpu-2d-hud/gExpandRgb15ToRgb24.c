// Categories: Graphics > Color
extern unsigned int gPackRgb24(unsigned int, unsigned int, unsigned int) __asm__("func_8001BC44");
unsigned int gExpandRgb15ToRgb24(unsigned int color) __asm__("func_8001BDE4");


/**
 *  @brief Expands a packed 15-bit RGB color and passes it to a color function.
 *
 *  Five-bit red, green, and blue components are expanded to eight bits by
 *  shifting left three places and repeating their high bits in the low positions.
 *  @param color Packed color with red in bits 10-14, green in 5-9, and blue in 0-4.
 *  @return The value returned by func_8001BC44 for the expanded components.
 *  @par Globals
 *  None referenced.
 *  @par Control flow
 *  Components are extracted and expanded before a single call.
 *  @par Calls
 *  func_8001BC44 receives the expanded red, green, and blue values.
 *  @par Side effects
 *  Any additional effects are performed by func_8001BC44.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Pure arithmetic helper not on the environment path: extracts 5-bit R, G, B
 * fields from a 15-bit color, expands each to 8 bits via (c << 3) | (c >> 2),
 * and calls func_8001BC44. Touches no globals and has no direct callers found
 * in src/.
 */
unsigned int gExpandRgb15ToRgb24(unsigned int color)
{
    unsigned int r, g, b;
    r = ((unsigned short)color >> 10) & 0x1F;
    r = (r << 3) | (r >> 2);
    g = ((unsigned short)color >> 5) & 0x1F;
    g = (g << 3) | (g >> 2);
    b = color & 0x1F;
    b = (b << 3) | (b >> 2);
    return gPackRgb24(r, g, b);
}
