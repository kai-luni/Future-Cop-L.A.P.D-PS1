// Categories: Input > Controller Init | Platform > Device Init
#include "game/types.h"

extern s32 gSavedStackPointer __asm__("D_8009FAD8");
extern u8 gFrameCallbackEnabled __asm__("D_8009FA20");

extern void gResetIrqDispatch(void) __asm__("func_80091480");
extern void func_80095878(void);
extern void gInitMemoryCardBios(void) __asm__("func_800200A8");
extern void gInitPadActuators(void) __asm__("func_8001E87C");
extern void gInitPadMapping(u32 port) __asm__("func_8001E588");

s32 gInitPadAndCardServices(void) __asm__("func_8001E4E4");

/**
 * @brief Initializes controller input and memory-card services during boot.
 * @return Always zero.
 *
 * @par Confirmed mechanics
 * Calls the interrupt reset, platform setup, and card BIOS initialization in
 * sequence. Clears gSavedStackPointer, initializes pad actuators, clears
 * gFrameCallbackEnabled, and initializes mappings for ports 0 and 1.
 *
 * @par Globals
 * Clears gSavedStackPointer and gFrameCallbackEnabled.
 *
 * @par Callers
 * Direct calls in the retail listing are from gInitRuntime.
 *
 * @par Role and path
 * System boot calls this before enabling per-frame device processing.
 *
 * @par Unknown
 * The implementation of the platform helper at func_80095878 determines its
 * additional effects.
 */
s32 gInitPadAndCardServices(void)
{
    u32 port;

    gResetIrqDispatch();
    func_80095878();
    gInitMemoryCardBios();
    gSavedStackPointer = 0;
    gInitPadActuators();
    gFrameCallbackEnabled = 0;
    for (port = 0; port < 2; port++) {
        gInitPadMapping(port);
    }
    return 0;
}
