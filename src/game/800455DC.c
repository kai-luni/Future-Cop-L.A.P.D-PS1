// Categories: Game Engine > Sound Channels
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

extern s32 D_8009F720;
extern struct SoundChannel *D_8009F728;
extern struct SoundChannel *D_8009F72C;

/**
 *  @brief Inserts a sound channel into the global doubly linked list.
 *
 *  Nonzero-priority channels are inserted before the first lower-priority
 *  channel; a zero-priority channel is appended at the tail.
 *  @param channel Channel to link into the list.
 *  @par Globals
 *  D_8009F728 and D_8009F72C hold the head and tail; D_8009F720 is the count.
 *  @par Control flow
 *  A nonzero priority scans the list; zero priority selects the tail directly.
 *  Neighbor links and list endpoints are updated according to insertion position.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Changes channel and neighbor links, list endpoints as needed, and the count.
 *  @par Callers
 *  Confirmed: func_80045678.
 */
void func_800455DC(struct SoundChannel *channel)
{
    struct SoundChannel *prev;
    struct SoundChannel *next;

    next = 0;
    if (channel->priority == 0) {
        prev = D_8009F72C;
        channel->prev = prev;
    } else {
        prev = 0;
        for (next = D_8009F728; next != 0; next = next->next) {
            if (next->priority < channel->priority) {
                break;
            }
            prev = next;
        }
        channel->prev = prev;
    }
    channel->next = next;
    if (prev != 0) {
        prev->next = channel;
    } else {
        D_8009F728 = channel;
    }
    if (next != 0) {
        next->prev = channel;
    } else {
        D_8009F72C = channel;
    }
    D_8009F720++;
}
