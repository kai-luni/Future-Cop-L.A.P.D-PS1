// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundVoice;

extern u8 gVolumeLimitSelector __asm__("D_8009F738");
extern struct SoundVoice *gVoiceList __asm__("D_8009F74C");
extern struct SoundVoice *gClass2SoundVoiceList __asm__("D_8009F754");

extern void gUpdateSoundVoiceListMix(struct SoundVoice *voice) __asm__("func_8004530C");
void gEnableHalfVolumeForSoundVoices(void) __asm__("func_800452D4");


/**
 *  @brief Sets a sound flag and processes two voice pointers.
 *
 *  Marks the shared flag before passing each stored voice to the same routine.
 *  The two calls occur in the order of the global pointers.
 *  @par Globals
 *  D_8009F738 is set to 1; D_8009F74C and D_8009F754 supply voices.
 *  @par Control flow
 *  Sets the flag and makes two unconditional calls.
 *  @par Calls
 *  func_8004530C processes each voice pointer.
 *  @par Side effects
 *  Changes D_8009F738; the calls may have additional effects.
 *  @par Callers
 *  Confirmed: func_80045050.
 */
void gEnableHalfVolumeForSoundVoices(void)
{
    gVolumeLimitSelector = 1;
    gUpdateSoundVoiceListMix(gVoiceList);
    gUpdateSoundVoiceListMix(gClass2SoundVoiceList);
}
