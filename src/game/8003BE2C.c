#include "game/types.h"

struct Pool {
    void *storage;
    void *active;
    void *free;
};

extern void *func_80017EBC(s32 size, s32 tag);

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
