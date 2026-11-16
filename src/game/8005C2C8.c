// Categories: Gameplay > Damage | Utilities > Set
#include "game/types.h"

struct Unk5C2C8 {
    u8 unk00[0x2A];
    u8 count;
    u8 unk2B[0x0D];
    s32 items[10];
};

/**
 * @brief Inserts a value into a 10-element deduplicating set if space remains and it is absent.
 *
 * Used during segment-sweep collision checks (func_8005C344) to avoid hitting the same
 * candidate entity twice; checks count (+0x2A) < 10 and scans items[0..count-1] (+0x38).
 *
 * @param set Record holding u8 count at +0x2A and s32 items[10] at +0x38.
 * @param value 32-bit identifier or candidate pointer to record.
 * @return 0 if value was appended and count incremented; 1 if count >= 10 or value already exists.
 *
 * @par Globals
 * None.
 * @par Control flow
 * Capacity guard (count >= 10), linear scan loop over 0..count-1 returning 1 on match, then append and increment.
 * @par Calls
 * None.
 * @par Side effects
 * Writes set->items[set->count] and increments set->count on insertion.
 * @par Callers
 * Confirmed: func_8005C344 (segment sweep hit deduplication).
 */
s32 func_8005C2C8(struct Unk5C2C8 *set, s32 value)
{
    s32 i;

    if (set->count >= 0xA) {
        return 1;
    }
    for (i = 0; i < set->count; i++) {
        if (set->items[i] == value) {
            return 1;
        }
    }
    set->items[set->count] = value;
    set->count++;
    return 0;
}
