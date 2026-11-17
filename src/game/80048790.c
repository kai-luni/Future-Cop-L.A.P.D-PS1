// Categories: Graphics > HUD | Game Engine > State Machine
#include "game/types.h"

extern s32 D_8009FAB8;
extern s32 gOverlayTimer __asm__("D_8009F760");

/**
 * @brief Reports whether the HUD overlay may be drawn right now.
 *
 * The layout mode D_8009FAB8 decides: mode 0 never allows drawing, mode 2 only
 * while the overlay timer D_8009F760 has run out. Every other mode returns 0,
 * so a half-finished overlay transition never draws.
 *
 * @return 1 when the caller may draw the overlay, 0 otherwise.
 *
 * @par Globals
 * D_8009FAB8 - HUD layout mode; D_8009F760 - overlay timer.
 *
 * @par Control flow
 * Switch over the mode with one timer test.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Unknown: no direct edge; companion of the layout update func_800487CC.
 */
u8 func_80048790(void)
{
    switch (D_8009FAB8) {
    case 0:
        return 0;
    case 2:
        if (gOverlayTimer == 0) {
            __asm__ volatile ("");
            return 1;
        }
        return 0;
    default:
        return 0;
    }
}
