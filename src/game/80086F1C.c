// Categories: Resources > Movie Playback | Platform > Sound
#include "game/types.h"

typedef struct {
    u8 pad_00[0x7B20];
    volatile u32 value;
} D_8009F4D8_Data;

extern D_8009F4D8_Data * const gMovieAudioState __asm__("D_8009F4D8");

/**
 * @brief Enables audio-sync timestamp updates in gMovieAudioState (paired with gMovieAudioClockDisable).
 *
 * Sets gMovieAudioState->value (+0x7B20) to 1 so the SPU IRQ callback func_80086D74 advances the audio clock at +0x7B24 and counter at +0x7B28. This 40-byte unit also contains gMovieAudioClockDisable at 0x80086F30, which clears +0x7B20 to 0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * gMovieAudioState - pointer to the 0x7B80-byte streaming audio/video state block; writes 1 to +0x7B20.
 *
 * @par Control flow
 * Straight-line store of 1 to gMovieAudioState->value (+0x7B20) and return.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes 1 to gMovieAudioState + 0x7B20 (enabling clock increments in func_80086D74).
 *
 * @par Callers
 * Confirmed: gMovieStreamDemux (0x80087C9C, enables audio clock before video frame decode) / Likely: none / Unknown: none.
 */
void gMovieAudioClockEnable(void) __asm__("func_80086F1C");

void gMovieAudioClockEnable(void)
{
    gMovieAudioState->value = 1;
}

void gMovieAudioClockDisable(void) __asm__("func_80086F30");

void gMovieAudioClockDisable(void)
{
    gMovieAudioState->value = 0;
}
