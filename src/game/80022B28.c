// Categories: Game Engine > Audio
#include "game/types.h"

struct StreamChannel {
    s32 id;
    s32 voice;
    s32 left;
    s32 right;
    s32 start;
    s32 length;
    u8 looping;
    u8 unk19[0x0B];
};

struct StreamState {
    u8 unk00[0x24];
    struct StreamChannel channels[3];
};

extern struct StreamState gMixerState __asm__("D_800A0968");

void gSetAudioStreamChannelRange(s32 start, s32 length, s32 channel, u8 looping) __asm__("func_80022B28");

/**
 * @brief Stores a stream channel's start and length and optionally sets its loop flag.
 *
 * @param start Start value stored in the channel.
 * @param length Length value stored in the channel.
 * @param channel Index of the stream channel to update.
 * @param looping Nonzero to set the channel's looping flag to one.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Computes a channel stride of 0x24, writes words at mixer +0x34 and +0x38 and sets byte +0x3C
 * only when the low byte of a3 is nonzero.
 *
 * @par Globals
 * Writes channel fields in gMixerState.
 *
 * @par Callers
 * Direct calls from func_800246A8.
 *
 * @par Role and path
 * Buffer replacement uses this to supply the source range to the next transfer request.
 *
 * @par Unknown
 * The caller supplies a valid channel index and an already set loop flag is sticky.
 */
void gSetAudioStreamChannelRange(s32 start, s32 length, s32 channel, u8 looping)
{
    if (looping) {
        gMixerState.channels[channel].looping = 1;
    }
    gMixerState.channels[channel].length = length;
    gMixerState.channels[channel].start = start;
}
