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

/**
 * @brief Decrements a registered asset's reference count and releases the
 *        asset when it reaches zero.
 *
 * @param arg0 Address of the node pointer (the node itself is read through it).
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * (*arg0)->count is decremented, then the global list D_800A0B70 is walked:
 * every node whose count has reached 0 is unlinked and handed to
 * gRecycleModuleReadBuffer(). The loop restarts from the head after a removal, so a whole
 * chain of dead nodes is cleaned up in one call; the walk is guarded against
 * a NULL head.
 *
 * @par Globals
 * D_800A0B70 (asset list: head pointer at +0x18).
 *
 * @par Likely role
 * The "release handle" step used by the deferred queues in func_8001B4A8: an
 * image or sound keeps a node alive until the last user releases it.
 *
 * @par Callers
 * func_8001B4A8 (frame end, both queues).
 */
void func_80029650(Node **arg0)
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
