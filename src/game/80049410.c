// Categories: Game Engine > Game State > Reset | Gameplay > Entity Script
#include "game/types.h"

struct ScriptTimer;

extern struct ScriptTimer *gScriptTimerTicks __asm__("D_8009F770");
extern s32 gScriptTimerCount __asm__("D_8009F774");

/**
 * @brief Clears the ScriptTimer array pointer D_8009F770 and count D_8009F774 during mission reset.
 *
 * Resets D_8009F770 and D_8009F774 to 0 in the between-missions reset func_80024D14, mirroring the boot reset func_80049400 before the next mission's timer table is installed. Category Game Engine > Game State > Reset | Gameplay > Entity Script follows the reset caller and script timer globals.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F770 - pointer to active ScriptTimer array, cleared to 0; D_8009F774 - active timer count, cleared to 0.
 *
 * @par Control flow
 * Straight-line stores of 0 to both globals and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes D_8009F770 = 0 and D_8009F774 = 0.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024D68 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void gResetScriptTimers(void) __asm__("func_80049410");
void gResetScriptTimers(void)
{
    gScriptTimerTicks = 0;
    gScriptTimerCount = 0;
}
