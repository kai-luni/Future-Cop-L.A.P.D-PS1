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

extern s32 func_80069D68(s32 script, s32 arg1, s32 arg2, struct Entity *entity);
extern void func_8004D958(struct Entity *entity);

s32 func_80031C74(struct Unk31C74Node *node, struct Entity *other)
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
        func_80069D68(node->script, 0, other->unk14 & 0x7FFF, 0);
    }

    if (other->unk16 != 1 && (node->flags & 1)) {
        other->flags |= 0x4000000;
        func_8004D958(other);
    }

    { u32 t = node->flags & 2; return t == 0; }
}
