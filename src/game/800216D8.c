// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundSystem {
    s32 bankCount;
    void **banks;
    s32 channelCount;
    void *channels;
};

extern struct SoundSystem D_800A07A8;

extern void func_800217A8(void);

/**
 *  @brief Clears the sound system's bank and channel references.
 *
 *  Performs bank cleanup when a bank array is present, then resets the
 *  corresponding pointers and counts for each present array.
 *  @par Globals
 *  D_800A07A8 holds the bank and channel pointers and counts being reset.
 *  @par Control flow
 *  Checks each pointer independently; cleanup precedes clearing bank state.
 *  @par Calls
 *  func_800217A8 performs bank cleanup when the bank pointer is non-null.
 *  @par Side effects
 *  Clears present array pointers and their counts; cleanup may have other effects.
 *  @par Callers
 *  Confirmed: func_80021554, func_80021C80.
 */
void func_800216D8(void)
{
    if (D_800A07A8.banks != 0) {
        func_800217A8();
        D_800A07A8.banks = 0;
        D_800A07A8.bankCount = 0;
    }
    if (D_800A07A8.channels != 0) {
        D_800A07A8.channels = 0;
        D_800A07A8.channelCount = 0;
    }
}
