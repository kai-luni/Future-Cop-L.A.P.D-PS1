// Categories: Graphics > Effects > Object Pool | Resources > Memory Management
#include "game/types.h"

struct Unk5BD20Target {
    u8 unk00[8];
    s32 id;
};

struct Unk5BD20Node {
    struct Unk5BD20Node *next;
    struct Unk5BD20Target *first;
    struct Unk5BD20Target *second;
    s32 unk0C;
    s32 firstId;
    s32 secondId;
    u8 unk18[0x12];
    u8 flags;
};

struct Unk5BD20Pool {
    void *storage;
    struct Unk5BD20Node *active;
    struct Unk5BD20Node *free;
};

extern void gReleasePoolNode(struct Unk5BD20Pool *pool, struct Unk5BD20Node *previous,
                          struct Unk5BD20Node *node) __asm__("func_8003BF1C");
void gSweepEffectPool(struct Unk5BD20Pool *pool, void *owner) __asm__("func_8005BD20");


/**
 * @brief Garbage-collects one effect pool: releases flagged nodes and drops
 *        stale owner pointers by id comparison.
 *
 * @param pool Effect pool; +4 the active list head.
 * @param owner Unused in the body (kept for signature symmetry).
 * @return Nothing.
 *
 * @par Confirmed
 * Walks the active list; nodes with flag bit 0 are released via
 * func_8003BF1C(pool, previous, node). For the rest, first/second pointers
 * are nulled when the stored id no longer matches the target's current id
 * (+8 of the target). Nodes are 0x2A bytes.
 *
 * @par Likely role
 * The per-frame pool sweep of the effect driver func_8005E0AC: the id check
 * detects owners that were freed and reallocated, so dangling effect links
 * cannot fire on a recycled object.
 *
 * @par Callers
 * func_8005E0AC.
 */
void gSweepEffectPool(struct Unk5BD20Pool *pool, void *owner)
{
    struct Unk5BD20Node *node;
    struct Unk5BD20Node *next;
    struct Unk5BD20Node *previous;

    previous = 0;
    for (node = pool->active; node != 0; node = next) {
        next = node->next;
        if (node->flags & 1) {
            gReleasePoolNode(pool, previous, node);
            continue;
        }
        if (node->first != 0 && node->firstId != node->first->id) {
            node->first = 0;
        }
        previous = node;
        if (node->second != 0 && node->secondId != node->second->id) {
            node->second = 0;
        }
    }
}
