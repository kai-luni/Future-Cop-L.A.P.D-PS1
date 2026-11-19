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
extern void gFlushSoundChannelUpdates(void) __asm__("func_80021E1C");

void gQueueSoundChannelShutdown(void) __asm__("func_80021F3C");

/**
 * @brief Queues all configured sound channels for shutdown and flushes the sound-channel
 * updates.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * ORs 1 shifted by each record's +0x0C channel number, using 0x1C-byte records, then calls
 * func_80021AB4 and gFlushSoundChannelUpdates.
 *
 * @par Globals
 * Reads the channel count and array in gSoundSystem. The first callee ORs the mask into
 * gSoundSystem +0x18.
 *
 * @par Callers
 * Direct calls from gShutdownSoundSystem (func_80021C80).
 *
 * @par Role and path
 * The shutdown sequence flushes the combined channel mask before closing the SPU DMA event.
 *
 * @par Unknown
 * The routine does not filter channels by the status in each record.
 */
void gQueueSoundChannelShutdown(void)
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
    gFlushSoundChannelUpdates();
}
