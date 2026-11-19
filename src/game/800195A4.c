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

struct LinkNode *gTakeFixedPoolSlot(struct LinkState *state) __asm__("func_800195A4");

/**
 * @brief Removes and returns the first free slot of a fixed-size pool.
 *
 * @param state Pool descriptor.
 * @return Removed slot pointer, or null if the list is empty.
 *
 * @par Confirmed mechanics
 * Loads the head at +0x10. If it is nonzero, replaces it with the slot's first
 * word and decrements the available count at +0x0C.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80017EBC, func_80028DC0,
 * func_8003FA34, func_80049C38, gSpawnSceneNode.
 *
 * @par Role and path
 * The shared allocator and gameplay pools call this when requesting one
 * fixed-size object.
 *
 * @par Unknown
 * Slot payload layout depends on the owner of the pool.
 */
struct LinkNode *gTakeFixedPoolSlot(struct LinkState *state)
{
    struct LinkNode *node = state->head;

    if (node != 0) {
        state->head = node->next;
        state->count--;
    }
    return node;
}
