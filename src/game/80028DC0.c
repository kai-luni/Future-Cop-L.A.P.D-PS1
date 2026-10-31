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

extern struct Unk800A0B70 D_800A0B70;

extern struct LinkNode *func_800195A4(struct LinkState *state);

void func_80028DC0(s32 arg)
{
    struct LinkNode *node;
    struct LinkNode *cur;

    node = func_800195A4(D_800A0B70.state);
    if (node != 0) {
        node->next = 0;
        node->data = arg;

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
}
