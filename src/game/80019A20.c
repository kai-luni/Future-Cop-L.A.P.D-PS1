// Categories: Resources > Record Lookup
#include "game/types.h"

typedef struct Unk80019A20 {
    s32 id;
    s32 offset;
    u8 unk08[4];
} Unk80019A20;

void *gFindRecordPayloadById(s32 id, u8 *base, s32 size) __asm__("func_80019A20");

/**
 * @brief Finds the payload of a variable-size record by its first word.
 *
 * @param id Record ID to find.
 * @param base Start of the record buffer.
 * @param size Byte extent for subsequent records.
 * @return Matching header plus 0x0C, or null on exhaustion.
 *
 * @par Confirmed mechanics
 * Compares the ID at +0, returns record +0xC on match, and otherwise advances
 * by the word at +4. Tests the signed offset against the size after advancing.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80051318, func_80051580.
 *
 * @par Role and path
 * Resource readers use this helper to select a record within an already loaded
 * buffer.
 *
 * @par Unknown
 * The first header is read before the size comparison, and the record format
 * supplies valid lengths.
 */
void *gFindRecordPayloadById(s32 id, u8 *base, s32 size)
{
    u8 *current = base;
    u8 *found;
    void *result;

    do {
        found = current + 0xC;
        if (id == ((Unk80019A20 *)current)->id) {
            goto matched;
        }
        current += ((Unk80019A20 *)current)->offset;
    } while ((current - base) < size);

    result = 0;
    goto done;

matched:
    result = found;

done:
    return result;
}
