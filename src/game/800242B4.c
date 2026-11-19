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

s32 gRemoveSoundProgramSample(s32 bankIndex, u8 key) __asm__("func_800242B4");

/**
 * @brief Removes the first sound program sample whose key matches.
 *
 * @param bankIndex Index of the bank to modify.
 * @param key Entry key to remove.
 * @return 1 if a matching entry was removed, otherwise 0.
 *
 * @par Confirmed mechanics
 * Indexes the program at stride 0x1C and compares low-byte keys in four-byte sample records.
 * After decrementing the count, uses unaligned word loads and stores to copy the former last
 * record into any resulting gap.
 *
 * @par Globals
 * Reads and updates gSoundEntryTable.
 *
 * @par Callers
 * Direct calls from gClearSoundProgramSamples (func_80023A24), gUnloadCvkSoundBank
 * (func_80023AB4).
 *
 * @par Role and path
 * Both per-bank teardown and the all-program reset use the same removal operation.
 *
 * @par Unknown
 * Removal can change sample order.
 */
s32 gRemoveSoundProgramSample(s32 bankIndex, u8 key)
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
