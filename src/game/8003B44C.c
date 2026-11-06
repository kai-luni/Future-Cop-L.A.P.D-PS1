#include "game/types.h"

struct KindData {
    u8 pad[4];
    u8 kind;
};

struct Node {
    struct Node *next;
    u8 pad[0x1c];
    struct KindData *data;
};

extern struct Node *D_800A1070[7];

void func_8003B44C(struct Node *node, s32 index)
{
    node->next = 0;
    switch (node->data->kind) {
    case 0x5d: index = 0; break;
    case 0x59: index = 1; break;
    case 0x5c: index = 2; break;
    case 0x5e: index = 3; break;
    case 0x5a: index = 4; break;
    case 0x57: index = 5; break;
    case 0x58: index = 6; break;
    }
    node->next = D_800A1070[index];
    D_800A1070[index] = node;
}
