// Categories: Game Engine > Audio
#include "game/types.h"

struct SoundBank {
    s32 samples[4];
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

extern struct SoundBank D_800A7938[];

/**
 *  @brief Finds a used sound bank entry with the requested key.
 *
 *  The search examines the bank table in order and stops at the first unused
 *  entry, so entries beyond that point are not considered.
 *  @param key Key to compare against each used entry's byte-sized key.
 *  @return Matching table index, or -1 if no match is found before termination.
 *  @par Globals
 *  D_800A7938 supplies the 110-entry sound bank table.
 *  @par Control flow
 *  A loop stops on an unused entry, a matching key, or the table limit.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_80047344.
 */
s32 func_800472E8(s32 key)
{
    s32 i;

    for (i = 0; i < 0x6E; i++) {
        if (D_800A7938[i].used == 0) {
            i = -1;
            break;
        }
        if (D_800A7938[i].key == key) {
            break;
        }
    }
    if (i == 0x6E) {
        i = -1;
    }
    return i;
}
