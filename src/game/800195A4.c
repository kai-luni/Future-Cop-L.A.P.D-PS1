// Categories: Resources > Memory Pools
#include "../../include/game/types.h"

struct LinkState {
    u8 pad_00[0xC];
    s32 count;
    struct LinkNode *head;
};

struct LinkNode {
    struct LinkNode *next;
};

/**
 * @brief Pops one node from a fixed-size pool's free list.
 *
 * @param state Pool descriptor: count at +0x0C, free-list head at +0x10.
 * @return The popped node, or NULL when the list is empty. The count is
 *         decremented only on success.
 *
 * @par Callers
 * func_80017EBC, for allocation mode 3 (one of the three fixed pools).
 *
 * @par Note
 * func_800195D0 is the matching push, so the two together are the pool
 * allocator; the nodes themselves are laid out by func_800194DC.
 */
struct LinkNode *func_800195A4(struct LinkState *state)
{
    struct LinkNode *node = state->head;

    if (node != 0) {
        state->head = node->next;
        state->count--;
    }
    return node;
}
