// Categories: Platform > Sound > Channels | Gameplay > Entity Update
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
void gSetSoundChannelPosition(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 index) __asm__("func_8004572C");


/**
 * @brief Updates the 2D world coordinates (X, Z) of an active looping sound channel.
 *
 * Guards against negative channel indices; for indices >= 0, indexes the 52-byte D_800A1160
 * SoundChannel array and writes arg1 (X) to +0x0C and arg2 (Z) to +0x10.
 *
 * @param arg0 Unused sound/entity identifier (ignored).
 * @param arg1 World X coordinate written to channel->unk0C (+0x0C).
 * @param arg2 World Z coordinate written to channel->unk10 (+0x10).
 * @param arg3 Unused parameter (callers pass -1).
 * @param index Channel index in D_800A1160; negative values return immediately.
 *
 * @par Globals
 * D_800A1160 - array of 0x34-byte SoundChannel structs (+0x0C and +0x10 overwritten).
 * @par Control flow
 * Single early-return check on index < 0, then array indexing and two word stores.
 * @par Calls
 * None.
 * @par Side effects
 * Mutates D_800A1160[index].unk0C and D_800A1160[index].unk10.
 * @par Callers
 * Confirmed: func_8004DF28 (0x8004E130), func_8004E248 (0x8004E4D8), and func_8008363C (0x80083740) / Likely: none / Unknown: none.
 */
void gSetSoundChannelPosition(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 index)
{
    struct SoundChannel *channel;

    if (index < 0) {
        return;
    }
    channel = &gVoiceTable[index];
    channel->unk0C = arg1;
    channel->unk10 = arg2;
}
