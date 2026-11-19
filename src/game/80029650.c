// Categories: Resources > Asset Registry | Resources > Reference Counting
#include "game/types.h"

typedef struct Node {
    struct Node *next;
    s32 pad[2];
    s32 count;
} Node;

typedef struct {
    u8 pad[0x18];
    Node *head;
} List;

extern List gStreamBufferList __asm__("D_800A0B70");
extern void gRecycleModuleReadBuffer(Node *node) __asm__("func_800288BC");

void gReleaseStreamAssetReference(Node **arg0) __asm__("func_80029650");

/**
 * @brief Decrements an asset buffer's reference count and recycles zero-count retained buffers.
 *
 * @param arg0 Address of the node pointer (the node itself is read through it).
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Dereferences the handle and subtracts one from node +0x0C. Walks the retained list, splices
 * out zero-count successors and checks the head after the walk. Clears each removed link before
 * calling gRecycleModuleReadBuffer.
 *
 * @par Globals
 * Updates the retained and reusable lists in gStreamBufferList.
 *
 * @par Callers
 * Direct calls from func_8001B4A8, gFlushDeferredAssetReleases (func_80029520), func_8002A31C,
 * func_8002A408.
 *
 * @par Role and path
 * Used by frame presentation and deferred release queues when uploaded or consumed chunks no
 * longer need their source buffer.
 *
 * @par Unknown
 * The handle and its node are expected to be valid at entry.
 */
void gReleaseStreamAssetReference(Node **arg0)
{
    Node *current;
    Node *node;
    List *list;

    (*arg0)->count--;
    __asm__ volatile("" : : : "memory");
    list = &gStreamBufferList;
    __asm__ volatile("" : "+r"(list));
    current = list->head;
    if (current != 0) {
        while (1) {
            if (*(Node *volatile *)&current->next != 0) {
                node = current->next;
                if (node->count == 0) {
                    current->next = node->next;
                    node->next = 0;
                    gRecycleModuleReadBuffer(node);
                } else {
                    current = node;
                }
            } else {
                node = list->head;
                if (node->count == 0) {
                    list->head = node->next;
                    node->next = 0;
                    gRecycleModuleReadBuffer(node);
                }
                break;
            }
        }
    }
}
