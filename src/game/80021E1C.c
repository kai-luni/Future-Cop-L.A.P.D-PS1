#include "game/types.h"

struct SoundChannel {
    void *unk00;
    u8 unk04[4];
    s32 unk08;
    s32 channel;
    s32 unk10;
    s32 unk14;
    s32 unk18;
};

struct SoundSystem {
    s32 bankCount;
    void **banks;
    s32 channelCount;
    struct SoundChannel *channels;
    s32 active;
    u32 mask;
    void *unk18;
};

extern struct SoundSystem D_800A07A8;
extern void *D_800A07BC;

extern void func_80098C68(s32, void *);
extern void func_80021AD0(struct SoundChannel *, s32, s32, s32);

void func_80021E1C(void)
{
    struct SoundSystem *sys;
    struct SoundChannel *chan;
    s32 i;

    sys = &D_800A07A8;
    if (sys->active != 0) {
        return;
    }
    sys->active = 1;
    if (sys->unk18 != 0) {
        func_80098C68(0, sys->unk18);
        sys->unk18 = 0;
    }
    if (D_800A07A8.mask != 0) {
        i = D_800A07A8.channelCount;
        chan = D_800A07A8.channels;
        i--;
        for (; i != -1; i--) {
            if ((1 << chan->channel) & D_800A07A8.mask) {
                func_80021AD0(chan, chan->unk10, chan->unk14, chan->unk18);
            }
            chan->unk08 = 2;
            chan++;
        }
        if (D_800A07A8.mask != 0) {
            func_80098C68(1, (void *)D_800A07A8.mask);
        }
    }
    D_800A07A8.mask = 0;
    D_800A07A8.active = 0;
}
