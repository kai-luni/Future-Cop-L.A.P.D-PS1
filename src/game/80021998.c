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

void gReleaseSoundBankSpuHandle(s32 index) __asm__("func_80021998");

/**
 * @brief Releases a bank's SPU allocation and marks the stored handle invalid.
 *
 * @param index Index of the sound bank slot.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads the indexed bank via the pointer at gSoundSystem +4. For a non-null
 * bank, passes word +8 to func_80098708 and writes -1 to that word.
 *
 * @par Globals
 * Reads the gSoundBankSlots field of gSoundSystem and updates the bank entry's
 * handle.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021DAC.
 *
 * @par Role and path
 * The per-slot bank unload path calls this before clearing the bank table
 * entry.
 *
 * @par Unknown
 * The reserved callee manages the release of the underlying SPU block.
 */
void gReleaseSoundBankSpuHandle(s32 index)
{
    struct SoundBank *bank;

    bank = gSoundSystem.banks[index];
    if (bank != 0) {
        func_80098708(bank->handle);
        bank->handle = -1;
    }
}
