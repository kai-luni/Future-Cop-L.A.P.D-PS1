// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundSystem {
    s32 bankCount;
    void **banks;
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");

extern void func_80021998(s32 index);

/**
 *  @brief Processes a sound bank index and clears its table entry.
 *
 *  Checks the index and bank table before invoking the bank operation and
 *  setting the selected bank pointer to null.
 *  @param index Bank table index to process.
 *  @par Globals
 *  D_800A07A8 supplies the bank count and table and receives the cleared entry.
 *  @par Control flow
 *  Returns for a negative index, an index greater than the count, or a null table.
 *  An index equal to the count is accepted by the implemented check.
 *  @par Calls
 *  func_80021998 processes the accepted index before its entry is cleared.
 *  @par Side effects
 *  Writes a null pointer to the selected bank entry; the call may have other effects.
 *  @par Callers
 *  Confirmed: func_800217A8, func_80023AB4.
 */
void func_80021DAC(s32 index)
{
    if (gSoundSystem.bankCount < index || index < 0 || gSoundSystem.banks == 0) {
        return;
    }
    func_80021998(index);
    gSoundSystem.banks[index] = 0;
}
