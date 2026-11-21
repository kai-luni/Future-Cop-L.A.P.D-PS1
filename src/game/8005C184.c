// Categories: Gameplay > Object Spawning | Gameplay > Combat
#include "game/types.h"

struct Unk5C184Vec {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk5C184Msg {
    void *unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    struct Unk5C184Vec vec;
    u8 unk1C[0x2D];
    u8 unk49;
    u8 unk4A[6];
};

struct Unk5C184Target {
    u8 unk00[0xC1];
    s8 unkC1;
};

struct Unk5C184Kind {
    u8 unk00[0x16];
    u8 kind;
    u8 unk17[0x95];
    struct Unk5C184Target *unkAC;
    u8 unkB0[4];
    struct Unk5C184Kind *unkB4;
};

struct Unk5C184Node {
    u8 unk00[8];
    struct Unk5C184Kind *unk08;
    u8 unk0C[0x12];
    u8 unk1E;
    u8 unk1F[0xA];
    u8 unk29;
};

struct Unk5C184Res {
    u8 unk00[0x10];
    u8 flags;
};

extern struct Unk5C184Kind *gSpawnMotionContext __asm__("D_8009F374");
extern s32 gGetEntitySpawnBank(struct Unk5C184Kind *object) __asm__("func_8003BF50");
extern s32 gSpawnTypeAtOwner(void *owner, u8 column, struct Unk5C184Msg *msg, s32 row,
                         s32 arg4) __asm__("func_800402DC");
void gSpawnBeamContactImpact(struct Unk5C184Vec *arg0, void *arg1, struct Unk5C184Node *node) __asm__("func_8005C184");


/**
 * @brief Rate-limits and spawns a contact impact object via func_800402DC, tagging the result by owner kind and team byte.
 *
 * Confirmed: decrements node->unk29 (+0x29) and returns early if signed result >= 0; else builds a stack message (unk49 = 5, unk00 = arg1, vec = *arg0), sets D_8009F374 = node->unk08, computes row via func_8003BF50, and calls func_800402DC(0, node->unk1E, &msg, flag, 0). Category Gameplay > Object Spawning | Gameplay > Combat follows func_800402DC table spawn and kind 1/0x12 team-flag tagging. Likely: reloading node->unk29 = 5 limits spawns to once per six calls, and +0xC1 == 0 vs non-zero selects team bit 0x8 vs 0x10. Unknown: meaning of unk49 = 5 and arg1.
 *
 * @param arg0 World position vector copied into msg.vec (+0x10).
 * @param arg1 Opaque payload pointer/value stored into msg.unk00 (+0x00).
 * @param node Driver node (+0x08 owner kind block, +0x1E spawn column, +0x29 cooldown counter).
 *
 * @par Globals
 * D_8009F374 - temporarily bound to node->unk08 across func_8003BF50/func_800402DC, then cleared to 0.
 *
 * @par Control flow
 * Early return on decremented unk29 >= 0; if func_800402DC returns non-null and unk08 exists, branches on kind == 1 (unkAC) vs 0x12 (unkB4->unkAC) to set flags 0x8 or 0x10.
 *
 * @par Calls
 * func_8003BF50 - classifies node->unk08 into spawn row 0 or 1. func_800402DC - spawns object for column node->unk1E.
 *
 * @par Side effects
 * Updates node->unk29 (decremented or reset to 5), temporarily writes D_8009F374, spawns an object, and may set bit 0x8 or 0x10 in res->flags.
 *
 * @par Callers
 * Confirmed: func_8005C344 (0x8005C4EC, 0x8005C55C, 0x8005C5F0 on segment raycast hits) / Likely: none / Unknown: none.
 */
void gSpawnBeamContactImpact(struct Unk5C184Vec *arg0, void *arg1, struct Unk5C184Node *node)
{
    struct Unk5C184Msg msg;
    struct Unk5C184Res *res;
    struct Unk5C184Kind *kind;
    struct Unk5C184Target *target;
    s32 flag;
    u8 count;

    count = node->unk29 - 1;
    node->unk29 = count;
    if ((s8)count >= 0) {
        return;
    }

    msg.unk49 = 5;
    msg.unk00 = arg1;
    msg.vec = *arg0;

    gSpawnMotionContext = node->unk08;
    flag = gGetEntitySpawnBank(node->unk08);
    res = gSpawnTypeAtOwner(0, node->unk1E, &msg, flag, 0);
    if (res != 0) {
        kind = node->unk08;
        if (kind != 0) {
            if (kind->kind == 1) {
                target = kind->unkAC;
                if (target != 0) {
                    if (target->unkC1 == 0) {
                        res->flags |= 0x8;
                    } else {
                        res->flags |= 0x10;
                    }
                }
            } else if (kind->kind == 0x12) {
                target = kind->unkB4->unkAC;
                if (target != 0) {
                    if (target->unkC1 == 0) {
                        res->flags |= 0x8;
                    } else {
                        res->flags |= 0x10;
                    }
                }
            }
        }
    }
    gSpawnMotionContext = 0;
    node->unk29 = 5;
}
