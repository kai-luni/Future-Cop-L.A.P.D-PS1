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
