// Categories: Platform > Sound | Resources > Sound Banks
#include "game/types.h"

struct SoundBank {
    u8 unk00[8];
    s32 handle;
};

struct SoundSystem {
    s32 bankCount;
    struct SoundBank **banks;
};

extern struct SoundSystem gSoundSystem __asm__("D_800A07A8");

extern void func_80098708(s32 handle);

/**
 * @brief Releases the SPU RAM handle for a loaded sound bank entry and marks it invalid.
 *
 * Looks up D_800A07A8.banks[index]; if non-null, calls func_80098708(bank->handle) to mark
 * its SPU reservation entry and trigger cleanup, then sets bank->handle (+0x08) to -1.
 *
 * @param index Sound bank slot index into D_800A07A8.banks.
 *
 * @par Globals
 * D_800A07A8 - global SoundSystem struct (reads banks pointer array at +0x04).
 *
 * @par Control flow
 * Single null check on D_800A07A8.banks[index] != 0 guarding the release and handle reset.
 *
 * @par Calls
 * func_80098708 - sets bit 0x80000000 on matching entry in D_8009EF0C and cleans up the SPU table.
 *
 * @par Side effects
 * Updates SPU table entry via func_80098708 and writes -1 to bank->handle (+0x08).
 *
 * @par Callers
 * Confirmed: func_80021DAC (0x80021DEC, bounds-checked sound bank unload helper that clears banks[index]) / Likely: none / Unknown: none.
 */
void func_80021998(s32 index)
{
    struct SoundBank *bank;

    bank = gSoundSystem.banks[index];
    if (bank != 0) {
        func_80098708(bank->handle);
        bank->handle = -1;
    }
}
