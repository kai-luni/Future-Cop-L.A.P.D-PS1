int gGetFontBitsPerPixel(unsigned char *value) __asm__("func_800489EC");

// Categories: Graphics > Text > Font Upload
/**
 * @brief Maps a font format byte to its upload code.
 *
 * @param value Pointer to the format byte.
 * @return 1 for format 0x44, 4 for format 0x40 and for every other value.
 *
 * @par Confirmed
 * Two comparisons, default 4; the initial result is also 4, so unknown
 * formats behave like 0x40.
 *
 * @par Likely role
 * The depth selector of the font registration func_80048A14: 0x40 and 0x44
 * are the two CLUT depths of the font textures, and the returned code picks
 * the upload path (likely 4-bit vs 1-bit packing).
 *
 * @par Unknown
 * Whether the code is a bit depth or a texture-page mode.
 *
 * @par Callers
 * func_80048A14.
 */
int gGetFontBitsPerPixel(unsigned char *value)
{
    int result = 4;

    if (*value == 0x40) {
        result = 4;
    } else if (*value == 0x44) {
        result = 1;
    }
    return result;
}
