// Categories: Platform > Streaming Audio | Platform > Sound > Voices
#include "game/types.h"

extern void func_80022A64(void);
extern void func_80022C68(void);

void gStopAudioStreamAndClearChannels(void) __asm__("func_800226D8");

/**
 * @brief Stops the audio stream and clears the per-channel start fields.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Calls func_80022A64 followed by func_80022C68.
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
 * The individual teardown operations are performed by func_80022A64.
 */
void gStopAudioStreamAndClearChannels(void)
{
    func_80022A64();
    func_80022C68();
}
