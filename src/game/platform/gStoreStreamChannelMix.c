// Categories: Platform > Audio
#include "game/types.h"

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
    u8 unk08[5];
    s8 voice;
};

struct StreamChannel {
    s32 id;
    struct SoundVoice *voice;
    s32 left;
    s32 right;
    u8 unk10[0x14];
};

struct StreamState {
    u8 unk00[4];
    s32 active;
    u8 unk08[0x1C];
    struct StreamChannel channels[3];
};

extern struct StreamState gMixerState __asm__("D_800A0968");

void gStoreStreamChannelMix(s32 channel, s32 left, s32 right) __asm__("func_800227BC");

/**
 * @brief Stores a clamped level and pan for one stream channel.
 *
 * @param channel Requested channel index, clamped to 0 through 2.
 * @param left Requested left level.
 * @param right Requested right level.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clamps the first and second mix values independently to 0 through 0x7F and the channel to 0
 * through 2, then writes at mixer offsets +0x2C and +0x30 with a 0x24-byte stride.
 *
 * @par Globals
 * Updates the selected record in gMixerState.
 *
 * @par Callers
 * Direct calls from gApplyStreamChannelMix (func_8002283C), gInitializeVagmAudioStream
 * (func_800244A4).
 *
 * @par Role and path
 * Startup stores these values before voice allocation. Live mix changes call the same setter
 * before updating the active voice.
 *
 * @par Unknown
 * The C parameter names left and right are local. The setup and mix callers use the first value
 * for level and the second for pan.
 */
void gStoreStreamChannelMix(s32 channel, s32 left, s32 right)
{
    s32 clampedLeft;
    s32 clampedRight;
    s32 clampedChannel;

    if (left >= 0) {
        if (left < 0x80) {
            clampedLeft = left;
        } else {
            clampedLeft = 0x7F;
        }
    } else {
        clampedLeft = 0;
    }
    left = clampedLeft;

    if (right >= 0) {
        if (right < 0x80) {
            clampedRight = right;
        } else {
            clampedRight = 0x7F;
        }
    } else {
        clampedRight = 0;
    }
    right = clampedRight;

    if (channel >= 0) {
        if (channel < 3) {
            clampedChannel = channel;
        } else {
            clampedChannel = 2;
        }
    } else {
        clampedChannel = 0;
    }
    channel = clampedChannel;

    gMixerState.channels[channel].left = left;
    gMixerState.channels[channel].right = right;
}
