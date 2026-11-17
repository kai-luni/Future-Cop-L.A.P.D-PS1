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

extern void func_800227BC(s32 channel, s32 left, s32 right);
extern void func_800477D8(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);
extern void func_800222CC(s32 voice, struct StreamMix *mix);

/**
 *  @brief Updates a stream channel's settings and applies its active voice mix.
 *
 *  Clamps the channel to the three-channel range, records the requested
 *  values, and updates the associated voice when streaming is active.
 *  @param channel Channel index, clamped to 0 through 2.
 *  @param left Requested left channel setting.
 *  @param right Requested right channel setting.
 *  @par Globals
 *  D_800A0968 supplies the active flag and channel voice and settings.
 *  @par Control flow
 *  Returns after recording settings if streaming is inactive or the voice is null.
 *  @par Calls
 *  func_800227BC records settings; func_800477D8 updates the voice.
 *  func_800222CC applies the resulting volume and pan to the voice index.
 *  @par Side effects
 *  Updates channel settings and, when applicable, voice and audio mix state.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_8002283C(s32 channel, s32 left, s32 right)
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
    func_800227BC(channel, left, right);
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
    func_800222CC(voice->voice, &mix);
}
