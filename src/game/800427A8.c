// Categories: Graphics > Effects > Object Pool
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

/**
 * @brief Unlinks a segment node from the active list and returns it to the
 *        owner's free list.
 *
 * @param owner Pool owner; +0x64 free head, +0x68 active head, +0xA4 count.
 * @param previous Predecessor in the active list, or 0 for the head.
 * @param node Node to retire.
 * @return Nothing.
 *
 * @par Confirmed
 * previous->next (or owner->active) takes node->next; node is pushed onto
 * owner->free and the count decrements. Pure list surgery, no freeing to
 * the heap.
 *
 * @par Likely role
 * The retire path of the two segment updaters func_800437E8/func_80043CA8:
 * expired trail/ribbon segments recycle into the same pool the emitter
 * func_80042AF4 allocates from.
 *
 * @par Callers
 * func_800437E8, func_80043CA8.
 */
void func_800427A8(struct Unk42764Owner *owner, struct Unk42764Node *previous,
                   struct Unk42764Node *node)
{
    if (previous != 0) {
        previous->next = node->next;
    } else {
        owner->active = node->next;
    }
    node->next = owner->free;
    owner->free = node;
    owner->count--;
}
