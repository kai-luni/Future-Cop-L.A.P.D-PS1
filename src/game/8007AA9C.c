// Categories: Graphics > HUD > Name Entry | Utilities > Character Table
#include "game/types.h"

/**
 * @brief Decodes glyph indices back into a name string, trimming trailing
 *        spaces from the right.
 *
 * @param dst Output buffer, len + 1 bytes (NUL written at dst[len]).
 * @param src The len glyph indices.
 * @param len Number of glyphs.
 * @param table The 64-entry glyph-to-character table.
 * @return Nothing.
 *
 * @par Confirmed
 * Walks right to left: each index maps through table[]; until the first
 * non-space (0x20) character is seen, cells are written as 0 (trimmed).
 * When the whole name is empty, dst[0] becomes a single space.
 *
 * @par Likely role
 * The commit path of the name-entry widgets func_8007ABBC/func_8008125C:
 * leaving the edit session turns the eight glyph cells back into the stored
 * name, dropping the padding spaces.
 *
 * @par Callers
 * func_8007ABBC, func_8008125C.
 */
void func_8007AA9C(u8 *dst, u8 *src, s32 len, u8 *table)
{
    s32 i;
    s32 flag = 0;

    dst[len] = 0;
    for (i = len - 1; i >= 0; i--) {
        u8 c = table[src[i]];
        if (c != 0x20) {
            flag = 1;
        }
        if (!flag) {
            c = 0;
        }
        dst[i] = c;
    }
    if (dst[0] == 0) {
        dst[0] = 0x20;
    }
}
