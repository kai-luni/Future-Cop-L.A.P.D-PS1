// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundVoice;

extern u8 gVolumeLimitSelector __asm__("D_8009F738");
extern struct SoundVoice *gVoicePriorityList __asm__("D_8009F744");
extern struct SoundVoice *gVoiceList __asm__("D_8009F74C");
extern struct SoundVoice *D_8009F754;

extern void func_8004530C(struct SoundVoice *voice);

/**
 *  @brief Clears a sound flag and processes three stored voices when the flag is set.
 *
 *  The flag gates a one-time sequence of operations on the stored voice pointers.
 *  @par Globals
 *  D_8009F738 is tested and cleared; D_8009F744, D_8009F74C, and
 *  D_8009F754 supply the three voice pointers.
 *  @par Control flow
 *  If the flag is nonzero, clears it and processes the voices in pointer order;
 *  otherwise returns without action.
 *  @par Calls
 *  func_8004530C is called once for each stored voice.
 *  @par Side effects
 *  Clears D_8009F738 and may cause effects through the voice calls.
 *  @par Callers
 *  Confirmed: func_80023E84.
 */
void func_80045370(void)
{
    if (gVolumeLimitSelector != 0) {
        gVolumeLimitSelector = 0;
        func_8004530C(gVoicePriorityList);
        func_8004530C(gVoiceList);
        func_8004530C(D_8009F754);
    }
}
