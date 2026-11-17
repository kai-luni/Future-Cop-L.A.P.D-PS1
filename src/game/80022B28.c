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

/**
 *  @brief Sets the start and length of a stream channel and optionally enables looping.
 *
 *  Updates the selected entry in the shared stream-channel state without
 *  checking the channel index or clearing an existing looping flag.
 *  @param start Start value stored in the channel.
 *  @param length Length value stored in the channel.
 *  @param channel Index of the stream channel to update.
 *  @param looping Nonzero to set the channel's looping flag to one.
 *  @par Globals
 *  D_800A0968 contains the channel array being modified.
 *  @par Control flow
 *  Sets looping only when requested, then stores length and start.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Modifies the selected channel's start, length, and possibly looping flag.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80022B28(s32 start, s32 length, s32 channel, u8 looping)
{
    if (looping) {
        gMixerState.channels[channel].looping = 1;
    }
    gMixerState.channels[channel].length = length;
    gMixerState.channels[channel].start = start;
}
