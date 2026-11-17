// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

extern s32 *D_8009F590;
extern s32 gFreeRangeCount __asm__("D_8009F594");
extern s32 gHeapSearchCursor __asm__("D_8009F598");

/**
 * Searches the free-memory range table in forward order for a block
 * large enough to satisfy the requested allocation size.
 *
 * Uses a next-fit search strategy, starting at the current search
 * position (D_8009F598). If no suitable range is found, the search
 * wraps around to the beginning of the table.
 *
 * Each free range is represented by two consecutive entries:
 * a start address and an end address. The available size is
 * calculated as end - start.
 *
 * Only searches for a suitable range; does not allocate memory
 * or modify the free-memory table.
 *
 * @param size Minimum number of contiguous bytes required.
 * @return Index of the first suitable range, or -1 if none exists.
 *
 * @par Globals
 * D_8009F590 (range table), D_8009F594 (word count), D_8009F598 (search cursor,
 * used as the starting index).
 *
 * @par Callers
 * func_80017EBC, for allocation mode 2 (allocate from the front of a range).
 *
 * @par Note
 * The cursor is not advanced here - the caller updates D_8009F598 after it has
 * chosen the range, which is why the function is a pure search.
 */
s32 func_80017D5C(s32 size)
{
    s32 i;
    s32 *block;

    block = &D_8009F590[gHeapSearchCursor];
    for (i = gHeapSearchCursor; i < gFreeRangeCount; i += 2, block += 2) {
        if (block[1] - block[0] >= size) {
            return i;
        }
    }
    block = &D_8009F590[1];
    for (i = 1; i < gHeapSearchCursor; i += 2, block += 2) {
        if (block[1] - block[0] >= size) {
            return i;
        }
    }
    return -1;
}
