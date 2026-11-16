// Categories: Input > Controller Init | Platform > Interrupts
#include "game/types.h"

extern void func_8008A5BC(void);

/**
 * @brief Starts the controller/pad interrupt driver by delegating to func_8008A5BC.
 *
 * Thin wrapper invoking func_8008A5BC, which installs handler packet D_800A3B60 on BIOS IRQ chain 2, unmasks VSync IRQ0 in I_STAT/I_MASK, configures root counter 3, resets both controller ports, and enables D_8009C760.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; func_8008A5BC initializes D_8009C760, D_800A3B60, D_800A3B70, and I_STAT/I_MASK.
 *
 * @par Control flow
 * Single call to func_8008A5BC and return.
 *
 * @par Calls
 * func_8008A5BC - installs pad VSync IRQ handler and resets controller ports.
 *
 * @par Side effects
 * Arms BIOS chain-2 IRQ handler, VSync IRQ0, and controller port polling via func_8008A5BC.
 *
 * @par Callers
 * Confirmed: func_8001E87C (0x8001E8F8, controller slot/device hook initialization) / Likely: none / Unknown: none.
 */
void func_80089E00(void)
{
    func_8008A5BC();
}
