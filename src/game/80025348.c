// Categories: Game Engine > Frame | Resources > Asset Loading
#include "game/types.h"

extern void func_8001B4A8(s32 arg0);
extern void gLoadQueuedAssets(void) __asm__("func_80024EC8");

/**
 * @brief Loading-screen frame tick: ends the frame in mode 1 and runs the
 *        tag-dispatched asset loaders.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * func_8001B4A8(1) (frame end, depth-bucket path) followed by
 * func_80024EC8() (dispatches the 20 asset loaders by tag).
 *
 * @par Callers
 * func_80025B7C (screen state machine / module teardown).
 */
void func_80025348(void)
{
    func_8001B4A8(0x1);
    gLoadQueuedAssets();
}
