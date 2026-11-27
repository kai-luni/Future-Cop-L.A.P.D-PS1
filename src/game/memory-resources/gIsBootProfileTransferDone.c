// Categories: Resources > Save Data
#include "game/types.h"

extern s32 gStillRunningState __asm__("D_8009F464");

/**
 * @brief Reports whether the boot profile transfer has finished.
 *
 * @return 0 while D_8009F464 == 2 (set by func_8007F8BC), 1 once the state has
 *         changed - the boot loop keeps calling func_80080B08() while this
 *         returns 0.
 *
 * @par Callers
 * func_80076A2C, in the condition of the boot wait loop.
 */
s32 gIsBootProfileTransferDone(void) __asm__("func_8007F8CC");

s32 gIsBootProfileTransferDone(void)
{
    return gStillRunningState != 2;
}
