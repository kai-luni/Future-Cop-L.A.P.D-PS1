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

extern struct SoundBank D_800A7938[];
extern s32 func_80047344(void *self, s32 key);

/**
 *  @brief Append three byte values to an enabled sound bank entry.
 *
 *  Looks up a bank using the low byte of the key and appends to its sample
 *  array only when the bank is enabled and has fewer than four entries.
 *  @param key Lookup key; only its low byte is used.
 *  @param a Value stored in the sample's a byte.
 *  @param c Value stored in the sample's c byte.
 *  @param b Value stored in the sample's b byte.
 *  @param self Object passed to the bank lookup.
 *  @return 1 if an entry was appended, otherwise 0.
 *  @par Globals
 *  D_800A7938 contains the banks and their sample arrays.
 *  @par Control flow
 *  Rejects a failed lookup, disabled bank, or full bank.
 *  @par Calls
 *  func_80047344 resolves self and the masked key to a bank index.
 *  @par Side effects
 *  Writes three sample bytes and increments the selected bank's count.
 *  @par Callers
 *  Confirmed: func_800235D4.
 */
s32 func_800241D0(s32 key, s32 a, s32 c, s32 b, void *self)
{
    s32 slot;
    s32 result;
    struct SoundBank *bank;

    slot = func_80047344(self, key & 0xFF);
    result = 0;
    if (slot != -1) {
        bank = &D_800A7938[slot];
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
