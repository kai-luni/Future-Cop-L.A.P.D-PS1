// Categories: Utilities > Character Table | Graphics > Text
#include "game/types.h"

extern u8 D_8009B3EC[];
extern u8 *gNameEntryAlphabet __asm__("D_8009F878");
extern s16 gPlayerNameLength __asm__("D_8009F87C");

/**
 * @brief Installs the global character/encoding table used by the text and
 *        name-entry code.
 *
 * D_8009F878 points at D_8009B3EC, which holds the 64-character sequence
 * "A-Z a-z 0-9 '_' and space", and D_8009F87C is set to 0x40, its length.
 *
 * @param (none) - the case passes 0 in $a0, but the function is declared
 *        void and ignores it; the parameter exists only in the caller.
 *
 * @return Nothing.
 *
 * @par Globals written
 * D_8009F878 (table pointer), D_8009F87C (entry count).
 *
 * @par Likely role
 * The table backs the "pick a letter" grids (memory-card/name entry) and the
 * text encoder: every glyph index is mapped through it.
 *
 * @par Callers
 * func_80076A2C, once, right before the front-end loop starts.
 */
void func_800780C0(void)
{
    gNameEntryAlphabet = D_8009B3EC;
    gPlayerNameLength = 0x40;
}
