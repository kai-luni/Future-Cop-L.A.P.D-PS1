// Categories: Resources > Memory Management | Platform > Memory
#include "game/types.h"

struct LinkState {
    s32 stride;
    u8 *end;
    s32 unk08;
    s32 count;
    s32 *link;
};

struct LinkNode;

extern s32 *gFreeRangeTablePtr __asm__("D_8009F590");
extern s32 gFreeRangeCount __asm__("D_8009F594");
extern s32 gHeapSearchCursor __asm__("D_8009F598");
extern struct LinkState *gPool3Size __asm__("D_8009FA4C");
extern struct LinkState *gPool2Size __asm__("D_8009FAAC");
extern struct LinkState *gPool1Size __asm__("D_8009F9D0");

extern s32 func_80017D5C(s32 size);
extern s32 func_80017E0C(s32 size);
extern struct LinkNode *func_800195A4(struct LinkState *state);
extern void *func_8001E240(s32 size, s32 mode);
extern u8 *func_8001E318(u8 *destination, u8 *source, s32 count);

/**
 * Allocates memory using one of several allocation strategies.
 *
 * Supports a general-purpose allocator, bidirectional allocation
 * from a shared free-memory range table, and fixed-size memory pools.
 *
 * Allocation modes:
 *   0 - General allocator (func_8001E240).
 *       Falls back to mode 2 if allocation fails.
 *
 *   1 - Allocates from the end of a suitable free-memory range,
 *       using a reverse next-fit search.
 *
 *   2 - Allocates from the beginning of a suitable free-memory range,
 *       using a forward next-fit search.
 *
 *   3 - Allocates from one of three preallocated memory pools.
 *       Falls back to mode 1 if no pool is available.
 *
 * Modes 1 and 2 reserve an additional 4-byte header containing
 * the total aligned allocation size. The returned pointer refers
 * to the usable memory immediately after this header.
 *
 * Allocation sizes for modes 1 and 2 are rounded up to a
 * 4-byte boundary, including the header.
 *
 * Fully consumed free-memory ranges are removed from the range
 * table. The current search position is updated when allocating
 * from the beginning of a range (mode 2).
 *
 * If allocation through mode 1 or 2 fails, the function attempts
 * the general allocator (mode 0) without further fallback.
 *
 * @param size Number of usable bytes requested.
 * @param tag Allocation mode (0-3).
 * @return Pointer to allocated memory, or NULL on failure.
 *         Returns NULL immediately if size is zero.
 *
 * @note Mode 3 pool sizes and allocation semantics depend on
 *       the initialization of the LinkState structures.
 * @note The behavior for allocation modes outside 0-3 is undefined
 *       by this function.
 *
 * @par Globals
 * - D_8009F590: free-range table (start/end pointer pairs).
 * - D_8009F594: number of valid words in that table.
 * - D_8009F598: current search cursor for mode 2 allocations.
 * - D_8009FA4C, D_8009FAAC, D_8009F9D0: the three fixed-size pools, tried in
 *   that order by mode 3.
 *
 * @par Calls
 * func_8001E240() is the general allocator (mode 0), func_80017D5C() and
 * func_80017E0C() search the free-range table from the front/back,
 * func_800195A4() pops a node from a fixed pool and func_8001E318() moves the
 * table words when a range is consumed.
 *
 * @par Callers
 * The universal allocator of the executable - almost every subsystem uses it,
 * with mode 2 (front of the free range) by far the most common at boot and
 * mode 3 for the small fixed-size structures.
 *
 * @par Matching note
 * The @c goto retry ladder and the "size -= 4" adjustment are part of the
 * retail control flow: when a range allocation fails the header word is
 * subtracted again before the general allocator is tried, so the byte match
 * depends on this exact order.
 */
void *func_80017EBC(s32 size, s32 tag)
{
    s32 index;
    void *result;
    struct LinkState *pool;

    if (size == 0) {
        return 0;
    }
    result = 0;
    index = 1;

retry:
    switch (tag) {
    case 0:
        result = func_8001E240(size, 0);
        if (result != 0) {
            break;
        }
        if (index != 0) {
            tag = 2;
            index = 0;
            goto retry;
        }
        break;

    case 2:
        size += 7;
        size &= ~3;
        index = func_80017D5C(size);
        if (index < 0) {
            size -= 4;
            tag = 0;
            index = 0;
            goto retry;
        }
        result = (void *)gFreeRangeTablePtr[index];
        gHeapSearchCursor = index;
        *(s32 *)result = size;
        result = (u8 *)result + 4;
        gFreeRangeTablePtr[index] += size;
        if (gFreeRangeTablePtr[index] == gFreeRangeTablePtr[index + 1] && gFreeRangeCount >= 4) {
            func_8001E318((u8 *)&gFreeRangeTablePtr[index], (u8 *)(&gFreeRangeTablePtr[index] + 2),
                         (gFreeRangeCount - index - 1) * 4);
            gFreeRangeCount -= 2;
        }
        break;

    case 1:
        size += 7;
        size &= ~3;
        index = func_80017E0C(size);
        if (index < 0) {
            size -= 4;
            tag = 0;
            index = 0;
            goto retry;
        }
        gFreeRangeTablePtr[index + 1] -= size;
        result = (void *)gFreeRangeTablePtr[index + 1];
        *(s32 *)result = size;
        result = (u8 *)result + 4;
        if (gFreeRangeTablePtr[index] == gFreeRangeTablePtr[index + 1] && gFreeRangeCount >= 4) {
            func_8001E318((u8 *)&gFreeRangeTablePtr[index], (u8 *)(&gFreeRangeTablePtr[index] + 2),
                         (gFreeRangeCount - index - 1) * 4);
            gFreeRangeCount -= 2;
            if (gHeapSearchCursor >= index + 2) {
                gHeapSearchCursor -= 2;
            }
        }
        break;

    case 3:
        pool = gPool3Size;
        if (pool->stride < size || pool->count <= 0) {
            pool = gPool2Size;
            if (pool->stride < size || pool->count <= 0) {
                pool = gPool1Size;
                if (pool->count <= 0) {
                    tag = 1;
                    goto retry;
                }
            }
        }
        result = func_800195A4((struct LinkState *)pool);
        break;
    }
    return result;
}
