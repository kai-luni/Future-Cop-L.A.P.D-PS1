// Categories: Resources > Memory Pools
#include "game/types.h"

struct PoolNode {
    struct PoolNode *next;
};

struct Pool {
    void *storage;
    struct PoolNode *active;
    struct PoolNode *free;
};

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Releases the backing storage of a fixed pool.
 *
 * The pool owns one contiguous block allocated at creation; this frees it
 * through the upper allocator and clears the pointer, so a second call is a
 * no-op. Active and free list heads are left alone - the caller discards the
 * pool descriptor afterwards.
 *
 * @param pool Pool descriptor (storage 0x00, active list 0x04, free list 0x08).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * One null test, then the free.
 *
 * @par Calls
 * func_800181D4 - frees the block.
 *
 * @par Side effects
 * Frees memory and clears the storage pointer.
 *
 * @par Callers
 * Confirmed: func_8003BE2C (pool setup) and the pool users of the 3Bxxx block.
 */
void func_8003BDEC(struct Pool *pool)
{
    if (pool->storage != 0) {
        gFreeBlock(pool->storage);
        pool->storage = 0;
    }
}
