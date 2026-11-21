// Categories: Graphics > HUD > Name Entry | Game Engine > Front-End Configuration
#include "game/types.h"

extern s8 gPlayerMenuDepth[] __asm__("D_8009F8C0");
extern s32 gPlayer1PrevMenuButtons[] __asm__("D_8009F8B0");
extern s32 gScalarCursor[] __asm__("D_8009F8F8");
extern u8 gPlaceholderGlyph[] __asm__("D_8009F470");
extern u8 gNameEntryGlyphBuf[] __asm__("D_800A3928");
extern s16 gPlayerNameLength __asm__("D_8009F87C");
extern u8 *gNameEntryAlphabet __asm__("D_8009F878");

extern void gEncodeNameToGlyphs(u8 *arg0, u8 *arg1, s32 arg2, u8 *arg3, s32 arg4) __asm__("func_8007A9D0");
extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");

/**
 * @brief Name-entry reset hook: on a masked input event, rewinds the cursor
 *        and re-encodes the placeholder name.
 *
 * @param arg0 Input/event mask, returned unchanged.
 * @param arg1 Player index.
 * @return arg0.
 *
 * @par Confirmed
 * Only when the edit flag D_8009F8C0[arg1] is set and (arg0 &
 * ~D_8009F8B0[arg1]) < 0: the cursor D_8009F8F8[arg1] becomes 0,
 * func_8007A9D0 encodes the placeholder D_8009F470 into the eight glyph
 * cells D_800A3928 + arg1 * 8 (table D_8009F878, length D_8009F87C), and
 * sound 0x48 plays.
 *
 * @par Likely role
 * The cancel/back action of the name widgets: the per-player mask
 * D_8009F8B0 selects which events trigger the reset, the placeholder is
 * the default name shown before editing.
 *
 * @par Callers
 * No direct callers in the call graph. func_80077AD4 stores the address in
 * three screen callback blocks (D_800A37A8.unk0C, D_800A37D8.unk0C,
 * D_800A36B8.unk0C), so the screen dispatcher reaches it indirectly.
 */
s32 gResetNameEntryState(s32 arg0, s32 arg1) __asm__("func_8007AB0C");

s32 gResetNameEntryState(s32 arg0, s32 arg1)
{
    if (gPlayerMenuDepth[arg1] != 0) {
        if ((arg0 & ~gPlayer1PrevMenuButtons[arg1]) < 0) {
            gScalarCursor[arg1] = 0;
            gEncodeNameToGlyphs(gPlaceholderGlyph, gNameEntryGlyphBuf + arg1 * 8, 8, gNameEntryAlphabet, gPlayerNameLength);
            gPlayNonpositionalSoundEffect(0x48, 0);
        }
    }
    return arg0;
}
