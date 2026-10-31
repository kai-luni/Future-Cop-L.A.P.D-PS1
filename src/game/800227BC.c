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

extern struct StreamState D_800A0968;

void func_800227BC(s32 channel, s32 left, s32 right)
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

    D_800A0968.channels[channel].left = left;
    D_800A0968.channels[channel].right = right;
}
