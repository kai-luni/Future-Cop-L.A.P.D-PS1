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
extern State D_800A0B70;

void func_800288BC(Node *node)
{
    Node *cur;
    node->count = 0;
    if (D_800A0B70.head == 0) {
        D_800A0B70.head = node;
    } else {
        cur = D_800A0B70.head;
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
