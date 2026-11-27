// Categories: Platform > Audio
#include "game/types.h"

struct SoundVoice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
};

extern void gSetSoundVoiceMixParameters(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra) __asm__("func_800477D8");
extern void func_800243C4(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra);
void gUpdateSoundVoiceMix(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra) __asm__("func_80047800");


/**
 * @brief Applies pan/volume to a voice and forwards the raw values.
 *
 * The voice is first updated with the new values (func_800477D8), then the same
 * call is repeated with the values the voice actually adopted (voice->pan,
 * voice->volume, voice->extra), so the mixer and the voice record stay in sync.
 * @param voice Voice record (bank 0x00, volume 0x04, base 0x05, pan 0x06, extra 0x07).
 * @param pan Panning value.
 * @param volume Volume value.
 * @param extra Extra mixer field.
 * @par Globals
 * None.
 * @par Control flow
 * Two calls, no branches.
 * @par Calls
 * func_800477D8 - voice update. func_800243C4 - mixer update with the adopted values.
 * @par Side effects
 * Rewrites the voice's mixer fields.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
void gUpdateSoundVoiceMix(struct SoundVoice *voice, s32 pan, s32 volume, s32 extra)
{
    gSetSoundVoiceMixParameters(voice, pan, volume, extra);
    func_800243C4(voice, voice->pan, voice->volume, voice->extra);
}
