// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundSystem {
    s32 bankCount;
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");

extern void func_80021DAC(s32 index);

/**
 *  @brief Applies an operation to every indexed sound bank.
 *
 *  The sound system's bank count determines how many indices are passed to
 *  the per-bank function. The operation performed on each bank is not shown here.
 *  @par Globals
 *  D_800A07A8 supplies the bank count used as the loop bound.
 *  @par Control flow
 *  Iterates from index zero while the index is less than the bank count.
 *  @par Calls
 *  func_80021DAC receives each bank index in ascending order.
 *  @par Side effects
 *  Any bank changes are performed by func_80021DAC.
 *  @par Callers
 *  Confirmed: func_800216D8.
 */
void func_800217A8(void)
{
    s32 i;

    for (i = 0; i < gSoundSystem.bankCount; i++) {
        func_80021DAC(i);
    }
}
