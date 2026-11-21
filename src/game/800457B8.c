// Categories: Platform > Sound > SE
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

extern void gReleaseSoundVoice(s32 handle) __asm__("func_80046698");
extern void func_80045548(struct SoundChannel *channel);

/**
 * @brief Stops a sound channel by index: clears its id, releases the
 *        voice handle and unlinks the channel.
 *
 * @param arg0 Unused in the body (kept for the shared call signature).
 * @param index Channel index into D_800A1160; negative aborts.
 * @return Nothing.
 *
 * @par Confirmed
 * A channel with id == -1 is already free and returns. Otherwise id = -1,
 * func_80046698(unk28) releases the voice handle when present, and
 * func_80045548(channel) unlinks it.
 *
 * @par Likely role
 * The universal stop entry of the sound system: the sequencer
 * (func_80083834) and the gameplay handlers call it with the channel
 * index; the ignored first argument carries the sound id at the call
 * sites (often ORed with 0x40000000).
 *
 * @par Callers
 * func_8002E630, func_8002F30C, func_8003A160, func_80041880, func_8004CECC,
 * func_8004D958, func_8004DF28, func_8004E248, func_8008352C.
 */
void func_800457B8(s32 arg0, s32 index)
{
    struct SoundChannel *channel;

    if (index < 0) {
        return;
    }
    channel = &gVoiceTable[index];
    if (channel->id == -1) {
        return;
    }
    channel->id = -1;
    if (channel->unk28 != 0) {
        gReleaseSoundVoice(channel->unk28);
    }
    func_80045548(channel);
}
