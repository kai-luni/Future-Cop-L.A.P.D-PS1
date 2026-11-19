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

void gResetSoundSystemState(void) __asm__("func_80021508");

/**
 * @brief Clears selected sound-system words and sets the default scalar at
 * +0x70 to 127.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clears words from +0 to +0x18 and three stride-0x1C words at +0x54, +0x38,
 * and +0x1C. Stores 0x7F to +0x70 in the return delay slot.
 *
 * @par Globals
 * Writes fields of gSoundSystem.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021C20.
 *
 * @par Role and path
 * Audio initialization calls this before applying sound table configuration.
 *
 * @par Unknown
 * The surrounding sound code determines the consumers of the default scalar.
 */
void gResetSoundSystemState(void)
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
