// Categories: Resources > Memory Management
#include "game/types.h"

extern s32 *gFreeRangeTablePtr __asm__("D_8009F590");
extern s32 gFreeRangeCount __asm__("D_8009F594");

/**
 * @brief Binary search for the free-range entry that contains an address.
 *
 * @param value Address to look up.
 * @return The table index of the range containing @c value: the highest index
 *         whose start address is <= value.
 *
 * @par Confirmed flow
 * The table D_8009F590 holds start/end pairs and D_8009F594 is the word count.
 * The loop halves the interval while it is at least two entries wide, then
 * returns @c high when value >= table[high], otherwise @c low.
 *
 * @par Callers
 * func_800181D4, to find which range a freed block belongs to before merging it
 * with its neighbours.
 */
s32 func_8001814C(s32 value)
{
    s32 high = gFreeRangeCount - 1;
    s32 *table = gFreeRangeTablePtr;
    s32 low = 0;
    s32 mid;

    while (high - low >= 2) {
        mid = (low + high) / 2;
        if (value < table[mid]) {
            high = mid;
        } else {
            low = mid;
        }
    }
    if (value >= table[high]) {
        return high;
    }
    return low;
}
