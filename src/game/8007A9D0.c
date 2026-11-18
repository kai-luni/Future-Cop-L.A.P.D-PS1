// Categories: Graphics > HUD > Name Entry | Utilities > Character Table
#include "game/types.h"

extern s32 func_8008CCC0(u8 *str);

/**
 * @brief Encodes a name string into glyph indices (inverse of func_8007AA9C).
 *
 * @param src The name string.
 * @param dst Output glyph indices, len bytes.
 * @param len Number of glyph cells.
 * @param table The character-to-glyph table.
 * @param tableLen Table length; characters not found map to index 0.
 * @return Nothing.
 *
 * @par Confirmed
 * The string length comes from the BIOS strlen stub func_8008CCC0; cells
 * beyond it are filled with the space character 0x20. Each character is
 * searched linearly in the table; a miss stores index 0.
 *
 * @par Likely role
 * The edit-session entry of the name widgets func_8007ABBC/func_8008125C:
 * the stored name becomes eight editable glyph cells, padded with spaces.
 *
 * @par Callers
 * func_8007AB0C, func_8007ABBC, func_8008125C.
 */
void gEncodeNameToGlyphs(u8 *src, u8 *dst, s32 len, u8 *table, s32 tableLen) __asm__("func_8007A9D0");

void gEncodeNameToGlyphs(u8 *src, u8 *dst, s32 len, u8 *table, s32 tableLen)
{
    s32 i;
    s32 j;
    s32 limit;
    s32 c;

    limit = func_8008CCC0(src);
    for (i = 0; i < len; i++) {
        if (i < limit) {
            c = src[i];
        } else {
            c = 0x20;
        }
        for (j = 0; j < tableLen; j++) {
            if (c == table[j]) {
                break;
            }
        }
        if (j == tableLen) {
            j = 0;
        }
        dst[i] = j;
    }
}
