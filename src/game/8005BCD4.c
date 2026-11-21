// Categories: Graphics > Effects > Object Pool | Gameplay > Object Lookup
#include "game/types.h"

struct Unk5BCD4Node {
    struct Unk5BCD4Node *next;
    void *first;
    void *second;
    u8 unk0C[0x1E];
    u8 flags;
};

struct Unk5BCD4Pool {
    void *storage;
    struct Unk5BCD4Node *active;
    struct Unk5BCD4Node *free;
};
struct Unk5BCD4Node *gFindActiveEffectByOwner(struct Unk5BCD4Pool *pool, void *target) __asm__("func_8005BCD4");


/**
 * @brief Finds the active effect node attached to a target object.
 *
 * @param pool Effect pool; +4 the active list head.
 * @param target The owner pointer to match (node +8).
 * @return The first active node with second == target and flag bit 1 clear,
 *         or 0.
 *
 * @par Confirmed
 * Linear walk of pool->active; nodes are 0x2A bytes (next, first, second,
 * 0x1E payload bytes, flags at +0x29). Flag bit 2 marks a node as skipped.
 *
 * @par Likely role
 * The "does this object already carry an effect" lookup of the proximity
 * spawner func_8005BDE4, so one owner gets at most one live node.
 *
 * @par Callers
 * func_8005BDE4.
 */
struct Unk5BCD4Node *gFindActiveEffectByOwner(struct Unk5BCD4Pool *pool, void *target)
{
    struct Unk5BCD4Node *node;

    for (node = pool->active; node != 0; node = node->next) {
        if (node->second == target && !(node->flags & 2)) {
            return node;
        }
    }
    return 0;
}
