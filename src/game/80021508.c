// Categories: Game Engine > Audio
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
    s32 volume;
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");

/**
 *  @brief Resets selected sound-system fields and sets the default volume.
 *
 *  Initializes bank and channel references, three unknown fields, and the
 *  active flags of the three voice slots in the shared sound-system state.
 *  @par Globals
 *  D_800A07A8 holds the sound-system fields modified by this routine.
 *  @par Control flow
 *  Clears the common fields, loops over voice slots from index 2 down to 0,
 *  and then sets volume to 0x7F.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites the specified fields; other voice-slot fields are unchanged.
 *  @par Callers
 *  Unknown: no direct callers are listed.
 */
void func_80021508(void)
{
    s32 i;

    gSoundSystem.bankCount = 0;
    gSoundSystem.banks = 0;
    gSoundSystem.channelCount = 0;
    gSoundSystem.channels = 0;
    gSoundSystem.unk10 = 0;
    gSoundSystem.unk14 = 0;
    gSoundSystem.unk18 = 0;
    for (i = 2; i >= 0; i--) {
        gSoundSystem.voices[i].active = 0;
    }
    gSoundSystem.volume = 0x7F;
}
