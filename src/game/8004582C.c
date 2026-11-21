// Categories: Game Engine > Audio
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

extern struct SoundChannel gVoiceTable[] __asm__("D_800A1160");
extern s32 gChannelCount __asm__("D_8009F720");
extern struct SoundChannel *gChannelListHead __asm__("D_8009F724");
extern struct SoundChannel *gSoundChannelQueueHead __asm__("D_8009F728");
extern struct SoundChannel *gSoundChannelQueueTail __asm__("D_8009F72C");
void gInitSoundChannelList(void) __asm__("func_8004582C");


/**
 *  @brief Initializes the sound channel array as a singly linked list.
 *
 *  All 30 channels receive an unused ID and their array index, and their next
 *  pointers form a list terminated by null. Other channel fields are untouched.
 *  @par Globals
 *  D_800A1160 holds the channels; D_8009F724 is set to the list head.
 *  D_8009F720, D_8009F728, and D_8009F72C are reset to zero.
 *  @par Control flow
 *  A loop initializes each channel, then the final next pointer is cleared.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Rewrites channel IDs, indices, and next pointers and resets the listed globals.
 *  @par Callers
 *  Confirmed: func_800453B8 and func_80045548.
 */
void gInitSoundChannelList(void)
{
    struct SoundChannel *channel;
    s32 i;

    channel = gVoiceTable;
    for (i = 0; i < 0x1E; i++) {
        channel->id = -1;
        channel->index = i;
        channel->next = channel + 1;
        channel++;
    }
    channel[-1].next = 0;
    gChannelCount = 0;
    gChannelListHead = gVoiceTable;
    gSoundChannelQueueHead = 0;
    gSoundChannelQueueTail = 0;
}
