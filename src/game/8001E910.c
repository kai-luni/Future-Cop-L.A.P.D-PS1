// Categories: Platform > Device Shutdown | Platform > Interrupts
#include "game/types.h"

extern void func_80089E20(void);
void gShutdownPadDriver(void) __asm__("func_8001E910");


/**
 * @brief Releases the device/controller BIOS interrupt hook during shutdown.
 *
 * Thin wrapper that invokes func_80089E20, which chains to func_8008A688 to disable BIOS vector 0xC0:0x0A slot 3 and unlink D_800A3B60 from the priority-2 IRQ chain.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; delegates to func_8008A688 which unlinks D_800A3B60.
 *
 * @par Control flow
 * Single tail-like call to func_80089E20 and return.
 *
 * @par Calls
 * func_80089E20 - forwards to func_8008A688 to tear down the BIOS IRQ hook.
 *
 * @par Side effects
 * Tears down the BIOS priority-2 interrupt hook via func_80089E20 / func_8008A688.
 *
 * @par Callers
 * Confirmed: func_8001E540 (0x8001E550, controller/device layer shutdown) / Likely: none / Unknown: none.
 *
 * @par Role and path
 * Releases the controller driver interrupt hook by calling func_80089E20.
 * Touches no globals directly and is called by func_8001E540 during
 * device-layer shutdown.
 */
void gShutdownPadDriver(void)
{
    func_80089E20();
}
