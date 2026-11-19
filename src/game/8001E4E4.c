// Categories: Input > Controller Init | Platform > Device Init
#include "game/types.h"

extern s32 gSavedStackPointer __asm__("D_8009FAD8");
extern u8 gFrameCallbackEnabled __asm__("D_8009FA20");

extern void gResetIrqDispatch(void) __asm__("func_80091480");
extern void func_80095878(void);
extern void gInitMemoryCardBios(void) __asm__("func_800200A8");
extern void gInitPadActuators(void) __asm__("func_8001E87C");
extern void gInitPadMapping(u32 port) __asm__("func_8001E588");

/**
 * @brief Initialises the controller/device layer for both ports.
 *
 * @return 0, always - the caller's error check never triggers.
 *
 * @par Confirmed flow
 * func_80091480(), func_80095878() and gInitMemoryCardBios() reset the three
 * sub-devices, D_8009FAD8 = 0, func_8001E87C() installs the device hooks,
 * D_8009FA20 = 0 marks the layer as "not running yet", and func_8001E588(port)
 * is called for port 0 and port 1 (the two controller ports).
 *
 * @par Globals
 * D_8009FAD8 (device state), D_8009FA20 (per-frame-callback enabled flag, set
 * later by func_8001F0B8).
 *
 * @par Likely role
 * The pad/device bring-up of the boot sequence; func_8001E540() is its
 * teardown counterpart.
 *
 * @par Callers
 * func_80076EB0, once, and only after func_8008DEA0() succeeded.
 */
s32 func_8001E4E4(void)
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
