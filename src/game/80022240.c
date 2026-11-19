// Categories: Game Engine > Audio Voices
#include "game/types.h"

struct SoundVoiceSlot {
    s32 active;
    u8 unk04[0x14];
    s32 owner;
};

struct SoundSystem {
    s32 bankCount;
    void **banks;
    s32 channelCount;
    void *channels;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    struct SoundVoiceSlot voices[3];
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");

extern void gQueueSoundChannelRelease(s32 channel) __asm__("func_8002238C");

void gReleaseSoundVoicesByOwner(s32 owner) __asm__("func_80022240");

/**
 * @brief Releases each active software voice belonging to the specified channel owner.
 *
 * @param owner Owner value to match and pass to the channel helper.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Checks the active word and owner of three records at stride 0x1C. On a match it calls
 * gQueueSoundChannelRelease, then stores zero to active and -1 to owner.
 *
 * @par Globals
 * Reads and updates the software voice records in gSoundSystem.
 *
 * @par Callers
 * Direct calls from gStopAudioStreamPlayback (func_80022A64), func_80023E84.
 *
 * @par Role and path
 * Stream shutdown uses the channel number as the software voice owner.
 *
 * @par Unknown
 * Several matching records would trigger one release call per record.
 */
void gReleaseSoundVoicesByOwner(s32 owner)
{
    s32 i;

    for (i = 0; i < 3; i++) {
        if (gSoundSystem.voices[i].active != 0 && gSoundSystem.voices[i].owner == owner) {
            gQueueSoundChannelRelease(owner);
            gSoundSystem.voices[i].active = 0;
            gSoundSystem.voices[i].owner = -1;
        }
    }
}
