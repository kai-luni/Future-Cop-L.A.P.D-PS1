// Categories: Game Engine > State
#include "../../include/game/types.h"

extern u8 gMixerState __asm__("D_800A0968");

u32 gIsAudioStreamConfigured(void) __asm__("func_800226C8");

/**
 * @brief Returns the audio stream controller's configuration byte.
 *
 * @param void No parameters.
 * @return The byte value, widened to u32.
 *
 * @par Confirmed mechanics
 * Uses LBU to return the leading mixer byte in v0. The configuration routine sets it to 1 after
 * initializing the controller.
 *
 * @par Globals
 * Reads the first byte of gMixerState.
 *
 * @par Callers
 * Direct calls from gInitializeVagmAudioStream (func_800244A4).
 *
 * @par Role and path
 * Audio stream startup checks this flag before stopping a previous configuration.
 *
 * @par Unknown
 * The accessor returns the raw byte.
 */
u32 gIsAudioStreamConfigured(void)
{
    return gMixerState;
}
