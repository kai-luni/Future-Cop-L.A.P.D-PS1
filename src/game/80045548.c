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
extern struct SoundChannel *D_8009F724;
extern struct SoundChannel *D_8009F728;
extern struct SoundChannel *D_8009F72C;

extern void func_8004582C(void);

/**
 *  @brief Unlinks a sound channel and pushes it onto another channel list.
 *
 *  Repairs the neighboring links or active-list endpoints, then places the
 *  channel at the head of the list rooted at D_8009F724.
 *  @param channel Channel to remove and prepend to the other list.
 *  @par Globals
 *  D_8009F728 and D_8009F72C are list endpoints; D_8009F724 is the
 *  destination head; D_8009F720 is a counter decremented when nonzero.
 *  @par Control flow
 *  Branches on each neighbor and on whether the counter is nonzero.
 *  @par Calls
 *  func_8004582C runs when D_8009F720 is zero.
 *  @par Side effects
 *  Changes list links and heads, may decrement the counter, and may call
 *  the helper; channel->prev is not cleared by this function.
 *  @par Callers
 *  Confirmed: func_800457B8.
 */
void func_80045548(struct SoundChannel *channel)
{
    if (channel->prev != 0) {
        channel->prev->next = channel->next;
    } else {
        D_8009F728 = channel->next;
    }
    if (channel->next != 0) {
        channel->next->prev = channel->prev;
    } else {
        D_8009F72C = channel->prev;
    }
    channel->next = D_8009F724;
    D_8009F724 = channel;
    if (D_8009F720 != 0) {
        D_8009F720--;
    } else {
        func_8004582C();
    }
}
