// Categories: Gameplay > Trigger Volume | Gameplay > Entity Update
#include "game/types.h"

struct Unk31C74Ptr {
    u8 unk00[0x40];
    s32 unk40;
};

struct Unk31C74Node {
    s16 x;
    s16 y;
    s16 z;
    s16 unk06;
    s16 unk08;
    s16 unk0A;
    s16 extentX;
    s16 extentY;
    s16 extentZ;
    s16 unk12;
    u8 flags;
    u8 unk15;
    u16 unk16;
    s32 script;
};

struct Entity {
    u8 unk00[0x14];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18[0x0C];
    u32 flags;
    u8 unk28[0x28];
    s32 posX;
    s32 posY;
    s32 posZ;
    u8 unk5C[0x0C];
    s32 radius;
    u8 unk6C[0x40];
    struct Unk31C74Ptr *ptrAC;
};

extern s32 gInterpretEntityScript(s32 script, s32 arg1, s32 arg2, struct Entity *entity) __asm__("func_80069D68");
extern void gRetireEntity(struct Entity *entity) __asm__("func_8004D958");
s32 gTestMapZoneTrigger(struct Unk31C74Node *node, struct Entity *other) __asm__("func_80031C74");


/**
 * @brief Tests an entity against a trigger box and fires the attached script.
 *
 * Each axis is checked separately: the node coordinate scaled by 256 minus the
 * entity position, reduced by the entity radius and the node extent
 * (extent << 4), must stay below zero. As soon as one axis misses, the function
 * returns 0. A type-1 entity that is busy (its partner block has a non-zero
 * 0x40) is ignored when the node has flag 4. On a hit the script at 0x1C is
 * run with the entity type as argument; entities of any type other than 1 are
 * also retired when the node carries flag 1. The return value is the negation
 * of flag 2, i.e. 1 for a normal trigger and 0 for a "stay" trigger.
 *
 * @param node Trigger node: position, extents, flags and script pointer.
 * @param other Entity entering the volume.
 * @return 1 when the volume should be released, 0 while the entity stays.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Three guard exits followed by the script call and the retire branch.
 *
 * @par Calls
 * func_80069D68 - trigger script. func_8004D958 - retires the entity.
 *
 * @par Side effects
 * Runs a script and may retire the entity.
 *
 * @par Callers
 * Likely: func_80031DE8 (same block).
 */
s32 gTestMapZoneTrigger(struct Unk31C74Node *node, struct Entity *other)
{
    s32 delta;
    s32 sign;

    delta = (node->x << 8) - other->posX;
    sign = delta >> 31;
    if (((sign + delta) ^ sign) - other->radius - (node->extentX << 4) >= 0) {
        return 0;
    }

    delta = (node->z << 8) - other->posZ;
    sign = delta >> 31;
    if (((sign + delta) ^ sign) - other->radius - (node->extentZ << 4) >= 0) {
        return 0;
    }

    delta = (node->y << 8) - other->posY;
    sign = delta >> 31;
    if (((sign + delta) ^ sign) - other->radius - (node->extentY << 4) >= 0) {
        return 0;
    }

    if (other->unk16 == 1 && (node->flags & 4) && other->ptrAC->unk40 != 0) {
        return 0;
    }

    if (node->script != 0) {
        gInterpretEntityScript(node->script, 0, other->unk14 & 0x7FFF, 0);
    }

    if (other->unk16 != 1 && (node->flags & 1)) {
        other->flags |= 0x4000000;
        gRetireEntity(other);
    }

    { u32 t = node->flags & 2; return t == 0; }
}
