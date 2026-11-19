// Categories: Gameplay > Entity Effects | Gameplay > Collision
#include "game/types.h"

struct Unk4C0C0Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk4C0C0Parent {
    u8 unk00[0x28];
    struct Unk4C0C0Vec3 vec;
    u8 unk34[8];
    s16 unk3C;
};

struct Unk4C0C0Node {
    u8 unk00[0x28];
    u8 flags;
    u8 unk29[0x1B];
    s16 unk44;
    s16 unk46;
    struct Unk4C0C0Node *next;
    union {
        struct Unk4C0C0Parent *parent;
        struct Unk4C0C0Vec3 vec;
    } u;
};

struct Unk4C0C0Owner {
    u8 unk00[0x0F];
    u8 unk0F;
    u8 unk10;
    u8 unk11[0x53];
    u8 unk64;
    u8 unk65[0x1F];
    struct Unk4C0C0Node *unk84;
};

struct Unk4C0C0Loc {
    s32 unk00;
    u8 unk04[0x0C];
    struct Unk4C0C0Vec3 vec;
    u8 unk1C[0x2D];
    u8 unk49;
};

extern s32 gLiveNodeCount __asm__("D_8009F9DC");

extern struct Unk4C0C0Node *func_8004AAD4(struct Unk4C0C0Owner *owner,
                                           struct Unk4C0C0Node *previous,
                                           struct Unk4C0C0Node *node);
extern s32 gSpawnTypeAtOwner(void *owner, u8 column, void *arg2, s32 row, s32 arg4) __asm__("func_800402DC");

/**
 * @brief Ages the attached effect/smoke nodes of an owner and spawns their puffs.
 *
 * The node list at owner + 0x84 is walked once. For every node whose life
 * counter 0x44 has gone negative the global effect count D_8009F9DC is reduced
 * and func_8004AAD4 removes the node; a removal that reports 0 marks the owner
 * with bit 0 in its 0x0F byte, so the caller knows the node vanished early.
 * While the low two bits of the global count are 2 or 3 every fourth removal
 * spawns a visible puff: the source vector is the node's parent vector (or its
 * own, when the 0x80 flag is clear), the height word 0x46/0x3C is scaled by 16
 * and the record type is fixed to 5 before func_800402DC places it into the
 * owner's grid column. Nodes that are still alive stay in the list and become
 * the new "previous" for the next unlink.
 *
 * @param owner Owner whose unk84 list is processed (column at 0x64, split flag
 *              at 0x10, early-out byte at 0x0F).
 *
 * @par Globals
 * D_8009F9DC - global effect/node counter, shared with the spawner.
 *
 * @par Control flow
 * One list walk; the removal, the early-out flag and the puff spawn are three
 * nested conditions inside it.
 *
 * @par Calls
 * func_8004AAD4 - unlink and retire one node. func_800402DC - grid placement of
 * the spawned puff.
 *
 * @par Side effects
 * Unlinks expired nodes, decrements the global counter and spawns effect
 * records; may set bit 0 of the owner's 0x0F byte.
 *
 * @par Callers
 * Confirmed: the effect update block (func_8004C324 neighbourhood).
 */
void func_8004C0C0(struct Unk4C0C0Owner *owner)
{
    struct Unk4C0C0Node *previous;
    struct Unk4C0C0Node *next;
    struct Unk4C0C0Node *node;
    struct Unk4C0C0Loc loc;

    node = owner->unk84;
    previous = 0;

    while (node != 0) {
        next = node->next;
        if (node->unk44 < 0) {
            gLiveNodeCount--;
            if (func_8004AAD4(owner, previous, node) == 0) {
                owner->unk0F |= 1;
            }
            if ((gLiveNodeCount & 3) >= 2) {
                if (owner->unk64 != 0) {
                    if (node->flags & 0x80) {
                        loc.vec = node->u.parent->vec;
                        loc.unk00 = node->u.parent->unk3C << 4;
                    } else {
                        loc.vec = node->u.vec;
                        loc.unk00 = node->unk46 << 4;
                    }
                    loc.unk49 = 5;
                    gSpawnTypeAtOwner(0, owner->unk64, &loc, (owner->unk10 >> 1) & 1, 0);
                }
            }
        } else {
            previous = node;
        }
        node = next;
    }
}
