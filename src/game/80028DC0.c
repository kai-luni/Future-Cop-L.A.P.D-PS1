// Categories: Game Engine > Object Pool | Resources > Memory Pools
#include "game/types.h"

struct LinkNode;

struct LinkState {
    u8 pad_00[0xC];
    s32 count;
    struct LinkNode *head;
};

struct LinkNode {
    struct LinkNode *next;
    s32 data;
};

struct Unk800A0B70 {
    u8 pad_00[0x2C];
    struct LinkState *state;
    struct LinkNode *head;
};

extern struct Unk800A0B70 gStreamBufferList __asm__("D_800A0B70");

extern struct LinkNode *func_800195A4(struct LinkState *state);

/**
 * @brief Pushes a value onto the persistent node list at D_800A0B70.
 *
 * A free node is taken from the linked pool func_800195A4; when the pool is
 * exhausted the call does nothing. The node's next pointer is terminated and the
 * value stored, then the node is appended: either as the new head when the list
 * was empty or behind the current tail, which needs a walk to the end.
 *
 * @param arg Value stored in the node at +0x04.
 *
 * @par Globals
 * D_800A0B70 - list state (state pointer 0x2C, head 0x30).
 *
 * @par Control flow
 * Allocation, then an append with a tail walk.
 *
 * @par Calls
 * func_800195A4 - node allocation from the pool.
 *
 * @par Side effects
 * Consumes one pool node and grows the list.
 *
 * @par Callers
 * Unknown: no direct edge; the persistent list is used by the level/save paths.
 */
void func_80028DC0(s32 arg)
{
    struct LinkNode *node;
    struct LinkNode *cur;

    node = func_800195A4(gStreamBufferList.state);
    if (node != 0) {
        node->next = 0;
        node->data = arg;

        if (gStreamBufferList.head == 0) {
            gStreamBufferList.head = node;
        } else {
            cur = gStreamBufferList.head;
            while (cur->next != 0) {
                cur = cur->next;
            }
            cur->next = node;
        }
    }
}
