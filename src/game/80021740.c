// Categories: Utilities > Table Lookup
#include "game/types.h"

struct KeyValue {
    s32 key;
    s32 value;
};

s32 gFindConfigValue(struct KeyValue *table, s32 key, s32 *value, s32 *index) __asm__("func_80021740");

/**
 * @brief Looks up the first matching key in a zero-terminated array of
 * key-value pairs.
 *
 * @param table Array of key-value pairs or null.
 * @param key Nonzero key to match.
 * @param value Optional destination for the value.
 * @param index Optional destination for the zero-based index.
 * @return One if the key was found, otherwise zero.
 *
 * @par Confirmed mechanics
 * Checks for a null table and zero sentinel. Advances eight bytes and
 * increments the index per entry. On a match, writes the value and index to
 * the respective output pointers if supplied.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gInitSoundStorage.
 *
 * @par Role and path
 * Sound initialization uses this lookup for counts and memory addresses.
 *
 * @par Unknown
 * The caller defines the value type for each key.
 */
s32 gFindConfigValue(struct KeyValue *table, s32 key, s32 *value, s32 *index)
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
