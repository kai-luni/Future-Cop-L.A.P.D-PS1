// Categories: Platform > Device Shutdown | Input > Controller
#include "game/types.h"

extern u8 gFrameCallbackEnabled __asm__("D_8009FA20");

extern void gResetBiosDevices(void) __asm__("func_800200F0");
extern void gShutdownPadDriver(void) __asm__("func_8001E910");
extern void gStopFrameCallback(void) __asm__("func_8001F158");
extern void gInvokeIrqCallback(void) __asm__("func_80091574");
s32 gShutdownDeviceLayer(void) __asm__("func_8001E540");


/**
 * @brief Shuts the controller/device layer down again.
 *
 * @return 0, always.
 *
 * @par Confirmed flow
 * func_800200F0() and func_8001E910() release the two sub-devices, then - only
 * if D_8009FA20 says the per-frame driver is running - func_8001F158() stops
 * it, and func_80091574() removes the interrupt hook. The flag is not cleared
 * here.
 *
 * @par Globals
 * D_8009FA20 (running flag, read only).
 *
 * @par Callers
 * func_80076FE0 - the counterpart of func_8001E4E4.
 *
 * @par Role and path
 * Shuts down the controller and device layer by releasing sub-devices through
 * func_800200F0 and func_8001E910, stopping the per-frame callback via
 * func_8001F158 if D_8009FA20 is non-zero, and removing the interrupt hook
 * via func_80091574. Reads gFrameCallbackEnabled (D_8009FA20) without
 * clearing it; called by func_80076FE0 as the counterpart to func_8001E4E4.
 */
s32 gShutdownDeviceLayer(void)
{
    gResetBiosDevices();
    gShutdownPadDriver();
    if (gFrameCallbackEnabled != 0) {
        gStopFrameCallback();
    }
    gInvokeIrqCallback();
    return 0;
}
