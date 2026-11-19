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

void gQueueCompletedModuleAsset(s32 arg) __asm__("func_80028DC0");

/**
 * @brief Appends a completed module asset to the loader's output list.
 *
 * @param arg Value stored in the node at +0x04.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Takes a two-word node from the pool at state +0x2C. On success stores the asset word and a
 * null link, then appends to the list at +0x30 or its existing tail.
 *
 * @par Globals
 * Reads the pool pointer and updates the completed-asset list in gStreamBufferList.
 *
 * @par Callers
 * Direct calls from gParseStreamedModuleChunks (func_8002902C).
 *
 * @par Role and path
 * The SHOC parser queues an asset once its accumulated data length reaches the expected size.
 *
 * @par Unknown
 * The call returns without queueing when no pool entry is available.
 */
void gQueueCompletedModuleAsset(s32 arg)
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
