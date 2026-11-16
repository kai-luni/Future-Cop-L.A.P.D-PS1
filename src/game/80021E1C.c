// Categories: Game Engine > Audio
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

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");
extern void *D_800A07BC;

extern void func_80098C68(s32, void *);
extern void func_80021AD0(struct SoundChannel *, s32, s32, s32);

/**
 *  @brief Processes pending sound-system state and channel mask bits.
 *
 *  A reentrancy flag protects processing of a pending pointer and channels.
 *  Selected channels are processed before the remaining mask is submitted.
 *  @par Globals
 *  D_800A07A8 holds the active flag, pending pointer, channels and mask.
 *  @par Control flow
 *  Returns if active; otherwise processes the pointer and loops over channels
 *  when the mask is nonzero, then clears the mask and active flag.
 *  @par Calls
 *  func_80098C68 submits the pointer or mask; func_80021AD0 handles a channel.
 *  @par Side effects
 *  Clears pending state, sets channel status to 2 and invokes audio operations.
 *  @par Callers
 *  Confirmed: func_80021F3C, func_80023314.
 */
void func_80021E1C(void)
{
    struct SoundSystem *sys;
    struct SoundChannel *chan;
    s32 i;

    sys = &gSoundSystem;
    if (sys->active != 0) {
        return;
    }
    sys->active = 1;
    if (sys->unk18 != 0) {
        func_80098C68(0, sys->unk18);
        sys->unk18 = 0;
    }
    if (gSoundSystem.mask != 0) {
        i = gSoundSystem.channelCount;
        chan = gSoundSystem.channels;
        i--;
        for (; i != -1; i--) {
            if ((1 << chan->channel) & gSoundSystem.mask) {
                func_80021AD0(chan, chan->unk10, chan->unk14, chan->unk18);
            }
            chan->unk08 = 2;
            chan++;
        }
        if (gSoundSystem.mask != 0) {
            func_80098C68(1, (void *)gSoundSystem.mask);
        }
    }
    gSoundSystem.mask = 0;
    gSoundSystem.active = 0;
}
