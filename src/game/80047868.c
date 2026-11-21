// Categories: Graphics > Text > Font | Utilities > Memory Management
#include "game/types.h"

extern void gFreeFontGlyphBuffers(void) __asm__("func_80048F3C");
void gFreeAllFontGlyphs(void) __asm__("func_80047868");


/**
 * @brief Releases the glyph data of every loaded font and nothing else.
 *
 * Thin wrapper over func_80048F3C, which walks the font table D_800A19A0 up to D_8009F364 and frees each entry's glyph pointer, leaving the counters and the palette untouched.
 *
 * @return Nothing.
 *
 * @par Globals
 * None directly; the font count D_8009F364 and the table D_800A19A0 are read inside func_80048F3C.
 *
 * @par Control flow
 * Single call, no locals and no branches.
 *
 * @par Calls
 * func_80048F3C - frees the glyph block of each font entry.
 *
 * @par Side effects
 * Frees the glyph blocks but leaves D_8009F364 non-zero, so a second call would free the same pointers again; the renderer still counts the fonts it no longer owns.
 *
 * @par Callers
 * Confirmed: func_800249BC (0x800249E4) and func_80024D14 (0x80024DB8), the two subsystem teardown sequences.
 */
void gFreeAllFontGlyphs(void)
{
    gFreeFontGlyphBuffers();
}
