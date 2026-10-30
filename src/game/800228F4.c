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

extern struct StreamState D_800A0968;
extern s32 D_8009FA2C;
extern void D_80022FA8(void);

extern SpuIrqCallback func_80098BF8(SpuIrqCallback callback);
extern void func_80022F44(void);
extern struct SoundVoice *func_80046B04(s32 sound, s32 arg1, s32 arg2, s32 arg3);
extern void func_800477D8(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);
extern s32 func_800220C4(s32 arg0, s32 arg1, s32 *arg2, s32 arg3);
extern void func_800221E4(s32 count, s32 *channels);

s32 func_800228F4(void)
{
    s32 ret = 0;
    u32 i;
    struct SoundVoice *voice;
    s32 params[4];
    s32 channels[2];

    if (D_800A0968.unk08 != 0) {
        D_800A0968.active = 1;
        D_800A0968.unk7C = D_800A0968.unk6C;
        func_80098BF8(D_80022FA8);
        func_80022F44();

        params[0] = D_800A0968.unk18;
        for (i = 0; i < D_800A0968.channelCount; i++) {
            params[1] = D_800A0968.channels[i].unk08;
            params[2] = D_800A0968.channels[i].unk0C;
            voice = func_80046B04(D_800A0968.channels[i].unk08, 0xA, 0, 0xFF);
            D_800A0968.channels[i].voice = voice;
            if (voice != 0) {
                func_800477D8(voice, params[2], params[1], -1);
                params[1] = voice->volume;
                params[2] = voice->pan;
                channels[i] = D_800A0968.channels[i].id =
                    func_800220C4(0x64, voice->unk0D, params,
                                  D_800A0968.channels[i].unk20);
            } else {
                channels[i] = -1;
            }
        }
        func_800221E4(D_800A0968.channelCount, channels);
        D_800A0968.unk88 = D_8009FA2C + D_800A0968.unk8C;
    } else {
        ret = -1;
    }
    return ret;
}
