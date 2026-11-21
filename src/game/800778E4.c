// Categories: Game Engine > Front End | Game Engine > Menu System
#include "game/types.h"

extern volatile s32 gFrameTickCounter __asm__("D_8009FA2C");
extern s32 gIdleDeadline __asm__("D_8009F870");
extern s32 gTitleScreenDeadline __asm__("D_8009F8E8");
extern u8 gTitleScreenConfirmEnabled __asm__("D_8009F46C");
extern u8 gMenuDescriptor[8] __asm__("D_800A3808");

extern void gActivateMenuPanel(u8 *arg0, s32 arg1) __asm__("func_80078650");
extern void gSyncMenuFontBank(void) __asm__("func_800788B4");

/**
 * @brief Updates the title/splash screen timeout and transitions to screen D_800A3808 once D_8009F8E8 expires.
 *
 * Registered as the draw/tick callback (+0x18) on screen descriptor D_800A38F8.
 * On phase 0 (arg0 == 0), advances idle deadline D_8009F870 by 0x2328 ticks and, once
 * D_8009FA2C passes D_8009F8E8, activates D_800A3808, sets D_8009F46C = 1, and refreshes the font bank.
 *
 * @param arg0 Callback phase; returns immediately unless arg0 == 0.
 *
 * @par Globals
 * D_8009FA2C - frame tick counter; D_8009F870 - extended by 0x2328;
 * D_8009F8E8 - expiry tick threshold; D_8009F46C - set to 1 on transition; D_800A3808 - target menu screen.
 * @par Control flow
 * Early return if arg0 != 0, stores D_8009F870, then branches on D_8009F8E8 < D_8009FA2C.
 * @par Calls
 * func_80078650 - activates menu D_800A3808 (arg1 = 0, replace without push);
 * func_800788B4 - switches active font bank via func_80048E28 if D_8009F468[8] changed.
 * @par Side effects
 * Updates D_8009F870, switches active screen to D_800A3808, sets D_8009F46C = 1, and updates font bank.
 * @par Callers
 * Confirmed: direct call at 0x80077978 in asm/us/main.s (same file, outside matching_units); address taken in func_80077AD4 (0x80077B34, stored at D_800A38F8+0x18). / Likely: front-end screen loop.
 */
void gUpdateTitleScreenTimeout(s32 arg0) __asm__("func_800778E4");

void gUpdateTitleScreenTimeout(s32 arg0)
{
    if (arg0 != 0) {
        return;
    }

    gIdleDeadline = gFrameTickCounter + 0x2328;
    if (gTitleScreenDeadline < gFrameTickCounter) {
        gActivateMenuPanel(gMenuDescriptor, 0);
        gTitleScreenConfirmEnabled = 1;
        gSyncMenuFontBank();
    }
}

void func_80077944(s32 arg0)
{
    if (arg0 == 3 && gTitleScreenConfirmEnabled != 0) {
        gTitleScreenDeadline = gFrameTickCounter + 1;
        gUpdateTitleScreenTimeout(0);
    }
}
