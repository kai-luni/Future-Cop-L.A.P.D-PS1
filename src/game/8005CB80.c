// Categories: Graphics > Effects > Object Pool | Gameplay > Object Lookup
#include "game/types.h"

extern int gAttachPartEffect();

struct Unk5CB80Node {
    struct Unk5CB80Node *next;
    void *first;
    void *second;
    u8 unk0C[0x1C];
    u8 flags;
};

struct Unk5CB80Pool {
    void *storage;
    struct Unk5CB80Node *active;
    struct Unk5CB80Node *free;
};

/**
 * @brief Finds the active sprite node attached to a target object (twin of
 *        func_8005BCD4 with 0x28-byte nodes).
 *
 * @param pool Sprite pool; +4 the active list head.
 * @param target The owner pointer to match (node +8).
 * @return The first active node with second == target and flag bit 1 clear,
 *         or 0.
 *
 * @par Confirmed
 * Same walk as func_8005BCD4, but the payload is 0x1C bytes (flags at +0x27),
 * matching the 0x28-byte map-cell sprite records.
 *
 * @par Likely role
 * The duplicate guard of the map-cell sprite handler gAttachPartEffect.
 *
 * @par Callers
 * gAttachPartEffect.
 */
struct Unk5CB80Node *func_8005CB80(struct Unk5CB80Pool *pool, void *target)
{
    struct Unk5CB80Node *node;

    for (node = pool->active; node != 0; node = node->next) {
        if (node->second == target && !(node->flags & 2)) {
            return node;
        }
    }
    return 0;
}
