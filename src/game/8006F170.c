// Categories: Graphics > Scene Graph > Node Update | Game Engine > Frame
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
extern u8 func_8006EE14(struct PoolNode *node, void *arg);
extern void gReleasePoolNode(struct Pool *pool, struct PoolNode *previous,
                          struct PoolNode *node) __asm__("func_8003BF1C");
void gTickTracerPool(void *arg) __asm__("func_8006F170");


/**
 * @brief Walks the active node list, updates each node and unlinks retired ones.
 *
 * @param arg Frame argument forwarded to the per-node update.
 * @return Nothing.
 *
 * @par Confirmed
 * Iterates D_800A3538.active with a stored previous pointer. func_8006EE14(node,
 * arg) returns 1 to keep the node; anything else unlinks it with
 * func_8003BF1C(pool, previous, node), which moves it to the free list. `next`
 * is captured before the call because the update may free the node.
 *
 * @par Likely role
 * The generic "age every node of this pool" step - the same shape as the particle
 * pass, on a different pool.
 *
 * @par Callers
 * func_8003F4A0 (the render pass driver, first pass).
 *
 * @par Environment path
 * Per-frame update of the beam/tracer node pool D_800A3538 (created by
 * func_8006EDC4 with 10 nodes of 0x48 bytes, filled by func_8006F0E8): each
 * active node is ticked by func_8006EE14 and retired to the free list through
 * func_8003BF1C when the tick does not return 1. It is called by
 * gRunSceneListPasses (func_8003F4A0). This is weapon-effect bookkeeping, not
 * on the level-construction or camera path.
 */
void gTickTracerPool(void *arg)
{
    struct PoolNode *node;
    struct PoolNode *next;
    struct PoolNode *previous;

    previous = 0;
    node = gTracerPool.active;
    while (node != 0) {
        next = node->next;
        if (func_8006EE14(node, arg) != 1) {
            gReleasePoolNode(&gTracerPool, previous, node);
        } else {
            previous = node;
        }
        node = next;
    }
}
