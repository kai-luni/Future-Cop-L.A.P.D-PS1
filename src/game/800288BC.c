// Categories: Resources > Reference Counting | Platform > Streaming Audio
#include "game/types.h"

typedef struct Node {
    struct Node *next;
    s32 unk04;
    s32 unk08;
    s32 count;
} Node;
typedef struct State {
    u8 pad[0x10];
    Node *head;
} State;
extern State gStreamBufferList __asm__("D_800A0B70");

void gRecycleModuleReadBuffer(Node *node) __asm__("func_800288BC");

/**
 * @brief Clears a stream buffer's reference count and appends it to the reusable list.
 *
 * @param node Buffer node (+0x00 next pointer, +0x0C reference count) to return to the free
 * list.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Stores zero at node +0x0C. Writes the node at state +0x10 if the list is empty, otherwise
 * walks +0x00 next pointers and writes the final link.
 *
 * @par Globals
 * Updates the reusable-list head in gStreamBufferList or its tail node.
 *
 * @par Callers
 * Direct calls from gCompleteModuleBufferRead (D_80028918), gRetireParsedModuleBuffer
 * (func_80028D28), gReleaseStreamAssetReference (func_80029650).
 *
 * @par Role and path
 * The module loader recycles rejected reads and unreferenced parsed buffers here.
 *
 * @par Unknown
 * The caller supplies a detached node.
 */
void gRecycleModuleReadBuffer(Node *node)
{
    Node *cur;
    node->count = 0;
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
extern u8 gModuleReadBusy __asm__("D_8009F6A8");
extern void *gModuleReadActiveBuffer __asm__("D_8009F6AC");
extern s32 gModuleReadEndCheck __asm__("D_8009F6E8");
extern s32 gModuleReadCompleteWriteIndex __asm__("D_8009F6B0");
