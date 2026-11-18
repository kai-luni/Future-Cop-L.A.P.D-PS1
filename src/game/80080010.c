// Categories: Game Engine > Front End
#include "game/types.h"

extern s32 gSelectEnabledMenuItem(s32 arg0, s32 arg1, s32 arg2) __asm__("func_800787D8");

/**
 * @brief Selects the first enabled group-0 item starting from index zero.
 *
 * Calls gSelectEnabledMenuItem (func_800787D8) with start 0, forward
 * direction 1, and group 0.
 *
 * @return The caller declares a return value, although the callee is void;
 * the original calling convention is preserved for byte matching.
 * @par Globals
 * None directly.
 * @par Control flow
 * None.
 * @par Calls
 * gSelectEnabledMenuItem (func_800787D8) - selects an enabled menu item.
 * @par Side effects
 * May update the active menu's group-0 selection.
 * @par Callers
 * Unknown: no direct edge; reached from the menu state handlers.
 */
s32 func_80080010(void)
{
    return gSelectEnabledMenuItem(0, 1, 0);
}
