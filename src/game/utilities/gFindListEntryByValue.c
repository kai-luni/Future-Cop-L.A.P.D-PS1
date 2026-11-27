// Categories: Utilities > Search
typedef unsigned char u8;
typedef struct { u8 value; u8 pad[5]; } Entry;
typedef struct { Entry *entries; int unused; u8 count; } List;

/**
 *  @brief Finds the first list entry with a matching value.
 *
 *  Searches the value byte of each entry in the supplied list and uses
 *  index zero as the fallback when no match is found.
 *  @param list List containing the entries and their count.
 *  @param value Byte value to find.
 *  @return Matching index, or zero if no entry matches.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  Scans entries in order and stops at the first match or the count.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_8007D408.
 */
u8 gFindListEntryByValue(List *list, u8 value) __asm__("func_8007D39C");

u8 gFindListEntryByValue(List *list, u8 value)
{
    Entry *entries = list->entries;
    u8 i = 0;
    while (i < list->count) {
        if (entries[i].value == value)
            break;
        i++;
    }
    if (i == list->count)
        i = 0;
    return i;
}
