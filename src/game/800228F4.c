// Categories: Game Engine > Audio Streaming
#include "game/types.h"

typedef void (*SpuIrqCallback)(void);

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
    u8 unk08[5];
    s8 unk0D;
};

struct StreamChannel {
    s32 id;
    s32 voice;
    s32 unk08;
    s32 unk0C;
    u8 unk10[0x10];
    s32 unk20;
};

struct StreamState {
    u8 unk00[4];
    s32 active;
    s32 unk08;
    u8 unk0C[8];
    u32 channelCount;
    s32 unk18;
    u8 unk1C[8];
    struct StreamChannel channels[2];
    s32 unk6C;
    u8 unk70[0x0C];
    s32 unk7C;
    u8 unk80[8];
    s32 unk88;
    s32 unk8C;
};

extern struct StreamState gMixerState __asm__("D_800A0968");
extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern void gHandleAudioStreamSpuIrq(void) __asm__("D_80022FA8");

extern SpuIrqCallback gSpuInstallIrqCallback(SpuIrqCallback callback) __asm__("func_80098BF8");
extern void gArmAudioStreamSpuIrq(void) __asm__("func_80022F44");
extern struct SoundVoice *func_80046B04(s32 sound, s32 arg1, s32 arg2, s32 arg3);
extern void func_800477D8(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);
extern s32 gAllocateSoundChannelVoice(s32 arg0, s32 arg1, s32 *arg2, s32 arg3) __asm__("func_800220C4");
extern void gQueueSoundChannelStarts(s32 count, s32 *channels) __asm__("func_800221E4");

s32 gStartAudioStreamChannels(void) __asm__("func_800228F4");

/**
 * @brief Installs the stream IRQ handler and starts the configured channels.
 *
 * @param void No parameters.
 * @return Zero if setup was attempted, or -1 if the availability field is zero.
 *
 * @par Confirmed mechanics
 * Requires mixer +0x08, sets +0x04, copies +0x6C to +0x7C, installs gHandleAudioStreamSpuIrq
 * and arms the IRQ. Iterates over 0x24-byte channel records, obtains voices, builds an index
 * list with -1 sentinels, queues the list and stores current tick plus mixer +0x8C as the
 * deadline.
 *
 * @par Globals
 * Updates gMixerState and reads gFrameTickCounter.
 *
 * @par Callers
 * Direct calls from func_800244A4.
 *
 * @par Role and path
 * Finishes audio stream startup after the priming request succeeds.
 *
 * @par Unknown
 * The result 0 reports that setup ran, even if individual voice allocations failed.
 */
s32 gStartAudioStreamChannels(void)
{
    s32 ret = 0;
    u32 i;
    struct SoundVoice *voice;
    s32 params[4];
    s32 channels[2];

    if (gMixerState.unk08 != 0) {
        gMixerState.active = 1;
        gMixerState.unk7C = gMixerState.unk6C;
        gSpuInstallIrqCallback(gHandleAudioStreamSpuIrq);
        gArmAudioStreamSpuIrq();

        params[0] = gMixerState.unk18;
        for (i = 0; i < gMixerState.channelCount; i++) {
            params[1] = gMixerState.channels[i].unk08;
            params[2] = gMixerState.channels[i].unk0C;
            voice = func_80046B04(gMixerState.channels[i].unk08, 0xA, 0, 0xFF);
            gMixerState.channels[i].voice = voice;
            if (voice != 0) {
                func_800477D8(voice, params[2], params[1], -1);
                params[1] = voice->volume;
                params[2] = voice->pan;
                channels[i] = gMixerState.channels[i].id =
                    gAllocateSoundChannelVoice(0x64, voice->unk0D, params,
                                  gMixerState.channels[i].unk20);
            } else {
                channels[i] = -1;
            }
        }
        gQueueSoundChannelStarts(gMixerState.channelCount, channels);
        gMixerState.unk88 = gFrameTickCounter + gMixerState.unk8C;
    } else {
        ret = -1;
    }
    return ret;
}
