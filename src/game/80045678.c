// Categories: Platform > Sound > SE | Game Engine > Object Pool
#include "game/types.h"

struct SoundChannel {
    s32 id;
    s32 priority;
    s16 slot;
    s8 index;
    u8 unk0B;
    s32 unk0C;
    s32 unk10;
    u8 unk14[0x14];
    s32 unk28;
    struct SoundChannel *next;
    struct SoundChannel *prev;
};

extern u8 gSoundBankTableLow[] __asm__("D_800A1830");
extern s32 gChannelCount __asm__("D_8009F720");
extern struct SoundChannel *gChannelListHead __asm__("D_8009F724");

extern s32 func_80045494(s32 id, u8 *table, s32 limit);
extern void gQueueSoundChannelByPriority(struct SoundChannel *channel) __asm__("func_800455DC");
s32 gAllocPositionalSoundChannel(s32 sound, s32 id, s32 arg2, s32 arg3) __asm__("func_80045678");


/**
 * @brief Allocates and activates a SoundChannel entry for a resolved sound effect ID.
 *
 * Resolves sound via func_80045494 in D_800A1830 (limit 0x47, falling back to D_800A18D0 for IDs >= 0x46), checks active channel count D_8009F720 < 0x1E, pops a free SoundChannel from D_8009F724, populates id/slot/unk0C/unk10, and inserts it via func_800455DC. Category Platform > Sound > SE | Game Engine > Object Pool follows the SE table lookup and channel pool list management.
 *
 * @param sound Sound effect ID.
 * @param id Caller-assigned owner/tag identifier stored in channel->id.
 * @param arg2 Channel parameter stored in channel->unk0C.
 * @param arg3 Channel parameter stored in channel->unk10.
 * @return Allocated channel->index (0..29), or -1 if sound is invalid/inactive or 30 channels are busy.
 *
 * @par Globals
 * D_800A1830 - primary SE slot lookup table; D_8009F720/D_8009F724 - active channel count (max 0x1E) and free-list head.
 * @par Control flow
 * Early exits on slot == 0xFF and D_8009F720 >= 0x1E before straight-line initialization and list insertion.
 * @par Calls
 * func_80045494 - maps sound ID to D_800A7938 record index (or 0xFF); func_800455DC - links channel into D_8009F728/D_8009F72C.
 * @par Side effects
 * Pops a node from D_8009F724, writes SoundChannel fields, links it into D_8009F728/D_8009F72C, and increments D_8009F720.
 * @par Callers
 * Confirmed: func_8004DF28 (0x8004E0F4), func_8004E248 (0x8004E49C), func_8008363C (0x8008368C) / Likely: none / Unknown: none.
 */
s32 gAllocPositionalSoundChannel(s32 sound, s32 id, s32 arg2, s32 arg3)
{
    struct SoundChannel *channel;
    s32 index;
    s32 slot;

    index = -1;
    slot = func_80045494(sound, gSoundBankTableLow, 0x47);
    if (slot == 0xFF) {
        goto done;
    }
    if (gChannelCount >= 0x1E) {
        goto done;
    }
    channel = gChannelListHead;
    channel->id = id;
    channel->unk28 = 0;
    channel->slot = slot;
    channel->unk0C = arg2;
    channel->unk10 = arg3;
    channel->priority = 0;
    gChannelListHead = channel->next;
    index = channel->index;
    gQueueSoundChannelByPriority(channel);
done:
    return index;
}
