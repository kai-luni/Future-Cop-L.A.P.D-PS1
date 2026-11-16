// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundChannelSlot {
    u8 unk00[0x0C];
    s32 channel;
    u8 unk10[0x0C];
};

struct SoundSystem {
    s32 bankCount;
    void **banks;
    s32 channelCount;
    struct SoundChannelSlot *channels;
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");

extern void func_80021AB4(u32 mask);
extern void func_80021E1C(void);

/**
 *  @brief Builds a mask of configured sound channels and dispatches it.
 *
 *  Reads the sound system's channel slots and combines their channel numbers
 *  into a bitmask before invoking the two follow-up operations.
 *  @par Globals
 *  D_800A07A8 supplies the channel count and slot array.
 *  @par Control flow
 *  Iterates over the configured slots, then calls both functions in order.
 *  @par Calls
 *  func_80021AB4 receives the mask; func_80021E1C performs the follow-up.
 *  @par Side effects
 *  Calls may modify state; this function does not directly write globals.
 *  @par Callers
 *  Confirmed: func_80021C80.
 */
void func_80021F3C(void)
{
    struct SoundChannelSlot *slot;
    s32 remaining;
    u32 mask;

    remaining = gSoundSystem.channelCount;
    slot = gSoundSystem.channels;
    mask = 0;
    for (; remaining != 0; remaining--) {
        mask |= 1 << slot->channel;
        slot++;
    }
    func_80021AB4(mask);
    func_80021E1C();
}
