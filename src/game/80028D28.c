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
