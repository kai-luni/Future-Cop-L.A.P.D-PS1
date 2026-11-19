// Categories: Graphics > Effects > Object Pool | Resources > Memory Management
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
extern void func_8003BDEC(struct Pool *pool);

/**
 * @brief Releases the backing storage of the beam-effect object pool D_800A3538.
 *
 * Calls func_8003BDEC(&D_800A3538) during world teardown to free D_800A3538.storage (allocated as 10 0x48-byte nodes by func_8006EDC4) via func_800181D4 and reset storage to 0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A3538 - 10-slot 0x48-byte beam-effect Pool struct; storage (+0x00) is freed and cleared via func_8003BDEC.
 *
 * @par Control flow
 * Straight-line call to func_8003BDEC(&D_800A3538) and return.
 *
 * @par Calls
 * func_8003BDEC - frees pool->storage if non-null and zeroes pool->storage.
 *
 * @par Side effects
 * Frees D_800A3538.storage and sets D_800A3538.storage = 0 via func_8003BDEC.
 *
 * @par Callers
 * Confirmed: func_8003F854 (0x8003F9FC, global world teardown sequence) / Likely: none / Unknown: none.
 */
void func_8006EDF0(void)
{
    func_8003BDEC(&gTracerPool);
}
