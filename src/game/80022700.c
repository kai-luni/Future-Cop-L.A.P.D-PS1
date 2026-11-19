// Categories: Game Engine > State
#include "../../include/game/types.h"

extern s32 gStreamStopPending __asm__("D_800A0974");

void gMarkAudioStreamStopPending(void) __asm__("func_80022700");

/**
 * @brief Marks an audio stream stop for deferred processing.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Stores 1 to the word at mixer +0x0C. The stream poller tests that word before dispatching the
 * stop-and-clear routine.
 *
 * @par Globals
 * Writes gStreamStopPending, which aliases gMixerState +0x0C.
 *
 * @par Callers
 * Direct calls from func_80024630.
 *
 * @par Role and path
 * The nonzero variant of stream shutdown sets this marker for the polling path.
 *
 * @par Unknown
 * The poller also needs the active field to be nonzero.
 */
void gMarkAudioStreamStopPending(void)
{
    gStreamStopPending = 1;
}
