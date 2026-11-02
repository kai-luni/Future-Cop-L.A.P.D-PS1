#include "game/types.h"

typedef struct Node {
    s32 unk0;
    s32 unk4;
    u8 unk8[0xA];
    s16 unk12;
    u8 unk14[0x2C];
    struct Node *next;
} Node;

typedef struct MaskEntry {
    s32 mask;
    u8 pad[12];
} MaskEntry;

extern Node **D_8009F780;
extern s32 D_8009F78C;
extern s32 D_8009F794;
extern s32 D_8009FAA4;

extern void func_800497A8(void);
extern void func_8004A048(Node *node, s32 arg1, s32 arg2);

void func_8004A924(s32 arg0, s32 arg1, MaskEntry *arg2)
{
    s32 i;
    s32 j;
    s32 mask;
    Node *node;
    MaskEntry *entry;

    if (D_8009F780 == 0) {
        return;
    }

    if (D_8009F794 + 0x4B0 < D_8009FAA4) {
        func_800497A8();
        D_8009F794 = D_8009FAA4;
    }

    entry = arg2;
    for (j = 0; j < 5; j++) {
        for (i = 0; i < 5; i++) {
            mask = entry->mask;
            entry++;
            if (mask == 0) {
                continue;
            }
            node = D_8009F780[(arg0 + i) + D_8009F78C * (arg1 + j)];
            if (node == 0) {
                continue;
            }
            do {
                if (node->unk4 < 0 || node->unk4 > D_8009FAA4) {
                    if (mask & node->unk12) {
                        func_8004A048(node, arg0 + i, arg1 + j);
                    }
                }
                node = node->next;
            } while (node != 0);
        }
    }
}
