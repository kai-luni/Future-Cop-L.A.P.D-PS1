// Categories: Game Engine > Frame | Graphics > HUD
#include "game/types.h"

extern s32 gPlayerCount __asm__("D_8009FA0C");
extern void gUpdateFrameCloseHud(s32 arg) __asm__("func_800486FC");

void gRefreshSoloFrameHud(void) __asm__("func_80027D70");

/**
 * @brief Refreshes the frame-close HUD when the player count equals one.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Compares the player count word with 1. On equality calls func_800486FC with a0 equal to 1.
 *
 * @par Globals
 * Reads gPlayerCount.
 *
 * @par Callers
 * Direct calls from gRunGameFrameLoop (func_80028618).
 *
 * @par Role and path
 * The gameplay frame loop uses this to gate the solo HUD and portrait overlay update.
 *
 * @par Unknown
 * The overlay's deeper state checks live in func_800486FC.
 */
void gRefreshSoloFrameHud(void)
{
    if (gPlayerCount == 1) {
        gUpdateFrameCloseHud(1);
    }
}
