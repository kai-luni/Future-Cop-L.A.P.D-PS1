// Categories: Game Engine > Frame | Graphics > HUD
#include "game/types.h"

extern s32 gScreenState[] __asm__("D_8009FA00");
extern s32 gPlayerCount[] __asm__("D_8009FA0C");
extern u8 gPortraitStreamId[] __asm__("D_8009F9B4");
extern s32 gHudMode2 __asm__("D_8009FAB8");
extern s32 gFrameSwitch __asm__("D_8009FAC8");

extern void gPlayPendingSessionSoundEffect(void) __asm__("func_80049260");
extern s32 func_80029748(u8 *name);
extern void gUpdateHudLayoutMode(u8 force) __asm__("func_800487CC");
extern void gEmitHudPanelSprite(s32 value) __asm__("func_800485AC");
void gUpdateFrameCloseHud(u8 enable) __asm__("func_800486FC");


/**
 * @brief Frame-close hook: refreshes the pause overlay, resolves the player
 *        name and conditionally resets the status line.
 *
 * @param enable Non-zero allows the status reset in single-player mode 2.
 * @return Nothing.
 *
 * @par Confirmed
 * In screen mode 4 (D_8009FA00[0]) func_80049260 runs first. Then
 * D_8009FAC8 = func_80029748(D_8009F9B4) resolves the name buffer,
 * func_800487CC(0) refreshes the HUD, and when D_8009FAB8 == 2, enable is
 * set and D_8009FA0C[0] == 1, func_800485AC(0) resets the status line.
 *
 * @par Likely role
 * The per-frame HUD close of the two-player driver func_80027510 and the
 * screen handler func_80027D70: the name resolution feeds the status line,
 * the reset only fires in the single-player pause configuration.
 *
 * @par Unknown
 * What func_80029748 returns for the name buffer and the meaning of
 * D_8009FAB8 == 2.
 *
 * @par Callers
 * func_80027510, func_80027D70.
 */
void gUpdateFrameCloseHud(u8 enable)
{
    if (gScreenState[0] == 4) {
        gPlayPendingSessionSoundEffect();
    }
    gFrameSwitch = func_80029748(gPortraitStreamId);
    gUpdateHudLayoutMode(0);
    if (gHudMode2 != 0 && gHudMode2 == 2 && enable && gPlayerCount[0] == 1) {
        gEmitHudPanelSprite(0);
    }
}
