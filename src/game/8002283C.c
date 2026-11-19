// Categories: Game Engine > Audio
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

struct StreamMix {
    s32 unk00;
    s32 volume;
    s32 pan;
};

extern struct StreamState gMixerState __asm__("D_800A0968");

extern void gStoreStreamChannelMix(s32 channel, s32 left, s32 right) __asm__("func_800227BC");
extern void func_800477D8(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);
extern void gSetSoundChannelMix(s32 voice, struct StreamMix *mix) __asm__("func_800222CC");

void gApplyStreamChannelMix(s32 channel, s32 left, s32 right) __asm__("func_8002283C");

/**
 * @brief Stores a stream channel mix and applies it to an active voice when present.
 *
 * @param channel Channel index, clamped to 0 through 2.
 * @param left Requested left channel setting.
 * @param right Requested right channel setting.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clamps the channel, calls gStoreStreamChannelMix, checks mixer +0x04 and reads the selected
 * voice pointer. Calls func_800477D8 with the stored pan and level, then uses signed bytes at
 * voice offsets +4, +6 and +0x0D to submit an SPU mix with pitch argument -1.
 *
 * @par Globals
 * Reads active state, the channel mix and the voice pointer in gMixerState.
 *
 * @par Callers
 * Direct calls from func_80024730.
 *
 * @par Role and path
 * Used when the stream volume or stereo/mono selection changes during playback.
 *
 * @par Unknown
 * The voice descriptor's additional fields are not accessed here.
 */
void gApplyStreamChannelMix(s32 channel, s32 left, s32 right)
{
    struct SoundVoice *voice;
    struct StreamMix mix;
    s32 clamped;

    if (channel >= 0) {
        if (channel < 3) {
            clamped = channel;
        } else {
            clamped = 2;
        }
    } else {
        clamped = 0;
    }
    channel = clamped;
    gStoreStreamChannelMix(channel, left, right);
    if (gMixerState.active == 0) {
        return;
    }
    voice = gMixerState.channels[channel].voice;
    if (voice == 0) {
        return;
    }
    func_800477D8(voice, gMixerState.channels[channel].right, gMixerState.channels[channel].left, -1);
    mix.volume = voice->volume;
    mix.pan = voice->pan;
    mix.unk00 = -1;
    gSetSoundChannelMix(voice->voice, &mix);
}
