// Categories: Resources > Object Pool | Game Engine > Memory Management
#include "game/types.h"

struct Pool {
    void *storage;
    void *active;
    void *free;
};

extern void *func_80017EBC(s32 size, s32 tag);

/**
 * @brief Allocates and initializes a fixed-stride intrusive free-list pool.
 *
 * Allocates size * count bytes with pool tag 1, chains each node's first pointer word
 * to the next slot, terminates the tail with null, and sets pool->free and active.
 *
 * @param pool Pool header receiving storage (+0x00), cleared active (+0x04), and free (+0x08).
 * @param size Byte stride of each element (must hold at least one pointer).
 * @param count Number of elements to allocate and link into the free list.
 * @return 1 on success, or 0 if func_80017EBC returns null.
 *
 * @par Globals
 * None.
 * @par Control flow
 * Allocates storage; returns 0 on null, loops i = 0 .. count-2 linking consecutive slots, writes null terminator at slot count-1.
 * @par Calls
 * func_80017EBC - allocates size * count bytes from tag 1.
 * @par Side effects
 * Allocates heap memory and initializes pool->storage, pool->active, and pool->free.
 * @par Callers
 * Confirmed: func_8005E1BC (allocates two object pools), func_8006EDC4 (D_800A3538 pool), func_800762B0 (kind-0x57 sprite node pool).
 */
s32 func_8003BE2C(struct Pool *pool, s32 size, s32 count)
{
    s32 i;

    pool->storage = func_80017EBC(size * count, 1);
    if (pool->storage == 0) {
        return 0;
    }

    for (i = 0; i < count - 1; i++) {
        *(void **)((u8 *)pool->storage + i * size) =
            (u8 *)pool->storage + (i + 1) * size;
    }
    *(void **)((u8 *)pool->storage + i * size) = 0;
    pool->active = 0;
    pool->free = pool->storage;
    return 1;
}
