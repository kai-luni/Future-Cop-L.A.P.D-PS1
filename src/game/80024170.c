// Categories: Game Engine > Audio | Platform > Sound > Voices
extern int gTrackedSoundState __asm__("D_800A0A88");

int gIsTrackedSoundPlaying(void) __asm__("func_80024170");

/**
 * @brief Reports whether the tracked sound is in playback state 1.
 *
 * @param void No parameters.
 * @return 1 when the tracked sound state equals 1, otherwise 0.
 *
 * @par Confirmed mechanics
 * Loads the word at handle +0x08, XORs it with 1 and uses SLTIU to form the equality result.
 * The start path writes 1 and the stop path clears the same word.
 *
 * @par Globals
 * Reads gTrackedSoundState at gAssetHandle +0x08.
 *
 * @par Callers
 * Direct calls from gAcquireTrackedSoundVoice (func_80024064), gFinishScreenModule
 * (func_80025B7C), gSoundPauseVoices (func_800476D0).
 *
 * @par Role and path
 * The audio pause and screen transition paths use this predicate when deciding how to handle
 * tracked playback.
 *
 * @par Unknown
 * Other nonzero values have no established meaning here.
 */
int gIsTrackedSoundPlaying(void)
{
    return gTrackedSoundState == 1;
}
