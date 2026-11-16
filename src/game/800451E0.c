// Categories: Platform > Sound
#include "game/types.h"

extern s32 D_8009F730;

/**
 * @brief Returns the global sound-mode (stereo/enable) flag D_8009F730.
 *
 * Confirmed: returns D_8009F730 unchanged; cross-referencing D_8007D18C, D_8007D27C, func_800451B4, func_800244A4, and func_80024730 shows D_8009F730 is toggled via func_800451B4(func_800451E0() ^ 1) in the options menu and selects stereo panning (0 / 0x7F) vs mono center pan (0x40, 0x40) on stream channels as well as stereo spatialization in func_800458E8.
 * Category follows direct read of D_8009F730 paired with setter func_800451B4 and callers across audio streaming and options menus.
 * Likely: stereo-vs-mono / sound-mode boolean getter (0 = mono, 1 = stereo); Unknown: exact meaning of its saved slot in func_80080A5C (arg0[7]) and func_80025F58 (D_8009ABE8[4] = 4).
 *
 * @param void No parameters.
 * @return Current value of D_8009F730 (0 or 1 as written by func_800451B4).
 *
 * @par Globals
 * D_8009F730 - global sound-mode flag read and returned.
 * @par Control flow
 * Single load and return; no branches or loops.
 * @par Calls
 * None.
 * @par Side effects
 * None observed (read-only getter).
 * @par Callers
 * Confirmed: D_8007D18C (0x8007D1BC), D_8007D27C (0x8007D29C, 0x8007D2AC), func_80025F58 (0x80026078), func_80080A5C (0x80080A80), func_800244A4 (0x8002459C in asm/us/main.s), func_80024730 (0x80024748 in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 func_800451E0(void)
{
    return D_8009F730;
}
