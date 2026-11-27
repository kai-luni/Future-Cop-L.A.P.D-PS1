// Categories: Resources > Movie Playback | Platform > Sound
#include "game/types.h"

typedef struct {
    u8 pad_0[0x7B24];
    u8 *field_7B24;
} Unk8009F4D8;

extern Unk8009F4D8 *gMovieAudioState __asm__("D_8009F4D8");

/**
 * @brief Returns the audio playback timestamp at offset +0x7B24 of gMovieAudioState.
 *
 * Reads gMovieAudioState->field_7B24, which func_80086AC0 updates from audio chunk timestamps via func_800867B0 and gMovieStreamDemux polls to pace movie frames; typed u8 * in C despite holding an s32/u32 clock value.
 *
 * @param void No parameters.
 * @return Value of gMovieAudioState->field_7B24 in $v0 (used as u32 by callers).
 *
 * @par Globals
 * gMovieAudioState - pointer to the 0x7B80-byte streaming audio/video state block; reads +0x7B24.
 *
 * @par Control flow
 * Single pointer dereference and return; no branches or loops.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: gMovieStreamDemux (0x80087CC0, frame-sync wait loop) / Likely: none / Unknown: none.
 */
u8 *gMovieAudioClockRead(void) __asm__("func_80086F44");

u8 *gMovieAudioClockRead(void)
{
    return gMovieAudioState->field_7B24;
}
