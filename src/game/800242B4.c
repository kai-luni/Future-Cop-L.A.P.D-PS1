// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundBankEntry {
    u8 key;
    u8 unk[3];
};

struct SoundBank {
    struct SoundBankEntry entries[4];
    u8 count;
    u8 cursor;
    u8 voices;
    u8 unk13;
    u8 used;
    u8 unk15;
    u8 key;
    u8 unk17;
    s32 head;
};

extern struct SoundBank gSoundEntryTable[] __asm__("D_800A7938");

/**
 *  @brief Removes the first matching key from a sound bank.
 *
 *  Searches the selected bank's active entries and fills any resulting gap
 *  with the last active entry, so entry order is not preserved.
 *  @param bankIndex Index of the bank to modify.
 *  @param key Entry key to remove.
 *  @return 1 if a matching entry was removed, otherwise 0.
 *  @par Globals
 *  D_800A7938 supplies the bank array and receives the modified entry count.
 *  @par Control flow
 *  Scans until a match or the count is reached; copies the last entry if needed.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Decrements the bank count and may overwrite an entry.
 *  @par Callers
 *  Confirmed: gClearSoundProgramSamples, gUnloadCvkSoundBank.
 */
s32 func_800242B4(s32 bankIndex, u8 key)
{
    struct SoundBank *bank;
    s32 i;
    register s32 result __asm__("$6");

    result = 0;
    __asm__("" : "=r"(result) : "0"(result));
    bank = &gSoundEntryTable[bankIndex];
    for (i = 0; i < bank->count; i++) {
        if (bank->entries[i].key == key) {
            bank->count--;
            if (i != bank->count) {
                bank->entries[i] = bank->entries[bank->count];
            }
            result = 1;
            break;
        }
    }
    return result;
}
