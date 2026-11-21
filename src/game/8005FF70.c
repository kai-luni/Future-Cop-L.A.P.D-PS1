// Categories: Gameplay > Table Lookup
#include "game/types.h"

struct Unk5FF70 {
    u8 key;
    u8 unk01[0x2F];
};

extern s32 gDefinitionEntryCount __asm__("D_8009F7F8");
extern struct Unk5FF70 gEffectKeyTable[] __asm__("D_800A2300");
s32 gFindEffectKeyIndex(s32 key) __asm__("func_8005FF70");


/**
 * @brief Finds the index of a spawn/definition record by its key byte.
 *
 * Linear search over the table D_800A2300 with D_8009F7F8 entries, comparing
 * each record's key byte with the requested key.
 * @param key Key byte to look for.
 * @return The record index, or -1 when nothing matches.
 * @par Globals
 * D_800A2300 - record table (0x30-byte stride); D_8009F7F8 - record count.
 * @par Control flow
 * Bounded linear loop.
 * @par Calls
 * None.
 * @par Side effects
 * None observed.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
s32 gFindEffectKeyIndex(s32 key)
{
    s32 i;

    for (i = 0; i < gDefinitionEntryCount; i++) {
        if (gEffectKeyTable[i].key == key) {
            return i;
        }
    }
    return -1;
}
