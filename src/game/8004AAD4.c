// Categories: Game Engine > Object Pool | Graphics > Effects
#include "game/types.h"

struct Unk4AADParent {
    u8 unk00[0x39];
    u8 flags;
    u16 count;
};

struct Unk4AADNode {
    u8 unk00[0x28];
    u8 flags;
    u8 unk29[0x1F];
    struct Unk4AADNode *next;
    struct Unk4AADParent *parent;
    u8 unk50[8];
};

struct Unk4AADOwner {
    u8 unk00[0x84];
    struct Unk4AADNode *head;
};

extern struct Unk4AADNode *D_8009F798;

/**
 * @brief Unlinks a debris node from an owner's active list, pushes it onto free list D_8009F798, and updates its parent.
 *
 * Removes node from owner->head (+0x84) using previous when non-null, prepends node
 * onto the global free list D_8009F798, and if node->flags (+0x28) has bit 0x80 set,
 * decrements node->parent->count (+0x3A) and sets node->parent->flags (+0x39) bit 1.
 *
 * @param owner Owning debris instance holding the active list head at +0x84.
 * @param previous Preceding node in owner->head, or NULL when unlinking the head.
 * @param node 0x58-byte node to unlink and return to D_8009F798.
 * @return Updated owner->head (+0x84) pointer.
 *
 * @par Globals
 * D_8009F798 - head of the 0x58-byte debris node free list; node is pushed to the front.
 * @par Control flow
 * If/else unlink on previous != 0, free-list prepend, conditional parent update on (node->flags & 0x80), returns owner->head.
 * @par Calls
 * None.
 * @par Side effects
 * Mutates owner->head or previous->next, node->next, D_8009F798, and optionally node->parent->count and flags.
 * @par Callers
 * Confirmed: func_8004C0C0 (expired node pruning), func_8004C324 (message 10 debris teardown).
 */
struct Unk4AADNode *func_8004AAD4(struct Unk4AADOwner *owner, struct Unk4AADNode *previous,
                                  struct Unk4AADNode *node)
{
    if (previous != 0) {
        previous->next = node->next;
    } else {
        owner->head = node->next;
    }
    node->next = D_8009F798;
    D_8009F798 = node;
    if (node->flags & 0x80) {
        node->parent->count--;
        node->parent->flags |= 1;
    }
    return owner->head;
}
