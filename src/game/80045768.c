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

extern struct SoundChannel D_800A1160[];

/**
 *  @brief Tests whether a sound channel has a valid ID and nonzero priority.
 *
 *  Reads the indexed channel from the shared sound-channel array when the
 *  index is nonnegative; no upper-bound check is performed.
 *  @param arg0 Unused argument.
 *  @param index Index of the sound channel to test.
 *  @return One if the index is nonnegative, the ID is not -1, and priority
 *  is nonzero; otherwise zero.
 *  @par Globals
 *  D_800A1160 contains the sound-channel array.
 *  @par Control flow
 *  Rejects negative indices and ID -1 before testing priority.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_8008352C.
 */
s32 func_80045768(s32 arg0, s32 index)
{
    register struct SoundChannel *base __asm__("$3");
    struct SoundChannel *channel;
    register s32 active __asm__("$2");
    register s32 result __asm__("$6");

    result = 0;
    if (index >= 0) {
        base = D_800A1160;
        channel = &base[index];
        if (channel->id != -1) {
            active = channel->priority != 0;
            __asm__ volatile ("" : "+r" (active));
            result = active;
        }
    }
    return result;
}
