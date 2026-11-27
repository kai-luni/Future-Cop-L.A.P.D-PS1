// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundSystem {
    s32 bankCount;
    void **banks;
    s32 channelCount;
    void *channels;
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");

extern void gUnloadAllSoundBanks(void) __asm__("func_800217A8");

void gClearSoundStorage(void) __asm__("func_800216D8");

/**
 * @brief Releases configured sound banks and clears the bank and channel table
 * descriptors.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * If the bank pointer at +4 is nonzero, calls gUnloadAllSoundBanks and clears
 * pointer and count. If the channel pointer at +0x0C is nonzero, clears it and
 * the count at +8.
 *
 * @par Globals
 * Reads and clears fields of gSoundSystem, including gSoundBankSlots,
 * gSoundChannelCount, and gChannelTable.
 *
 * @par Callers
 * Direct calls in the retail listing are from gInitSoundStorage,
 * func_80021C80.
 *
 * @par Role and path
 * The sound teardown path detaches the configured storage after unloading bank
 * entries.
 *
 * @par Unknown
 * The caller retains ownership of the bank-pointer and channel arrays
 * themselves.
 */
void gClearSoundStorage(void)
{
    if (gSoundSystem.banks != 0) {
        gUnloadAllSoundBanks();
        gSoundSystem.banks = 0;
        gSoundSystem.bankCount = 0;
    }
    if (gSoundSystem.channels != 0) {
        gSoundSystem.channels = 0;
        gSoundSystem.channelCount = 0;
    }
}
