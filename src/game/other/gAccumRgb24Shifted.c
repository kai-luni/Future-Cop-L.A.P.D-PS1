void gAccumRgb24Shifted(unsigned int color, unsigned int *dst) __asm__("func_8001BD2C");

// Categories: Graphics > Color
/**
 *  @brief Adds packed RGB components to three accumulators.
 *
 *  Extracts each color byte, places it in bits 16-23, and adds it to
 *  the corresponding red, green, or blue destination element.
 *  @param color Packed color with red in bits 16-23 and blue in bits 0-7.
 *  @param dst Three unsigned accumulators in red, green, blue order.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Updates all three accumulators without branching.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the three words at dst using unsigned addition.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 *
 * @par Role and path
 * Pure arithmetic helper not on the environment path: extracts the red,
 * green, and blue bytes from color, shifts each into bits 16-23, and adds
 * them to dst[0..2]. Touches no globals, makes no calls, and has no direct
 * callers found in src/.
 */
void gAccumRgb24Shifted(unsigned int color, unsigned int *dst)
{
    dst[0] += color & 0xFF0000;
    dst[1] += ((color >> 8) & 0xFF) << 16;
    dst[2] += (color & 0xFF) << 16;
}
