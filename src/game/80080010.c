// Categories: Game Engine > Front End
#include "game/types.h"

extern s32 func_800787D8(s32 arg0, s32 arg1, s32 arg2);

/**
 * @brief Applies front-end screen mode 0 with the sandbox override.
 *
 * Calls the screen-mode setter func_800787D8 with the fixed triple (0, 1, 0):
 * mode 0, override enabled, no extra argument. Used when a screen returns to
 * its default presentation.
 *
 * @return The setter's return value.
 * @par Globals
 * None directly.
 * @par Control flow
 * None.
 * @par Calls
 * func_800787D8 - screen mode setter.
 * @par Side effects
 * Changes the front-end presentation mode.
 * @par Callers
 * Unknown: no direct edge; reached from the menu state handlers.
 */
s32 func_80080010(void)
{
    return func_800787D8(0, 1, 0);
}
