// Categories: Game Engine > Frame | Graphics > Display
#include "game/types.h"

extern void gLoadQueuedAssets(void) __asm__("func_80024EC8");
extern void func_8001B4A8(s32 arg0);

void gPresentInteractiveScreenFrame(void) __asm__("func_80027EBC");

/**
 * @brief Runs asset dispatch and presents the interactive screen frame.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls gLoadQueuedAssets, then calls func_8001B4A8 with mode 1.
 *
 * @par Globals
 * None directly. The two callees own asset and draw state.
 *
 * @par Callers
 * Direct calls from gRunInteractiveScreenLoop (func_80027EE4).
 *
 * @par Role and path
 * The interactive screen frame loop uses this order before its next iteration.
 *
 * @par Unknown
 * The mode-1 presentation details are handled by func_8001B4A8.
 */
void gPresentInteractiveScreenFrame(void)
{
    (void)gLoadQueuedAssets();
    func_8001B4A8((s32)0x1);
}
