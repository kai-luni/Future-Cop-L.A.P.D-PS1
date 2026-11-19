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
extern void *gSoundChannelEnableMask __asm__("D_800A07BC");

extern void func_80098C68(s32, void *);
extern void func_80021AD0(struct SoundChannel *, s32, s32, s32);

void gFlushSoundChannelUpdates(void) __asm__("func_80021E1C");

/**
 * @brief Applies pending sound-channel updates and clears the queued state.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Tests and sets the guard at +0x10, sends and clears the word at +0x18 in mode 0, then visits
 * the 0x1C-byte channel records when +0x14 is nonzero. It calls func_80021AD0 for matching
 * channel bits, sets each visited status to 2, sends a nonzero +0x14 mask in mode 1 and clears
 * the mask and guard.
 *
 * @par Globals
 * Reads and writes gSoundSystem. gSoundChannelEnableMask aliases its +0x14 word.
 *
 * @par Callers
 * Direct calls from gFlushSoundChannelsIfCvkActive (D_80023314), gQueueSoundChannelShutdown
 * (func_80021F3C).
 *
 * @par Role and path
 * Used both as a guarded periodic flush and during driver shutdown.
 *
 * @par Unknown
 * The meaning of channel status 2 beyond this flush is unresolved.
 */
void gFlushSoundChannelUpdates(void)
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
