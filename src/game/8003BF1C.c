// Categories: Graphics > Scene Graph > Object List | Utilities
#include "game/types.h"

struct PoolNode {
    struct PoolNode *next;
};

struct Pool {
    void *storage;
    struct PoolNode *active;
    struct PoolNode *free;
};
void gReleasePoolNode(struct Pool *pool, struct PoolNode *previous, struct PoolNode *node) __asm__("func_8003BF1C");


/**
 * @brief Unlinks a node from the active list and pushes it onto the free list.
 *
 * @param pool     Pool: @c storage at +0, @c active at +4, @c free at +8.
 * @param previous Predecessor inside the active list, NULL when the node is the
 *                 list head (then pool->active is advanced).
 * @param node     Node to release.
 * @return Nothing.
 *
 * @par Confirmed
 * previous ? previous->next = node->next : pool->active = node->next, then
 * node->next = pool->free and pool->free = node - the mirror image of
 * func_80042764/80042788, but here with an explicit predecessor (an O(1)
 * unlink inside a singly linked list).
 *
 * @par Callers
 * func_8005BD20, func_8005C0C0, func_8006F170 and func_80075A84 - the scene
 * walkers that create and destroy render objects per frame.
 *
 * @par Environment path
 * Returns a node to a fixed pool laid out as {storage, active, free}: it
 * unlinks the node from the active list at +0x04 using the supplied
 * predecessor (or advances the active head when the predecessor is null) and
 * pushes it onto the free list at +0x08. It is the inverse of gTakePoolNode
 * (func_8003BEF0). Callers are func_8005BD20, func_8005C0C0, func_8006F170
 * and gRunPolygonPass (func_80075A84, which frees dead emitter particles this
 * way each frame).
 */
void gReleasePoolNode(struct Pool *pool, struct PoolNode *previous,
                   struct PoolNode *node)
{
    if (previous != 0) {
        previous->next = node->next;
    } else {
        pool->active = node->next;
    }
    node->next = pool->free;
    pool->free = node;
}
