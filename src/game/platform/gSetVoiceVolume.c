// Categories: Platform > Sound > Volume
#include "game/types.h"

extern s32 gVoiceVolumeBase __asm__("D_8009F338");
void gSetVoiceVolume(s32 volume) __asm__("func_80045294");


/**
 * @brief Sets the voice/stream volume, clamped to the 0..0x7F range.
 *
 * Same clamp as the other two volume setters but a third slot: D_8009F338 is what func_800452C8 returns and what the stream buffer setup and the movie paths read, so it scales CD audio and streamed speech rather than SPU effects.
 *
 * @param volume Requested voice/stream volume.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F338 - voice/stream volume word, written with the clamped value.
 *
 * @par Control flow
 * Two comparisons select among three stores; no loops and no calls.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Only D_8009F338 changes; running streams keep their current level until the next stream setup reads the word.
 *
 * @par Callers
 * Confirmed: func_80044F8C (0x80044FB4) at sound init with 0x40, func_800260B8 (0x8002650C) and func_80080984 (0x800809BC) from the options screen, func_8007CA34 (0x8007CA5C), func_8007CACC (0x8007CC64).
 */
void gSetVoiceVolume(s32 volume)
{
    if (volume >= 0) {
        if (volume < 0x80) {
            gVoiceVolumeBase = volume;
        } else {
            gVoiceVolumeBase = 0x7F;
        }
    } else {
        gVoiceVolumeBase = 0;
    }
}
