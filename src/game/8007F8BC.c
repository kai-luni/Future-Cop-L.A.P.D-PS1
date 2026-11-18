// Categories: Resources > Save Data | Game Engine > Timing
#include "game/types.h"

extern s32 gStillRunningState __asm__("D_8009F464");

/**
 * @brief Marks the boot profile transfer as in progress.
 *
 * Writes D_8009F464 = 2. func_8007F8CC() reports "done" as soon as the state is
 * no longer 2, and func_80080B08() sets 3 when the transfer ends - so 2 and 3 are
 * the two states the boot wait distinguishes. The transfer is the memory-card
 * profile read; the name has nothing to do with the movie player.
 *
 * @return Nothing. @par Callers func_80076A2C (boot), before the wait loop that
 * also pumps the intro movie.
 */
void gBeginBootProfileTransfer(void) __asm__("func_8007F8BC");

void gBeginBootProfileTransfer(void)
{
    gStillRunningState = 2;
}
