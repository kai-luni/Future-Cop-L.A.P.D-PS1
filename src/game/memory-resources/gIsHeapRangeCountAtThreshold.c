// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

extern s32 gFreeRangeCount __asm__("D_8009F594");
extern s32 gHeapRangeCountThreshold __asm__("D_8009F5A0");

s32 gIsHeapRangeCountAtThreshold(void) __asm__("func_80017B88");

/**
 * @brief Tests whether the allocator's range-table word count has reached its
 * threshold.
 * @return One when the range count is at least the threshold, otherwise zero.
 *
 * @par Confirmed mechanics
 * Loads gFreeRangeCount and gHeapRangeCountThreshold, performs signed SLT, and
 * inverts the Boolean result in the return delay slot.
 *
 * @par Globals
 * Reads gFreeRangeCount and gHeapRangeCountThreshold.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80043430,
 * gSpawnMeshFractureDebris.
 *
 * @par Role and path
 * Heap initialization installs the threshold. Gameplay callers query it
 * through this predicate.
 *
 * @par Unknown
 * The callers determine what action to take at the threshold.
 */
s32 gIsHeapRangeCountAtThreshold(void)
{
    return gFreeRangeCount >= gHeapRangeCountThreshold;
}
