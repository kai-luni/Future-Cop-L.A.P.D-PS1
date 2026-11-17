// Categories: Resources > Movie Playback | Game Engine > Timing
#include "game/types.h"

extern s32 gStillRunningState __asm__("D_8009F464");

/**
 * @brief Marks the movie player as "running" so that the wait loop keeps
 *        pumping it.
 *
 * Writes D_8009F464 = 2. func_8007F8CC() reports "still running" exactly as
 * long as this state is 2, and func_80080B08() sets 3 once the player reaches
 * a terminal state - so 2 and 3 are the two states the boot wait distinguishes.
 *
 * @return Nothing. @par Callers func_80076A2C, right before the wait that
 * plays "Intro.wve".
 */
void func_8007F8BC(void)
{
    gStillRunningState = 2;
}
