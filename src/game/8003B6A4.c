// Categories: Gameplay > Entity Lifecycle | Game Engine > Object Pool
#include "game/types.h"

struct Unk3CA80 {
    u8 unk00[4];
    s16 unk04;
    u8 unk06;
    s8 unk07;
    u8 unk08[7];
    u8 flags;
    u8 unk10[4];
    s16 unk14;
    u8 unk16[0x0E];
    void (*callback)(s32 message, struct Unk3CA80 *object, s32 arg2, s32 arg3);
};

struct NodeB {
    u8 unk00[4];
    s32 unk04;
    u8 unk08[0x30];
    u8 unk38;
};

struct NodeA {
    struct NodeA *next;
    u8 unk04[0xC];
    u8 flags;
    u8 unk11[0xB];
    struct NodeB *unk1C;
};

struct NodeC {
    struct NodeC *next;
    u8 unk04[0xC];
    s32 unk10;
};

extern struct NodeA *D_800A1070[7];
extern struct NodeC *D_8009F30C;
extern void func_800181D4(void *arg);
extern void func_8003CA80(struct Unk3CA80 *object);

/**
 * @brief Tears down active flagged slots and frees deferred nodes owned by a given key.
 *
 * Walks all 7 lists in D_800A1070[0..6], invoking func_8003CA80 on entries whose
 * flags bit 2 is set and whose owner key (unk1C->unk04 when unk1C != 0 && unk38 == 0,
 * else 0) matches arg_, then unlinks and frees matching NodeC items from D_8009F30C.
 *
 * @param arg_ Owner/key value matched against NodeA->unk1C->unk04 (or 0) and NodeC->unk10.
 *
 * @par Globals
 * D_800A1070 - array of 7 active NodeA linked lists; D_8009F30C - singly-linked deferred NodeC list.
 *
 * @par Control flow
 * Outer loop i = 0..6 over D_800A1070[i] with owner-match guard, followed by a singly-linked filter/unlink pass over D_8009F30C.
 *
 * @par Calls
 * func_8003CA80 - dispatches message 7 to node->callback and marks slot retired; func_800181D4 - frees unlinked NodeC blocks.
 *
 * @par Side effects
 * Fires message 7 teardown callbacks, mutates NodeA slots, unlinks from D_8009F30C, and frees NodeC blocks.
 *
 * @par Callers
 * Confirmed: func_8003BAE4 (0x8003BB08, called before placing entity into map grid columns).
 */
void func_8003B6A4(s32 arg_)
{
    register s32 arg __asm__("$19");
    register struct NodeC *prev __asm__("$20");
    s32 i;
    struct NodeA *node;
    struct NodeC *cur;
    struct NodeC *next;

    arg = arg_;
    prev = 0;

    for (i = 0; i < 7; i++) {
        node = D_800A1070[i];
        while (node != 0) {
            if (node->unk1C != 0 && node->unk1C->unk38 == 0) {
                if (node->unk1C->unk04 != arg) {
                    goto next;
                }
            } else if (arg != 0) {
                goto next;
            }
            if (node->flags & 4) {
                func_8003CA80((struct Unk3CA80 *)node);
            }
        next:
            node = node->next;
        }
    }

    cur = D_8009F30C;
    while (cur != 0) {
        next = cur->next;
        if (cur->unk10 == arg) {
            if (prev != 0) {
                prev->next = next;
            } else {
                D_8009F30C = next;
            }
            cur->next = 0;
            func_800181D4(cur);
        } else {
            prev = cur;
        }
        cur = next;
    }
}
