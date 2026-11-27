// Categories: Resources > Asset Loader | Resources > File Access
#include "game/types.h"

struct LinkState {
    unsigned char pad_00[0xC];
    int count;
    int *link;
};

struct Node {
    struct Node *next;
    s32 value;
};

struct Data_800A0B70 {
    u8 pad_00[0x2C];
    struct LinkState *state;
    struct Node *link;
};

extern struct Data_800A0B70 gStreamBufferList __asm__("D_800A0B70");

extern void gParseStreamedModuleChunks(u8 arg) __asm__("func_8002902C");
extern void func_800195D0(struct LinkState *state, int *link);

s32 gPopCompletedModuleAsset(u8 arg) __asm__("func_800295E8");

/**
 * @brief Pops one completed module asset, optionally advancing the stream parser first.
 *
 * @param arg Non-zero to run the parser before popping; only the low 8 bits are tested.
 * @return The request block, or 0 when the queue is empty.
 *
 * @par Confirmed mechanics
 * Tests the low byte of a0 before calling gParseStreamedModuleChunks. If the list at state
 * +0x30 is nonempty, saves the asset in node +4, advances the head and calls func_800195D0 with
 * the pool at +0x2C and the removed node.
 *
 * @par Globals
 * Updates the completed-asset list in gStreamBufferList and uses its node pool.
 *
 * @par Callers
 * Direct calls from gLoadQueuedAssets (func_80024EC8).
 *
 * @par Role and path
 * The queued asset loader drains these blocks into its tag-dispatch path.
 *
 * @par Unknown
 * The popped block's ownership passes to the caller.
 */
s32 gPopCompletedModuleAsset(u8 arg)
{
    struct Node *node;
    struct Node *next;
    struct LinkState *state;
    s32 value;

    if (arg != 0) {
        gParseStreamedModuleChunks(arg);
    }
    node = gStreamBufferList.link;
    if (node == 0) {
        return 0;
    }
    value = node->value;
    next = node->next;
    state = gStreamBufferList.state;
    gStreamBufferList.link = next;
    func_800195D0(state, (int *)node);
    return value;
}
