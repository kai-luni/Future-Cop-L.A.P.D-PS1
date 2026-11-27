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
void gPushEmitterActiveNode(struct Unk42764Owner *owner, struct Unk42764Node *node) __asm__("func_80042788");


/**
 * @brief Pushes a node onto the active list of an effect owner.
 *
 * @param owner Owner block: @c active at +0x68, count at +0xA4.
 * @param node  Node; its first word is the link.
 * @return Nothing.
 *
 * @par Confirmed
 * node->next = owner->active, owner->active = node, count++ - a LIFO push.
 * func_80042764 pops from the free list of the same owner, so the pair is a
 * fixed-size node pool.
 *
 * @par Callers
 * func_80042AF4 only.
 */
void gPushEmitterActiveNode(struct Unk42764Owner *owner, struct Unk42764Node *node)
{
    node->next = owner->active;
    owner->active = node;
    owner->count++;
}
