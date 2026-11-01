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

extern struct Data_800A0B70 D_800A0B70;

extern void func_8002902C(u8 arg);
extern void func_800195D0(struct LinkState *state, int *link);

s32 func_800295E8(u8 arg)
{
    struct Node *node;
    struct Node *next;
    struct LinkState *state;
    s32 value;

    if (arg != 0) {
        func_8002902C(arg);
    }
    node = D_800A0B70.link;
    if (node == 0) {
        return 0;
    }
    value = node->value;
    next = node->next;
    state = D_800A0B70.state;
    D_800A0B70.link = next;
    func_800195D0(state, (int *)node);
    return value;
}
