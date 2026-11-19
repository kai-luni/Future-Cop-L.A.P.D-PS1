// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundSystem {
    s32 bankCount;
    void **banks;
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");

extern void func_80021998(s32 index);

void gUnregisterSoundBank(s32 index) __asm__("func_80021DAC");

/**
 * @brief Releases a sound bank by index and clears its table cell.
 *
 * @param index Bank table index to process.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Branches around the operation if the signed index is negative, exceeds the limit, or the
 * table pointer is null. Otherwise calls func_80021998 and zeroes the indexed word.
 *
 * @par Globals
 * Reads the limit and table pointer from gSoundSystem and clears one table cell.
 *
 * @par Callers
 * Direct calls from func_800217A8, func_80023AB4.
 *
 * @par Role and path
 * The bank-release path uses this to drop the SPU allocation and remove its lookup entry.
 *
 * @par Unknown
 * The release helper determines whether a table cell has an allocated SPU handle.
 */
void gUnregisterSoundBank(s32 index)
{
    if (gSoundSystem.bankCount < index || index < 0 || gSoundSystem.banks == 0) {
        return;
    }
    func_80021998(index);
    gSoundSystem.banks[index] = 0;
}
