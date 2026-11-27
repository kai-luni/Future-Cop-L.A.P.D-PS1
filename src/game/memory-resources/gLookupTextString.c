// Categories: Resources > Text Tables | Graphics > Text
#include "game/types.h"

typedef struct TableEntry {
    u16 id;
    u16 offset;
} TableEntry;

typedef struct TableHeader {
    u16 unk0;
    u16 count;
    TableEntry entries[1];
} TableHeader;

typedef struct Unk800899A4 {
    TableHeader *header;
    u8 unk04[0x18];
    s32 unk1C;
} Unk800899A4;

extern Unk800899A4 *gTextLookupTable __asm__("D_8009F518");
u8 *gLookupTextString(u16 id) __asm__("func_80089914");


/**
 * @brief Resolves a text id to its string via binary search over the loaded
 *        text table.
 *
 * @param id The 16-bit text id.
 * @return Pointer to the string (entries base + entry offset), or 0 when no
 *         table is loaded or the id is missing.
 *
 * @par Confirmed
 * D_8009F518->header holds {unk0, count, entries[]}; entries are {u16 id,
 * u16 offset} sorted by id. Classic binary search (low/high, mid = (low +
 * high) / 2); on a match the result is (u8 *)entries + offset, i.e. the
 * offset is relative to the entry array base.
 *
 * @par Likely role
 * The central string lookup of the game: every text draw (func_800788F0),
 * the front-end widgets and the profile code resolve their labels through
 * it; the table is one of the loaded assets.
 *
 * @par Callers
 * func_8007799C, func_800788F0, func_8007E1A4, func_80080B70, func_8008125C.
 *
 * @par Role and path
 * Resolves a 16-bit text id by binary searching the sorted entries array in
 * gTextLookupTable->header and returning (u8 *)entries + entry->offset, or 0
 * if no table is loaded or the id is absent. Called by text and UI routines
 * including func_8007799C, func_800788F0, func_8007E1A4, func_80080B70, and
 * func_8008125C.
 */
u8 *gLookupTextString(u16 id)
{
    register u8 *result __asm__("$9") = 0;
    if (gTextLookupTable != 0) {
        register TableHeader *header __asm__("$2") = gTextLookupTable->header;
        register s32 count __asm__("$3");
        register s32 low __asm__("$7");
        register s32 high __asm__("$6");
        register TableEntry *entries __asm__("$8");
        register TableEntry *entry __asm__("$5");

        low = (s32)result;
        count = header->count;
        entries = header->entries;
        high = count - 1;

        do {
            s32 mid = (low + high) / 2;
            register s32 diff __asm__("$2");
            entry = &entries[mid];
            diff = id - entry->id;

            if (diff < 0) {
                high = mid - 1;
            } else {
                low = mid + 1;
            }
            if (diff == 0) {
                goto found;
            }
        } while (low <= high);
        goto done;
found:
        {
            register u32 offset __asm__("$2") = entry->offset;
            result = (u8 *)entries + offset;
        }
    }
done:
    return result;
}
