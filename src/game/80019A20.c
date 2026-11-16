// Categories: Resources > Record Lookup
#include "game/types.h"

typedef struct Unk80019A20 {
    s32 id;
    s32 offset;
    u8 unk08[4];
} Unk80019A20;

/**
 *  @brief Finds a record by ID in an offset-linked buffer.
 *
 *  Examines record headers starting at base and advances by each header's
 *  offset until a matching ID is found or the size boundary is reached.
 *  The first header is examined even when size is nonpositive.
 *  @param id Record ID to find.
 *  @param base Start of the record buffer.
 *  @param size Byte extent used to limit subsequent record traversal.
 *  @return Pointer 12 bytes past the matching header, or null if not found.
 *  @par Globals
 *  None.
 *  @par Control flow
 *  A do-while loop follows offsets and exits immediately on a match.
 *  @par Calls
 *  None.
 *  @par Side effects
 *  None.
 *  @par Callers
 *  Confirmed: func_80051318, func_80051580.
 */
void *func_80019A20(s32 id, u8 *base, s32 size)
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
