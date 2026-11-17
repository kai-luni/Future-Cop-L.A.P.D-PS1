// Categories: Graphics > Text | Utilities > Memory Management
#include "game/types.h"

struct FontEntry {
    u8 unk00[0x18];
    void *glyphs;
    u8 unk1C[0x100];
};

extern struct FontEntry D_800A19A0[];
extern s32 gFontBankCount __asm__("D_8009F364");

extern void func_800181D4(void *block);

/**
 * @brief Frees the glyph buffers for all loaded fonts in the font table.
 *
 * Iterates across D_800A19A0[0..D_8009F364-1] (0x11C-byte FontEntry records) and frees
 * each entry's glyphs pointer (+0x18) without resetting D_8009F364 or palette state.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F364 - count of loaded font entries; D_800A19A0 - array of FontEntry records whose glyphs (+0x18) pointers are freed.
 * @par Control flow
 * Single counted for loop from i = 0 while i < D_8009F364.
 * @par Calls
 * func_800181D4 - frees D_800A19A0[i].glyphs (direct jal at 0x80048F64 in asm/us/main.s).
 * @par Side effects
 * Deallocates each font's glyph memory via func_800181D4 while leaving D_8009F364 non-zero.
 * @par Callers
 * Confirmed: func_80047868 (font glyph teardown wrapper).
 */
void func_80048F3C(void)
{
    s32 i;

    for (i = 0; i < gFontBankCount; i++) {
        func_800181D4(D_800A19A0[i].glyphs);
    }
}
