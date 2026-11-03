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

extern struct Unk5C184Kind *D_8009F374;
extern s32 func_8003BF50(struct Unk5C184Kind *object);
extern s32 func_800402DC(void *owner, u8 column, struct Unk5C184Msg *msg, s32 row,
                         s32 arg4);

void func_8005C184(struct Unk5C184Vec *arg0, void *arg1, struct Unk5C184Node *node)
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

    D_8009F374 = node->unk08;
    flag = func_8003BF50(node->unk08);
    res = func_800402DC(0, node->unk1E, &msg, flag, 0);
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
    D_8009F374 = 0;
    node->unk29 = 5;
}
