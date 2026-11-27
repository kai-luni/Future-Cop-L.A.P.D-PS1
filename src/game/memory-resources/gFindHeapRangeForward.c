// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

extern s32 *gFreeRangeTablePtr __asm__("D_8009F590");
extern s32 gFreeRangeCount __asm__("D_8009F594");
extern s32 gHeapSearchCursor __asm__("D_8009F598");

s32 gFindHeapRangeForward(s32 size) __asm__("func_80017D5C");

/**
 * @brief Finds a sufficiently large heap gap by searching forward from the
 * current cursor.
 *
 * @param size Minimum gap length in bytes.
 * @return Table index of the first fit in search order, or -1.
 *
 * @par Confirmed mechanics
 * Starts at gHeapSearchCursor and walks every second table index up to
 * gFreeRangeCount. If needed it wraps to index 1 and stops before the initial
 * cursor. A gap fits when its next boundary minus its current boundary is at
 * least the requested size.
 *
 * @par Globals
 * Reads gFreeRangeTablePtr, gFreeRangeCount, and gHeapSearchCursor.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80017EBC.
 *
 * @par Role and path
 * gAllocMemory selects this search for allocation from the lower end of a gap.
 *
 * @par Unknown
 * The allocator caller determines how a suitable range is split.
 */
s32 gFindHeapRangeForward(s32 size)
{
    s32 i;
    s32 *block;

    block = &gFreeRangeTablePtr[gHeapSearchCursor];
    for (i = gHeapSearchCursor; i < gFreeRangeCount; i += 2, block += 2) {
        if (block[1] - block[0] >= size) {
            return i;
        }
    }
    block = &gFreeRangeTablePtr[1];
    for (i = 1; i < gHeapSearchCursor; i += 2, block += 2) {
        if (block[1] - block[0] >= size) {
            return i;
        }
    }
    return -1;
}
