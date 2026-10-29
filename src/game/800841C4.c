#include "game/types.h"

struct Node841C4 {
    u8 pad00[0x4C];
    u8 flags;
    u8 timer;
    u8 pad4E[2];
    s32 field50;
    u16 id;
    u8 pad56[2];
    struct Node841C4 *next;
};

struct Cell841C4 {
    struct Node841C4 *head;
    u32 unk04;
};

struct LinkState;
extern struct LinkState *D_8009F934;
extern struct Cell841C4 *D_8009F938;
extern s32 D_8009F93C;
extern s32 D_8009F940;
extern s32 D_8009FAA4[];
extern void func_800195D0(struct LinkState *state, int *link);

void func_800841C4(s32 id, s32 mode, s32 index)
{
    register s32 command __asm__("$11") = mode;
    s32 i;
    struct Node841C4 *node;
    struct Node841C4 *prev;

    for (i = D_8009F93C * D_8009F940 - 1; (u32)i < 0x80000000U; i--) {
        node = D_8009F938[i].head;
        prev = 0;
        if (node == 0) {
            continue;
        }

        while (node != 0) {
            if (node->id == id) {
                switch (command) {
                case 0x19:
                    if (index == 0) {
                        if (prev == 0) {
                            D_8009F938[i].head = node->next;
                        } else {
                            prev->next = node->next;
                        }
                        func_800195D0(D_8009F934, (int *)node);
                    } else {
                        if (index < 0) { node->field50 = -1; }
                        else { node->field50 = D_8009FAA4[0] + index * 300; }
                        node->flags &= 0x7F;
                    }
                    return;

                case 0x1A:
                    if (index == 0) {
                        node->flags |= 0x08;
                    } else {
                        node->timer = 0x12C / index;
                        node->flags &= 0xF7;
                    }
                    return;

                case 0x1B:
                    if (index == 0) {
                        node->flags |= 0x10;
                    } else {
                        node->flags &= 0xEF;
                    }
                    return;
                }
                return;
            }

            prev = node;
            node = node->next;
        }
    }
}
