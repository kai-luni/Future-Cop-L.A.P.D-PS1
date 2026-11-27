// Categories: Resources > Memory Pools | Utilities > Memory Management
#include "game/types.h"

struct FixedPool {
    s32 stride;
    u8 *end;
    s32 count;
    s32 available;
    void *free;
};

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");

struct FixedPool *gCreateFixedPool(s32 count, s32 size, s32 tag) __asm__("func_800194DC");

/**
 * @brief Allocates a fixed-size pool and initializes its intrusive free list.
 *
 * @param count Number of slots.
 * @param size Bytes per slot before rounding.
 * @param tag Tag passed to gAllocMemory.
 * @return Pool header pointer, or null if allocation fails.
 *
 * @par Confirmed mechanics
 * Rounds the slot size up to four bytes and allocates count times that size
 * plus 0x14. Initializes header fields and links each slot back to the
 * previous slot, with the last slot installed as the free-list head.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gInitHeapAndFixedPools,
 * func_80029F50, func_8003F79C, func_80049690, func_8008347C.
 *
 * @par Role and path
 * The allocator and other engine subsystems use the pool to recycle same-sized
 * records.
 *
 * @par Unknown
 * The allocation tag and payload layout are selected by the caller.
 */
struct FixedPool *gCreateFixedPool(s32 count, s32 size, s32 tag)
{
    struct FixedPool *pool;
    void **node;
    void *previous;
    s32 stride;
    s32 bytes;

    stride = (size + 3) & ~3;
    bytes = count * stride + 0x14;
    pool = gAllocMemory(bytes, tag);
    if (pool != 0) {
        previous = 0;
        node = (void **)(pool + 1);
        pool->count = count;
        pool->available = count;
        pool->stride = stride;
        pool->end = (u8 *)(bytes + (s32)pool);
        while (count-- > 0) {
            *node = previous;
            previous = node;
            node = (void **)((u8 *)previous + stride);
        }
        pool->free = previous;
    }
    return pool;
}
