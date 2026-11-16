// Categories: Gameplay > Entity Script | Game Engine > State Machine
#include "game/types.h"

extern u8 D_8009F840;

/**
 * @brief Inhibits bytecode script execution by setting the global script-VM disable flag D_8009F840.
 *
 * Confirmed: stores 1 into D_8009F840. Reading func_80069D68 confirms that a non-zero D_8009F840 causes the script VM to return 0 immediately without executing opcodes; func_80069B58 later clears D_8009F840 = 0 on mission init. Category Gameplay > Entity Script | Game Engine > State Machine reflects D_8009F840's role as the VM gate and its reset/teardown callers.
 * Likely: disables entity scripts during mission teardown/reset until func_80069B58 finishes reinitializing VM banks.
 * Unknown: none.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F840 - bytecode script-VM inhibit byte (set to 1).
 *
 * @par Control flow
 * Single byte store and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Sets D_8009F840 = 1.
 *
 * @par Callers
 * Confirmed: func_800249BC (0x800249D4, subsystem teardown) and func_80024D14 (0x80024D60, global subsystem reset) / Likely: none / Unknown: none.
 */
void func_80069D58(void)
{
    D_8009F840 = 1;
}
