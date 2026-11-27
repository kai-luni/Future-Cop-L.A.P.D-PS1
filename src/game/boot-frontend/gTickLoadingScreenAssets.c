// Categories: Game Engine > Frame | Resources > Asset Loading
#include "game/types.h"

extern void func_8001B4A8(s32 arg0);
extern void gLoadQueuedAssets(void) __asm__("func_80024EC8");

void gTickLoadingScreenAssets(void) __asm__("func_80025348");

/**
 * @brief Finishes a loading-screen frame and runs the tag-dispatched asset pass.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls func_8001B4A8 with a0 equal to 1, then calls gLoadQueuedAssets.
 *
 * @par Globals
 * None directly. Both calls update their respective frame and asset state.
 *
 * @par Callers
 * Direct calls from gFinishScreenModule (func_80025B7C).
 *
 * @par Role and path
 * The screen module's frame-finish path selects this loading tick.
 *
 * @par Unknown
 * The wrapper does not inspect either call's result.
 */
void gTickLoadingScreenAssets(void)
{
    func_8001B4A8(0x1);
    gLoadQueuedAssets();
}
