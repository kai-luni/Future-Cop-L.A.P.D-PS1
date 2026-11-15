// Categories: Resources > Movie Playback
#include "game/types.h"

extern s32 D_8009F464;

/**
 * @brief Reports whether the movie player is still running.
 *
 * @return 0 while D_8009F464 == 2 (running, set by func_8007F8BC), 1 as soon as
 *         the state has changed - the boot loop keeps calling func_80080B08()
 *         while this returns 0.
 *
 * @par Callers
 * func_80076A2C, in the condition of the intro wait loop.
 */
s32 func_8007F8CC(void)
{
    return D_8009F464 != 2;
}
