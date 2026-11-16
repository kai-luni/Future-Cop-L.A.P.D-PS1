// Categories: Game Engine > Lists
#include "game/types.h"

struct Node {
    struct Node *next;
    s32 unk04;
    s32 unk08;
    s32 count;
};

struct State {
    u8 pad[0x14];
    struct Node *head;
    struct Node *other;
};

extern struct State D_800A0B70;
extern void func_800288BC(struct Node *node);

/**
 *  @brief Removes the head node and either appends it to another list or handles it.
 *
 *  The head must be non-null. Its link and field at offset 0x08 are cleared
 *  before its count determines which destination receives it.
 *  @par Globals
 *  D_800A0B70 contains the source head and the other list head.
 *  @par Control flow
 *  Positive-count nodes are appended by traversing the other list; others are handled.
 *  @par Calls
 *  func_800288BC receives a node whose count is not positive.
 *  @par Side effects
 *  Changes list links and clears the removed node's field at offset 0x08.
 *  @par Callers
 *  Confirmed: func_8002902C.
 */
void func_80028D28(void)
{
    struct Node *node;
    struct Node *cur;

    node = D_800A0B70.head;
    D_800A0B70.head = node->next;
    node->next = 0;
    node->unk08 = 0;
    if (node->count > 0) {
        if (D_800A0B70.other == 0) {
            D_800A0B70.other = node;
        } else {
            cur = D_800A0B70.other;
            while (cur->next != 0) {
                cur = cur->next;
            }
            cur->next = node;
        }
    } else {
        func_800288BC(node);
    }
}
