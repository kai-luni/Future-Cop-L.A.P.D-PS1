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

/**
 * @brief Allocates and initializes a fixed-size block pool with an intrusive singly-linked free list.
 *
 * Rounds block size up to 4-byte alignment, allocates a 0x14-byte FixedPool header plus count * stride bytes via func_80017EBC, and threads all count blocks onto pool->free.
 *
 * @param count Number of fixed-size slots in the pool.
 * @param size Requested slot byte size (rounded up to a 4-byte multiple).
 * @param tag Heap/pool allocation tag passed to func_80017EBC.
 * @return Pointer to the initialized FixedPool header, or 0 if allocation fails.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Computes stride = (size + 3) & ~3 and bytes = count * stride + 0x14; calls func_80017EBC(bytes, tag). If non-null, sets pool header fields (count, available, stride, end) and loops count times linking each slot to the previous node.
 *
 * @par Calls
 * func_80017EBC - allocates the contiguous header-plus-slots buffer.
 *
 * @par Side effects
 * Allocates heap memory and populates the FixedPool header and embedded node links.
 *
 * @par Callers
 * Confirmed: func_80017C68 (0x80017CCC, 0x80017CE0, 0x80017CF4), func_80029F50 (0x8002A090), func_8003F79C (0x8003F814), func_80049690 (0x800496B0), and func_8008347C (0x80083494).
 */
struct FixedPool *func_800194DC(s32 count, s32 size, s32 tag)
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
