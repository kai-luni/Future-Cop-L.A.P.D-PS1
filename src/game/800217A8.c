// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundSystem {
    s32 bankCount;
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");

extern void func_80021DAC(s32 index);

void gUnloadAllSoundBanks(void) __asm__("func_800217A8");

/**
 * @brief Walks the sound bank slots and unloads each bank.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Starts at index zero and calls func_80021DAC once per slot while the index
 * is below the bank count. Reloads the count after each call.
 *
 * @par Globals
 * Reads the bank count field of gSoundSystem.
 *
 * @par Callers
 * Direct calls in the retail listing are from gClearSoundStorage.
 *
 * @par Role and path
 * Sound storage teardown calls this before clearing the bank descriptor.
 *
 * @par Unknown
 * The per-bank routine handles empty slots and owns the release side effects.
 */
void gUnloadAllSoundBanks(void)
{
    s32 i;

    for (i = 0; i < gSoundSystem.bankCount; i++) {
        func_80021DAC(i);
    }
}
