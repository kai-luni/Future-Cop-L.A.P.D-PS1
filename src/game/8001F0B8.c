// Categories: Game Engine > Timing | Platform > Interrupts
#include "game/types.h"

extern s32 gGameClock __asm__("D_8009FA60");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern s32 gDisplaySetupToggle __asm__("D_8009F9CC");
extern s32 gScreenModeHook __asm__("D_8009F99C");
extern u8 gFrameCallbackEnabled __asm__("D_8009FA20");
extern u8 D_8001EFF8[];

extern void gSetVSyncCallback(void *arg0) __asm__("func_80091510");
void gStartFrameCallback(void) __asm__("func_8001F0B8");


/**
 * @brief Starts the per-frame callback driver.
 *
 * @par Confirmed flow
 * The four counters D_8009FA60 (frame counter), D_8009FA2C (time base),
 * D_8009F9CC and D_8009F99C are cleared, func_80091510(D_8001EFF8) installs the
 * callback table that lives at D_8001EFF8, and D_8009FA20 = 1 marks the driver
 * as running (func_8001E540 uses that flag to decide whether to stop it).
 *
 * @par Globals
 * D_8009FA60, D_8009FA2C, D_8009F9CC, D_8009F99C (cleared), D_8009FA20 (running
 * flag), D_8001EFF8 (callback table).
 *
 * @par Likely role
 * func_8009xxxx is the same interrupt/vsync family that the movie player and
 * the frame barrier use, so this is the "start frame timing" call. D_8009FA2C is
 * exactly the counter the boot sequence waits on (D_8009FA2C + 0x5DC), which
 * confirms that it is the tick source.
 *
 * @par Callers
 * func_80076EB0, right before func_8002333C().
 *
 * @par Role and path
 * Starts the per-frame callback driver by zeroing gGameClock,
 * gFrameTickCounter, gDisplaySetupToggle, and gScreenModeHook, registering
 * D_8001EFF8 (func_8001EFF8) via func_80091510, and setting
 * gFrameCallbackEnabled (D_8009FA20) to 1. Writes D_8009FA60, D_8009FA2C,
 * D_8009F9CC, D_8009F99C, and D_8009FA20; called by func_80076EB0.
 */
void gStartFrameCallback(void)
{
    register u8 *callback_table __asm__("$4");
    callback_table = D_8001EFF8;
    gGameClock = 0;
    gFrameTickCounter = 0;
    gDisplaySetupToggle = 0;
    gScreenModeHook = 0;
    gSetVSyncCallback(callback_table);
    gFrameCallbackEnabled = 1;
}
