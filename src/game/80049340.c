// Categories: Graphics > Text > Measurement
#include "game/types.h"

struct FontGlyph {
    u8 unk00[8];
    s8 advance;
};

struct FontEntry;

extern struct FontEntry *D_8009F35C;

extern struct FontGlyph *func_800489C8(struct FontEntry *font, u8 character);

/**
 * @brief Measures a string in the active font and returns twice its advance width.
 *
 * @param text NUL-terminated string; bytes below 0x20 are skipped.
 * @return 2 * (sum of the per-glyph advance values), 0 without a font.
 *
 * @par Confirmed
 * Same walk as func_800492A0 (func_800489C8(font, character)->advance for
 * characters >= 0x20 over D_8009F35C), but the total is multiplied by 2 and
 * returned directly instead of writing width/height outputs.
 *
 * @par Likely role
 * Pixel-width helper for the HUD and frame code: the font advances are
 * half-pixel units, so the doubling converts them to screen pixels.
 *
 * @par Unknown
 * Whether the factor 2 is a unit conversion or a deliberate double spacing.
 *
 * @par Callers
 * func_800267C8, func_80027510, func_80027F80, func_80047F30.
 */
s32 func_80049340(u8 *text)
{
    struct FontEntry *font;
    s32 total;
    u8 character;

    font = D_8009F35C;
    total = 0;
    if (font != 0) {
        while ((character = *text++) != 0) {
            if (character >= 0x20) {
                total += func_800489C8(font, character)->advance;
            }
        }
    }
    return total * 2;
}
