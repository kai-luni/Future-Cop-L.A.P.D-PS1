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

/**
 * @brief Resets a stream/asset buffer node's reference count and appends it to D_800A0B70.head.
 *
 * Confirmed: zeroes node->count (+0x0C) and links node at the tail of the singly-linked list rooted at D_800A0B70.head (+0x10). Category reflects D_800A0B70 stream/asset buffer management (func_80028918, func_80029650).
 *
 * @param node Buffer node (+0x00 next pointer, +0x0C reference count) to return to the free list.
 *
 * @par Globals
 * D_800A0B70 - stream/asset buffer state whose head pointer at +0x10 receives or anchors node.
 *
 * @par Control flow
 * Clears node->count, checks D_800A0B70.head == 0 for direct insertion, otherwise traverses cur->next until null.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Clears node->count (+0x0C) and mutates D_800A0B70.head (+0x10) or the tail node's next link (+0x00).
 *
 * @par Callers
 * Confirmed: func_80028918 (0x80028A14, 0x80028A78), func_80028D28 (0x80028DA8), func_80029650 (0x800296C4, 0x80029700) / Likely: none / Unknown: none.
 */
void func_800288BC(Node *node)
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
extern u8 D_8009F6A8;
extern void *D_8009F6AC;
extern s32 D_8009F6E8;
extern s32 D_8009F6B0;
