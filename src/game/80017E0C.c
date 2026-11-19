// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

extern s32 *gFreeRangeTablePtr __asm__("D_8009F590");
extern s32 gFreeRangeCount __asm__("D_8009F594");
extern s32 gHeapSearchCursor __asm__("D_8009F598");

s32 gFindHeapRangeBackward(s32 size) __asm__("func_80017E0C");

/**
 * @brief Finds a sufficiently large heap gap by searching backward before the
 * current cursor.
 *
 * @param size Minimum gap length in bytes.
 * @return Table index of the first fit in search order, or -1.
 *
 * @par Confirmed mechanics
 * Starts two words before gHeapSearchCursor and walks downward in two-word
 * steps. It wraps to gFreeRangeCount minus 2 and can continue back through the
 * cursor. Tests the signed difference between adjacent boundaries.
 *
 * @par Globals
 * Reads gFreeRangeTablePtr, gFreeRangeCount, and gHeapSearchCursor.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80017EBC.
 *
 * @par Role and path
 * gAllocMemory selects this search for allocation from the upper end of a gap.
 *
 * @par Unknown
 * The allocator caller determines how a suitable range is split.
 */
s32 gFindHeapRangeBackward(s32 size)
{
    s32 i;
    s32 *block;

    block = &gFreeRangeTablePtr[gHeapSearchCursor - 2];
    for (i = gHeapSearchCursor - 2; i > 0; i -= 2, block -= 2) {
        if (block[1] - block[0] >= size) {
            return i;
        }
    }
    block = &gFreeRangeTablePtr[gFreeRangeCount - 2];
    for (i = gFreeRangeCount - 2; i >= gHeapSearchCursor; i -= 2, block -= 2) {
        if (block[1] - block[0] >= size) {
            return i;
        }
    }
    return -1;
}
