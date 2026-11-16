// Categories: Platform > Device Shutdown | Platform > Interrupts
#include "game/types.h"

extern void func_8008A688(void);

/**
 * @brief Stops the controller/pad interrupt driver by delegating to func_8008A688.
 *
 * Thin wrapper invoking func_8008A688, which enters a critical section, calls BIOS vector 0xC0:0x0A with (3, 1) to disable IRQ3 clear control, and dequeues D_800A3B60 from BIOS priority-2 IRQ chain.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; func_8008A688 unlinks D_800A3B60 from the BIOS IRQ chain.
 *
 * @par Control flow
 * Single call to func_8008A688 and return.
 *
 * @par Calls
 * func_8008A688 - tears down BIOS root-counter control and unlinks D_800A3B60.
 *
 * @par Side effects
 * Modifies BIOS vector 0xC0:0x0A slot 3 and unlinks D_800A3B60 via func_8008A688.
 *
 * @par Callers
 * Confirmed: func_8001E910 (0x8001E918, called by func_8001E540 device shutdown) / Likely: none / Unknown: none.
 */
void func_80089E20(void)
{
    func_8008A688();
}
