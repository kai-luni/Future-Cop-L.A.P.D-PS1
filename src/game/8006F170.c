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

extern struct Pool D_800A3538;
extern u8 func_8006EE14(struct PoolNode *node, void *arg);
extern void func_8003BF1C(struct Pool *pool, struct PoolNode *previous,
                          struct PoolNode *node);

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
 */
void func_8006F170(void *arg)
{
    struct PoolNode *node;
    struct PoolNode *next;
    struct PoolNode *previous;

    previous = 0;
    node = D_800A3538.active;
    while (node != 0) {
        next = node->next;
        if (func_8006EE14(node, arg) != 1) {
            func_8003BF1C(&D_800A3538, previous, node);
        } else {
            previous = node;
        }
        node = next;
    }
}
