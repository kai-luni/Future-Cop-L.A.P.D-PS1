// Categories: Utilities > Table Lookup
#include "game/types.h"

struct KeyValue {
    s32 key;
    s32 value;
};

/**
 *  @brief Finds the first matching key in a zero-terminated table.
 *
 *  Scans key-value records until a match or a record with key zero is reached.
 *  Optional output pointers receive the matched value and zero-based index.
 *  @param table Table to search, or null for an empty search.
 *  @param key Key to find; zero is the terminator and cannot match.
 *  @param value Optional destination for the matched value.
 *  @param index Optional destination for the matched index.
 *  @return One if a match is found, otherwise zero.
 *  @par Globals
 *  None accessed directly.
 *  @par Control flow
 *  Checks for an empty table, then loops until a match or terminator.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  Writes non-null output destinations only when a match is found.
 *  @par Callers
 *  Confirmed: gInitSoundStorage.
 */
s32 func_80021740(struct KeyValue *table, s32 key, s32 *value, s32 *index)
{
    s32 found;
    s32 i;

    found = 0;
    if (table != 0 && table->key != 0) {
    i = 0;
    do {
        if (table->key == key) {
            found = 1;
            if (value != 0) {
                *value = table->value;
            }
            if (index != 0) {
                *index = i;
            }
            break;
        }
        table++;
        i++;
    } while (table->key != 0);
    }
    return found;
}
