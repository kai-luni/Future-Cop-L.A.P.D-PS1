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
extern void D_80022FA8(void);

extern SpuIrqCallback gSpuInstallIrqCallback(SpuIrqCallback callback) __asm__("func_80098BF8");
extern void func_80022F44(void);
extern struct SoundVoice *func_80046B04(s32 sound, s32 arg1, s32 arg2, s32 arg3);
extern void func_800477D8(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);
extern s32 func_800220C4(s32 arg0, s32 arg1, s32 *arg2, s32 arg3);
extern void func_800221E4(s32 count, s32 *channels);

/**
 *  @brief Starts the configured stream channels when stream state is available.
 *
 *  It installs an SPU callback, prepares each configured voice and channel,
 *  submits the channel list, and records a deadline in the stream state.
 *  @return Zero if setup was attempted, or -1 if the availability field is zero.
 *  @par Globals
 *  D_800A0968 supplies configuration and receives active state, voice pointers,
 *  channel IDs and a deadline; D_8009FA2C supplies the current time.
 *  @par Control flow
 *  An availability check gates setup; a loop handles each configured channel,
 *  recording -1 in the submitted list when voice allocation fails.
 *  @par Calls
 *  gSpuInstallIrqCallback installs D_80022FA8; func_80022F44 prepares the stream.
 *  func_80046B04 obtains voices; func_800477D8 configures them;
 *  func_800220C4 creates channel IDs and func_800221E4 submits the list.
 *  @par Side effects
 *  Changes stream state, installs a callback and invokes audio setup routines.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
s32 func_800228F4(void)
{
    s32 ret = 0;
    u32 i;
    struct SoundVoice *voice;
    s32 params[4];
    s32 channels[2];

    if (gMixerState.unk08 != 0) {
        gMixerState.active = 1;
        gMixerState.unk7C = gMixerState.unk6C;
        gSpuInstallIrqCallback(D_80022FA8);
        func_80022F44();

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
                    func_800220C4(0x64, voice->unk0D, params,
                                  gMixerState.channels[i].unk20);
            } else {
                channels[i] = -1;
            }
        }
        func_800221E4(gMixerState.channelCount, channels);
        gMixerState.unk88 = gFrameTickCounter + gMixerState.unk8C;
    } else {
        ret = -1;
    }
    return ret;
}
