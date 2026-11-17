// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
};

extern u8 gVolumeLimitSelector __asm__("D_8009F738");

/**
 *  @brief Sets a sound voice's volume, pan, and extra parameter.
 *
 *  Stores the requested base volume and applies a global half-volume
 *  condition to the effective volume before updating the voice fields.
 *  @param voice Voice record to update.
 *  @param pan Pan value stored in the voice.
 *  @param volume Requested base volume and source for effective volume.
 *  @param extra Extra value stored in the voice.
 *  @par Globals
 *  D_8009F738 selects whether the effective volume is shifted right by one.
 *  @par Control flow
 *  Conditionally shifts volume, then stores the effective volume, pan, and extra.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Overwrites four signed-byte fields in the voice record.
 *  @par Callers
 *  Confirmed: func_8002283C, func_800228F4, func_8004772C, and func_80047800.
 */
void func_800477D8(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra)
{
    voice->baseVolume = volume;
    if (gVolumeLimitSelector != 0) {
        volume >>= 1;
    }
    voice->volume = volume;
    voice->pan = pan;
    voice->extra = extra;
}
