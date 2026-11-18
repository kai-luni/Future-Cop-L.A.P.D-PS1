// Categories: Game Engine > Front-End Configuration | Graphics > Text > Encoding
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");

extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern s32 func_8008CCC0(u8 *arg0);
extern s32 func_800210D0(u8 arg0);

/**
 * @brief Encodes a profile name string into the front-end block's glyph
 *        buffer.
 *
 * @param arg0 Name string (bytes).
 * @return Nothing.
 *
 * @par Confirmed
 * Clears 0x40 bytes at D_8009F92C + 4, measures the string with the BIOS
 * strlen func_8008CCC0, then maps each byte through func_800210D0 into an
 * s16 glyph slot.
 *
 * @par Likely role
 * The name-upload step of the profile image builder func_80080B70: the
 * 0x40-byte buffer holds up to 32 glyphs, the mapping converts the stored
 * character codes to the font's glyph indices.
 *
 * @par Callers
 * func_80080B70.
 */
void gEncodeProfileNameGlyphs(u8 *arg0) __asm__("func_800822CC");

void gEncodeProfileNameGlyphs(u8 *arg0)
{
    s32 count;
    s32 i;
    s16 *dst;
    u8 *src = arg0;

    func_8001E4C0(gSaveProfileImage + 4, 0, 0x40);
    count = func_8008CCC0(src);
    dst = (s16 *)(gSaveProfileImage + 4);
    for (i = 0; i < count; i++) {
        *dst = func_800210D0(*arg0);
        arg0++;
        dst++;
    }
}
