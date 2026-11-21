// Categories: Game Engine > Frame | Platform > GPU | Graphics > Display Setup
#include "game/types.h"

extern  void (*gScreenModeHook)(void) __asm__("D_8009F99C");
extern s32 gDisplaySetupToggle __asm__("D_8009F9CC");
extern s32 gScreenState __asm__("D_8009FA00");
extern s32 gGameClock __asm__("D_8009FA60");
extern volatile s32 gFrameTickCounter __asm__("D_8009FA2C");
extern volatile u8 gFrameBarrier __asm__("D_8009FA3C");

extern s32 gDrawSync(s32 arg0) __asm__("func_80092C8C");
extern void func_8001BFE4(void);
extern void gFlipDisplayLayer(void) __asm__("func_8001BFA8");
void gFrameCallback(void) __asm__("func_8001EFF8");


/**
 * @brief Per-frame tick installed as a callback: runs the chained hook, advances
 *        the palette/timer counters and finishes a pending display transfer.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * Calls the function pointer D_8009F99C when non-null. In screen modes 4 and
 * 13 (D_8009FA00) it adds 5 to D_8009FA60, the palette-cycle counter the HUD
 * reads as (D_8009FA60 >> 4) & 7; D_8009FA2C always gains 5. When the pending
 * flag D_8009FA3C is set, func_80092C8C(1) polls the GPU; on completion the
 * flag is cleared and func_8001BFE4 plus func_8001BFA8 run (fade-layer and
 * display-setup finish). Finally D_8009F9CC toggles between 0 and 1.
 *
 * @par Likely role
 * The vertical-blank/frame callback of the render driver: the +5 steps are
 * palette animation in the pause screens and a frame timer, the toggle is
 * the double-buffer page flag.
 *
 * @par Unknown
 * What the chained hook D_8009F99C points at in each screen mode.
 *
 * @par Callers
 * No direct callers in the call graph. func_8001F0B8 installs the callback
 * table at D_8001EFF8 through func_80091510, so the interrupt path reaches
 * it indirectly.
 *
 * @par Role and path
 * Per-frame callback installed via func_80091510 by func_8001F0B8: invokes
 * gScreenModeHook if non-null, increments gGameClock by 5 in screen states 4
 * and 13, always increments gFrameTickCounter by 5, polls func_80092C8C(1)
 * when gFrameBarrier is set to clear the barrier and run func_8001BFE4 and
 * func_8001BFA8, and toggles gDisplaySetupToggle. Touches D_8009F99C,
 * D_8009F9CC, D_8009FA00, D_8009FA2C, D_8009FA3C, and D_8009FA60.
 */
void gFrameCallback(void)
{
    if (gScreenModeHook != 0) {
        gScreenModeHook();
    }
    if (gScreenState == 4 || gScreenState == 13) {
        gGameClock += 5;
    }
    gFrameTickCounter += 5;
    if (gFrameBarrier != 0) {
        if (gDrawSync(1) == 0) {
            gFrameBarrier = 0;
            func_8001BFE4();
            gFlipDisplayLayer();
        }
    }
    gDisplaySetupToggle = 1 - gDisplaySetupToggle;
}
