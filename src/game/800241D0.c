// Categories: Game Engine > Audio
#include "game/types.h"

struct Sample {
    u8 a, b, c, d;
};

struct SoundBank {
    struct Sample samples[4];
    u8 count;
    u8 pad[3];
    u8 enabled;
    u8 pad2[3];
    s32 head;
};

extern struct SoundBank gSoundEntryTable[] __asm__("D_800A7938");
extern s32 func_80047344(void *self, s32 key);

s32 gAppendSoundProgramSample(s32 key, s32 a, s32 c, s32 b, void *self) __asm__("func_800241D0");

/**
 * @brief Appends a three-byte sample descriptor to an enabled sound program with space.
 *
 * @param key Lookup key; only its low byte is used.
 * @param a Value stored in the sample's a byte.
 * @param c Value stored in the sample's c byte.
 * @param b Value stored in the sample's b byte.
 * @param self Object passed to the bank lookup.
 * @return 1 if an entry was appended, otherwise 0.
 *
 * @par Confirmed mechanics
 * Masks the lookup key to eight bits, calls func_80047344 and rejects -1. Uses a 0x1C-byte
 * program stride, checks enable byte +0x14 and count byte +0x10 against four, writes sample
 * bytes in the order a, b, c to one four-byte slot and increments the count.
 *
 * @par Globals
 * Reads and updates gSoundEntryTable.
 *
 * @par Callers
 * Direct calls from gInstallCvkGroupSamples (func_800235D4).
 *
 * @par Role and path
 * CVK group installation registers the loaded sample's program, bank and group fields here.
 *
 * @par Unknown
 * The fourth byte of the sample record is not written by this routine.
 */
s32 gAppendSoundProgramSample(s32 key, s32 a, s32 c, s32 b, void *self)
{
    s32 slot;
    s32 result;
    struct SoundBank *bank;

    slot = func_80047344(self, key & 0xFF);
    result = 0;
    if (slot != -1) {
        bank = &gSoundEntryTable[slot];
        if (bank->enabled != 0 && bank->count < 4) {
            bank->samples[bank->count].a = a;
            bank->samples[bank->count].c = c;
            bank->samples[bank->count].b = b;
            bank->count++;
            result = 1;
        }
    }
    return result;
}
