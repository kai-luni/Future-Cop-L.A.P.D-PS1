#include "game/types.h"

struct Unk445B0Node {
    struct Unk445B0Node *next;
    u8 unk04[0x0B];
    u8 flags;
};

extern void func_80044260(struct Unk445B0Node *node, void *arg);

void func_800445B0(struct Unk445B0Node *node, void *arg)
{
    while (node != 0) {
        if (!(node->flags & 8)) {
            func_80044260(node, arg);
        }
        node = node->next;
    }
}
