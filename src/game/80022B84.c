// Categories: Utilities > Table Lookup
#include "game/types.h"

struct KeyValue {
    s32 key;
    s32 value;
};

s32 gFindStreamOption(struct KeyValue *table, s32 key, s32 *value, s32 *index) __asm__("func_80022B84");

/**
 * @brief Finds a matching key in a zero-terminated table of stream options.
 *
 * @param table Table to search, or null for an empty search.
 * @param key Key to find; zero is the terminator and cannot match.
 * @param value Optional destination for the matched value.
 * @param index Optional destination for the matched index.
 * @return One if a match is found, otherwise zero.
 *
 * @par Confirmed mechanics
 * Tests for a null table or zero first key, then walks eight-byte records. On a match, writes
 * the value and zero-based index through whichever output pointers are nonnull.
 *
 * @par Globals
 * None directly.
 *
 * @par Callers
 * Direct calls from gConfigureAudioStreaming (func_80022484).
 *
 * @par Role and path
 * The audio stream configurator uses this to override default option values.
 *
 * @par Unknown
 * The caller supplies a terminated table.
 */
s32 gFindStreamOption(struct KeyValue *table, s32 key, s32 *value, s32 *index)
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
