// Categories: Resources > Memory Management
#include "game/types.h"

extern s32 *gFreeRangeTablePtr __asm__("D_8009F590");
extern s32 gFreeRangeCount __asm__("D_8009F594");

s32 gFindHeapBoundaryIndex(s32 value) __asm__("func_8001814C");

/**
 * @brief Finds the heap boundary index at or immediately below an address.
 *
 * @param value Address to look up in the heap boundary table.
 * @return Index of the selected boundary.
 *
 * @par Confirmed mechanics
 * Bisects between index 0 and gFreeRangeCount minus 1 using signed
 * comparisons. With adjacent bounds it compares the address to the upper
 * boundary and selects the lower index if necessary.
 *
 * @par Globals
 * Reads gFreeRangeTablePtr and gFreeRangeCount.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_800181D4.
 *
 * @par Role and path
 * gFreeBlock uses this lookup to locate the table interval in which to merge a
 * released allocation.
 *
 * @par Unknown
 * The lookup assumes that the boundary table is sorted and the address is in
 * its domain.
 */
s32 gFindHeapBoundaryIndex(s32 value)
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
