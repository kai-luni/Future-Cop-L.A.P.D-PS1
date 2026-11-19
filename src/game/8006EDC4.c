// Categories: Gameplay > Effect Pools
#include "game/types.h"

struct PoolNode {
    struct PoolNode *next;
};

struct Pool {
    void *storage;
    struct PoolNode *active;
    struct PoolNode *free;
};

extern struct Pool gTracerPool __asm__("D_800A3538");
extern void func_8003BE2C(struct Pool *pool, s32 size, s32 count);

/**
 * @brief Creates the effect pool used by the attached-effect blocks.
 *
 * func_8003BE2C is called with the pool descriptor D_800A3538, a block size of
 * 0x48 bytes and ten slots - the same shape as the two pools func_8005E1BC
 * builds, but for a different consumer.
 * @par Globals
 * D_800A3538 - pool descriptor.
 * @par Control flow
 * None.
 * @par Calls
 * func_8003BE2C - pool creation.
 * @par Side effects
 * Allocates the pool storage and fills its free list.
 * @par Callers
 * Confirmed: the effect/level setup path (world init block).
 */
void func_8006EDC4(void)
{
    func_8003BE2C(&gTracerPool, 0x48, 0xA);
}
