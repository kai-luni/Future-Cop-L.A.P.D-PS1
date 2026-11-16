// Categories: Utilities > Mathematics | Game Engine > Map Occupancy
#include "game/types.h"

/**
 * @brief Classifies a signed value into quartile index 0..3 relative to `range`.
 *
 * Compares `value` against `(range >> 2)`, `(range >> 1)`, and `((range * 3) >> 2)` using arithmetic right shifts, yielding four coarse buckets used for entity boundary checks and grid cell typing. Category Utilities > Mathematics | Game Engine > Map Occupancy follows the pure shift thresholds and callers func_8003BA50/func_8003BAE4.
 *
 * @param value Signed counter or coordinate to bucket.
 * @param range Signed span partitioned into four intervals.
 * @return `0` if `value <= range >> 2`, `1` if `< range >> 1`, `2` if `< (range * 3) >> 2`, else `3`.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Three ordered threshold checks with early returns; no loops or memory accesses.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8003BA50 (0x8003BA78, 0x8003BA88 in asm/us/main.s) and func_8003BAE4 (0x8003BB18 in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 func_8003B65C(s32 value, s32 range)
{
    if (value <= range >> 2) {
        return 0;
    }
    if (value < range >> 1) {
        return 1;
    }
    if (value < (range * 3) >> 2) {
        return 2;
    }
    return 3;
}
