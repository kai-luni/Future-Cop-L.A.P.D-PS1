#include "game/types.h"

struct SoundVoice {
    s32 active;      /* 0x00 */
    u8 unk04;        /* 0x04 */
    u8 unk05;        /* 0x05 */
    u8 unk06;        /* 0x06 */
    u8 pad07[7];     /* 0x07 */
    u16 unk0E;       /* 0x0E */
    u16 unk10;       /* 0x10 */
    u8 pad12[2];     /* 0x12 */
    s32 unk14;       /* 0x14 */
    s32 owner;       /* 0x18 */
};

struct SoundChannel {
    void *unk00;     /* 0x00 */
    u8 unk04[8];     /* 0x04 */
    s32 unk0C;       /* 0x0C */
    s32 unk10;       /* 0x10 */
    s32 unk14;       /* 0x14 */
    s32 unk18;       /* 0x18 */
};

struct SoundSystem {
    s32 bankCount;                       /* 0x00 */
    void **banks;                        /* 0x04 */
    s32 channelCount;                    /* 0x08 */
    struct SoundChannel *channels;       /* 0x0C */
    s32 active;                          /* 0x10 */
    u32 mask;                            /* 0x14 */
    void *unk18;                         /* 0x18 */
    struct SoundVoice voices[3];         /* 0x1C */
};

struct VoiceArgs {
    s32 unk00;
    s32 unk04;
    s32 unk08;
};

extern struct SoundSystem D_800A07A8;

extern void func_800223B0(s32 a0, s32 a1, s32 a2, s32 *a3, s32 *a4);

s32 func_800220C4(s32 unused, s32 index, struct VoiceArgs *args, s32 arg3)
{
    struct SoundSystem *sys;
    struct SoundChannel *chan;
    u8 *ptr;
    s32 out1;
    s32 out2;
    s32 result;
    s32 i;
    s32 one;
    s32 vol;
    s32 fmt;
    s32 rate;

    out1 = 0;
    out2 = 0;
    result = -1;
    func_800223B0(0, args->unk04, args->unk08, &out1, &out2);
    i = 0;
    one = 1;
    vol = 0x7F;
    fmt = 0x80FF;
    rate = 0x5FCE;
    sys = &D_800A07A8;
    ptr = (u8 *)sys->voices + 4;
    chan = sys->channels + index;
    chan->unk00 = 0;
    for (; i < 3; i++) {
        if (sys->voices[i].active == 0) {
            sys->voices[i].active = one;
            sys->voices[i].owner = index;
            sys->voices[i].unk05 = vol;
            sys->voices[i].unk06 = vol;
            sys->voices[i].unk14 = arg3;
            sys->voices[i].unk0E = fmt;
            sys->voices[i].unk10 = rate;
            chan->unk00 = ptr;
            break;
        }
        ptr += 0x1C;
    }
    if (chan->unk00 != 0) {
        chan->unk10 = args->unk00;
        chan->unk14 = out1;
        chan->unk18 = out2;
        result = index;
    }
    return result;
}
