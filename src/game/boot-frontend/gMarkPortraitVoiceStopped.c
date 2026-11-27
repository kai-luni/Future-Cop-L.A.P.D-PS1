// Categories: Game Engine > State
#include "game/types.h"

extern u8 gBitFieldState __asm__("D_8009F6B8");

void gMarkPortraitVoiceStopped(void) __asm__("func_80029720");

/**
 * @brief Clears the tracked portrait voice flag and marks completion when the animation flag is
 * clear.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clears bit 0x10 in the stream flags. When the original value lacks 0x20, sets bit 0x08 as
 * well.
 *
 * @par Globals
 * Reads and updates gBitFieldState.
 *
 * @par Callers
 * Direct calls from gStopTrackedSoundVoice (func_80023E84).
 *
 * @par Role and path
 * The tracked voice teardown uses this to complete playback once neither leg remains active.
 *
 * @par Unknown
 * The meanings of the other flag bits come from the parser and portrait step.
 */
void gMarkPortraitVoiceStopped(void)
{
    u8 state = gBitFieldState;

    gBitFieldState = state & 0xEF;
    if ((state & 0x20) == 0) {
        gBitFieldState = (state & 0xEF) | 0x08;
    }
}
