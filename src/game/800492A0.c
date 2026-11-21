// Categories: Graphics > Text > Measurement
#include "game/types.h"

struct FontGlyph {
    u8 unk00[8];
    s8 advance;
};

struct FontEntry {
    u8 unk00[6];
    s16 height;
};

extern struct FontEntry *gFont __asm__("D_8009F35C");

extern struct FontGlyph *gLookupFontGlyph(struct FontEntry *font, u8 character) __asm__("func_800489C8");
void gMeasureTextExtent(u8 *text, s32 *width, s32 *height) __asm__("func_800492A0");


/**
 * @brief Measures a string in the active font: total advance width and line height.
 *
 * @param text NUL-terminated string; bytes below 0x20 are skipped.
 * @param width Out: sum of the per-glyph advance values.
 * @param height Out: the font's line height, 0 when no font is installed.
 * @return Nothing.
 *
 * @par Confirmed
 * With D_8009F35C non-null, walks the string and adds
 * func_800489C8(font, character)->advance for every character >= 0x20;
 * the height comes from the font entry's height field. Without a font both
 * outputs are 0. The advance is the signed byte at glyph offset +8.
 *
 * @par Likely role
 * The layout helper behind centred text: the name-entry and loadout widgets
 * call it before drawing so they can centre or frame the string.
 *
 * @par Callers
 * func_800788F0, func_8007ABBC, func_8007E1A4, func_8007F1F8, func_8008125C.
 */
void gMeasureTextExtent(u8 *text, s32 *width, s32 *height)
{
    s32 total;
    s32 lineHeight;
    u8 character;

    lineHeight = total = 0;
    if (gFont != 0) {
        for (;;) {
            character = *text;
            if (character == 0) {
                break;
            }
            text++;
            if (character < 0x20) {
                continue;
            }
            total += gLookupFontGlyph(gFont, character)->advance;
        }
        lineHeight = gFont->height;
    }
    *height = lineHeight;
    *width = total;
}
