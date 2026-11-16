// Categories: Graphics > Text > Rendering | Game Engine > Init
#include "game/types.h"

extern s32 D_8009FAB8;
extern s32 D_8009F760;
extern s32 D_8009FAC8;

extern void func_80048EC4(void);

/**
 * @brief Resets the HUD/text layer state and reinitialises the text renderer.
 *
 * Three layer words are cleared first - the display mode D_8009FAB8 that func_800486FC compares against 2, the last-drawn element D_8009F760 and the frame-draw flag D_8009FAC8 - and func_80048EC4 then reinitialises the renderer.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009FAB8, D_8009F760, D_8009FAC8 - cleared here. D_8009F35C, D_8009F360, D_8009F364, D_8009F768, D_8009F76A, D_8009FACC - cleared inside func_80048EC4.
 *
 * @par Control flow
 * Three stores, then a single call; no branches.
 *
 * @par Calls
 * func_80048EC4 - empties both 20-entry colour rows, zeroes the font count and palette cursor and resets the palette to white.
 *
 * @par Side effects
 * Glyph memory stays allocated (only the counters are lost) while the palette is forced to white via func_80048978, so the HUD redraws from an empty state.
 *
 * @par Callers
 * Confirmed: func_80024904 (0x80024988) and func_80024A24 (0x80024C6C), the game-state reset and the mission init.
 */
void func_8004783C(void)
{
    D_8009FAB8 = 0;
    D_8009F760 = 0;
    D_8009FAC8 = 0;
    func_80048EC4();
}
