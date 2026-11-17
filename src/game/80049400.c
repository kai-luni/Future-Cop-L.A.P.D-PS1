// Categories: Game Engine > Init | Gameplay > Entity Script
#include "game/types.h"

struct ScriptTimer;

extern struct ScriptTimer *gScriptTimerTicks __asm__("D_8009F770");
extern s32 D_8009F774;

/**
 * @brief Clears the ScriptTimer array pointer D_8009F770 and count D_8009F774 during mission boot.
 *
 * Resets D_8009F770 and D_8009F774 to 0 in func_80024A24 before level loader post-processing in func_80049420 publishes the mission timer table and func_80049564 ticks active timers. Category Game Engine > Init | Gameplay > Entity Script reflects boot initialization and script timer globals.
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
 * Confirmed: func_80024A24 (0x80024C34 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_80049400(void)
{
    gScriptTimerTicks = 0;
    D_8009F774 = 0;
}
