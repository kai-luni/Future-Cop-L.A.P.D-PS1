#include "game/types.h"

extern s32 *D_8009F590;
extern s32 D_8009F594;
extern s32 D_8009F598;

/**
 * Searches the free-memory range table in reverse order for a block
 * large enough to satisfy the requested allocation size.
 *
 * Uses a reverse next-fit search strategy, starting immediately
 * before the current search position (D_8009F598). If no suitable
 * range is found, the search wraps around to the end of the table.
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
 */
s32 func_80017E0C(s32 size)
{
    s32 i;
    s32 *block;

    block = &D_8009F590[D_8009F598 - 2];
    for (i = D_8009F598 - 2; i > 0; i -= 2, block -= 2) {
        if (block[1] - block[0] >= size) {
            return i;
        }
    }
    block = &D_8009F590[D_8009F594 - 2];
    for (i = D_8009F594 - 2; i >= D_8009F598; i -= 2, block -= 2) {
        if (block[1] - block[0] >= size) {
            return i;
        }
    }
    return -1;
}
