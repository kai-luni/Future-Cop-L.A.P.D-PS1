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

extern void func_8002238C(s32 channel);

/**
 *  @brief Clears active voice slots belonging to an owner.
 *
 *  The routine scans the three global voice slots and processes every
 *  active slot whose owner matches the supplied value.
 *  @param owner Owner value to match and pass to the channel helper.
 *  @par Globals
 *  D_800A07A8 contains the voice slots whose active and owner fields change.
 *  @par Control flow
 *  A three-iteration loop checks both active status and owner equality.
 *  @par Calls
 *  func_8002238C is called with owner for each matching slot.
 *  @par Side effects
 *  Matching slots are marked inactive and their owners are set to -1.
 *  @par Callers
 *  Confirmed: func_80022A64, func_80023E84.
 */
void func_80022240(s32 owner)
{
    s32 i;

    for (i = 0; i < 3; i++) {
        if (gSoundSystem.voices[i].active != 0 && gSoundSystem.voices[i].owner == owner) {
            func_8002238C(owner);
            gSoundSystem.voices[i].active = 0;
            gSoundSystem.voices[i].owner = -1;
        }
    }
}
