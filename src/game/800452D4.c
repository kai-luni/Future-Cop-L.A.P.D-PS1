// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundVoice;

extern u8 gVolumeLimitSelector __asm__("D_8009F738");
extern struct SoundVoice *D_8009F74C;
extern struct SoundVoice *D_8009F754;

extern void func_8004530C(struct SoundVoice *voice);

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
void func_800452D4(void)
{
    gVolumeLimitSelector = 1;
    func_8004530C(D_8009F74C);
    func_8004530C(D_8009F754);
}
