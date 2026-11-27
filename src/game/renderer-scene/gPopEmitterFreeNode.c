// Categories: Graphics > Effects > Object Pool | Utilities
#include "game/types.h"

struct Unk42764Node {
    struct Unk42764Node *next;
};

struct Unk42764Owner {
    u8 unk00[0x64];
    struct Unk42764Node *free;
    struct Unk42764Node *active;
    u8 unk6C[0x38];
    u16 count;
};
struct Unk42764Node *gPopEmitterFreeNode(struct Unk42764Owner *owner) __asm__("func_80042764");


/**
 * @brief Pops a node from the free list of an effect owner.
 *
 * @param owner Owner block: free-list head at +0x64, active list at +0x68,
 *              count at +0xA4.
 * @return The node, or NULL when the free list is empty. The count is *not*
 *         decremented here - func_80042788 increments it when the node is
 *         pushed onto the active list.
 *
 * @par Callers
 * func_80042AF4 only.
 */
struct Unk42764Node *gPopEmitterFreeNode(struct Unk42764Owner *owner)
{
    struct Unk42764Node *node;

    node = owner->free;
    if (node != 0) {
        owner->free = node->next;
    }
    return node;
}
