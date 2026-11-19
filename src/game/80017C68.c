// Categories: Resources > Memory Pools | Platform > Memory
#include "game/types.h"

extern s32 *gFreeRangeTablePtr __asm__("D_8009F590");
extern s32 gFreeRangeCount __asm__("D_8009F594");
extern s32 gHeapSearchCursor __asm__("D_8009F598");
extern void *gFixedPool1 __asm__("D_8009F9D0");
extern s32 gHeapMainBlockSize __asm__("D_8009F9E8");
extern void *gFixedPool3 __asm__("D_8009FA4C");
extern s32 gHeapPoolBase __asm__("D_8009FA9C");
extern void *gFixedPool2 __asm__("D_8009FAAC");

extern void *gCreateFixedPool(s32 count, s32 size, s32 tag) __asm__("func_800194DC");

void gInitHeapAndFixedPools(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5) __asm__("func_80017C68");

/**
 * @brief Seeds the heap range table and creates three fixed-size pools.
 *
 * @param a0 Slot size for the first pool.
 * @param a1 Slot count for the first pool.
 * @param a2 Slot size for the second pool.
 * @param a3 Slot count for the second pool.
 * @param a4 Slot size for the third pool.
 * @param a5 Slot count for the third pool.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Sets the range count to 3 and cursor to 1. Writes the heap base twice and
 * heap end twice to the table. Passes each count and slot size pair to
 * gCreateFixedPool with tag 2.
 *
 * @par Globals
 * Reads gFreeRangeTablePtr, gHeapPoolBase, and gHeapMainBlockSize. Writes
 * table entries, gFreeRangeCount, gHeapSearchCursor, gFixedPool1, gFixedPool2,
 * and gFixedPool3.
 *
 * @par Callers
 * Direct calls in the retail listing are from gLoadFrontEndModule,
 * func_80024A24, func_80077050.
 *
 * @par Role and path
 * Mission and subsystem startup set up the shared allocator and fixed-size
 * pools used by later allocations.
 *
 * @par Unknown
 * The callers choose the element layouts and counts.
 */
void gInitHeapAndFixedPools(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5)
{
    s32 off = gHeapMainBlockSize;
    s32 *ptr = gFreeRangeTablePtr;
    s32 base;

    gFreeRangeCount = 3;
    base = gHeapPoolBase;
    gHeapSearchCursor = 1;
    ptr[0] = base;
    ptr[1] = base;
    ptr[2] = base + off;
    ptr[3] = base + off;
    gFixedPool1 = gCreateFixedPool(a1, a0, 2);
    gFixedPool3 = gCreateFixedPool(a5, a4, 2);
    gFixedPool2 = gCreateFixedPool(a3, a2, 2);
}
