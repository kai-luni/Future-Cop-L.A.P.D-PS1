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

extern s32 D_8009F9DC;

extern struct Unk4C0C0Node *func_8004AAD4(struct Unk4C0C0Owner *owner,
                                           struct Unk4C0C0Node *previous,
                                           struct Unk4C0C0Node *node);
extern s32 func_800402DC(void *owner, u8 column, void *arg2, s32 row, s32 arg4);

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
            D_8009F9DC--;
            if (func_8004AAD4(owner, previous, node) == 0) {
                owner->unk0F |= 1;
            }
            if ((D_8009F9DC & 3) >= 2) {
                if (owner->unk64 != 0) {
                    if (node->flags & 0x80) {
                        loc.vec = node->u.parent->vec;
                        loc.unk00 = node->u.parent->unk3C << 4;
                    } else {
                        loc.vec = node->u.vec;
                        loc.unk00 = node->unk46 << 4;
                    }
                    loc.unk49 = 5;
                    func_800402DC(0, owner->unk64, &loc, (owner->unk10 >> 1) & 1, 0);
                }
            }
        } else {
            previous = node;
        }
        node = next;
    }
}
