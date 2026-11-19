// Categories: Platform > Streaming Audio | Platform > Sound > Voices
#include "game/types.h"

extern void gStopAudioStreamPlayback(void) __asm__("func_80022A64");
extern void gClearAudioStreamSpuBuffers(void) __asm__("func_80022C68");

void gStopAudioStreamAndClearChannels(void) __asm__("func_800226D8");

/**
 * @brief Stops the audio stream and clears the per-channel start fields.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls gStopAudioStreamPlayback followed by gClearAudioStreamSpuBuffers.
 *
 * @par Globals
 * None directly. Both callees update gMixerState.
 *
 * @par Callers
 * Direct calls from gPollAudioStreamStop (func_80022654), func_800244A4, func_80024630.
 *
 * @par Role and path
 * Used for shutdown, deferred stop and reconfiguration.
 *
 * @par Unknown
 * The individual teardown operations are performed by gStopAudioStreamPlayback.
 */
void gStopAudioStreamAndClearChannels(void)
{
    gStopAudioStreamPlayback();
    gClearAudioStreamSpuBuffers();
}
