#include "game/types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

struct Unk5CB80Node {
    struct Unk5CB80Node *next;
    void *first;
    void *second;
    u8 unk0C[0x1C];
    u8 flags;
};

struct Unk5CB80Pool {
    void *storage;
    struct Unk5CB80Node *active;
    struct Unk5CB80Node *free;
};

struct ListNode {
    struct ListNode *previous;
    struct ListNode *next;
    struct ListNode *tail;
};

struct Unk5CBCCNode {
    struct Unk5CBCCNode *next;
    void *first;
    void *second;
    u8 unk0C[4];
    void *unk10;
    void *unk14;
    s32 unk18;
    u16 unk1C;
    u8 unk1E;
    u8 unk1F;
    struct Unk5CBCCNode *self;
    void (*callback)(void);
    u8 flags;
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    Vec3 vec;
};

struct Unk5CBCCLocal {
    u8 unk00[0x10];
    Vec3 vec;
    s32 out[9];
    u8 unk40[9];
    u8 flag;
    u8 unk4A[6];
    Vec3 temp;
};

struct Unk5CBCCArg0 {
    u8 unk00[8];
    void *unk08;
};

struct Unk5CBCCArg2 {
    u8 unk00[8];
    void *unk08;
    u8 unk0C[0x50];
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    u8 unk68[0x10];
    u8 unk78[0xC];
    s32 unk84;
};

struct Unk5CBCCArg5 {
    u16 unk00;
    u8 unk02[4];
    s16 unk06;
    u8 unk08[2];
    s16 unk0A;
    u8 unk0C[2];
    u8 unk0E;
    u8 unk0F;
    u8 unk10[3];
    u8 unk13;
};

struct ModelInstance {
    void *model;
    u8 coord[0x44];
    s16 frame;
};

struct Unk402DCOwner {
    u8 unk00[8];
    s32 unk08;
};

extern struct Unk5CB80Pool D_800A22B0;
extern u8 D_800A6578[];

extern struct Unk5CB80Node *func_8005CB80(struct Unk5CB80Pool *pool, void *target);
extern struct ListNode *func_8003BEF0(struct ListNode *node);
extern void func_80017668(struct ModelInstance *instance, s32 part, s32 *out);
extern void func_8005E2A0(s32 *a, s32 *b, s32 c, s32 d, void *e, void *f, s32 g);
extern void func_8001A180(s32 *vec);
extern void func_800193F8(s32 *a, s32 *b);
extern s32 func_8003BF50(struct Unk5CBCCArg0 *arg0);
extern s32 func_800402DC(struct Unk402DCOwner *owner, u8 column, s32 arg2, s32 row, s32 arg4);
extern void D_8005CA04(void);

void func_8005CBCC(struct Unk5CBCCArg0 *arg0, s32 arg1, struct Unk5CBCCArg2 *arg2,
                   struct ModelInstance *arg3, s32 partin, struct Unk5CBCCArg5 *arg5)
{
    struct Unk5CBCCLocal local;
    struct Unk5CBCCNode *node;
    s32 row;
    u8 *p;
    u8 *base;
    register s32 tmp __asm__("$2");
    register struct Unk5CBCCArg0 *owner __asm__("$3");
    struct Unk5CB80Pool *pool = &D_800A22B0;
    register s32 part __asm__("$18") = partin;

    node = (struct Unk5CBCCNode *)func_8005CB80(pool, arg0);
    if (node == 0) {
        local.flag = 0xC;
        if (arg3 == 0) {
            return;
        }

        tmp = part << 24;
        part = tmp >> 24;
        if (part < 0) {
            return;
        }

        node = (struct Unk5CBCCNode *)func_8003BEF0((struct ListNode *)pool);
        if (node == 0) {
            return;
        }

        func_80017668(arg3, part, &local.vec.x);
        node->vec = local.vec;

        if (arg2 != 0) {
            if (arg5->unk06 > 0) {
                func_8005E2A0(&local.vec.x, &local.temp.x, arg5->unk06 << 4,
                              arg5->unk0A << 4, &arg2->unk5C, arg2->unk78, arg2->unk84);
            } else {
                local.temp.x = arg2->unk5C - local.vec.x;
                local.temp.y = arg2->unk60 - local.vec.y;
                local.temp.z = arg2->unk64 - local.vec.z;
                func_8001A180(&local.temp.x);
            }
            func_800193F8(&local.temp.x, local.out);
        } else {
            s32 first;
            first = ((s32 *)arg3->coord)[0];
            local.out[1] = ((s32 *)arg3->coord)[1];
            local.out[2] = ((s32 *)arg3->coord)[2];
            local.out[3] = ((s32 *)arg3->coord)[3];
            local.out[4] = ((s32 *)arg3->coord)[4];
            local.out[5] = ((s32 *)arg3->coord)[5];
            local.out[6] = ((s32 *)arg3->coord)[6];
            local.out[7] = ((s32 *)arg3->coord)[7];
            local.out[8] = ((s32 *)arg3->coord)[8];
            local.out[0] = first;
        }

        row = func_8003BF50(arg0);
        base = D_800A6578;
        p = &base[(arg5->unk0F << 2) + (row << 10)];
        *p &= 0xFB;

        row = func_8003BF50(arg0);
        p = &base[(arg5->unk0F << 2) + (row << 10)];
        *p |= 1;

        node->self = node;
        node->callback = D_8005CA04;

        if (func_800402DC(0, arg5->unk0F, local.flag ? (s32)&local : 0, row,
                          (s32)&node->self) == 0) {
            node->flags = 1;
            return;
        }

        __asm__("" : "=r"(owner) : "0"(arg0));
        node->unk2B = arg5->unk13;
        node->unk1C = arg5->unk00;
        node->second = arg0;
        node->unk1E = arg5->unk0E;
        node->flags = 0;
        node->unk2A = 0;
        if (owner != 0) {
            node->unk14 = owner->unk08;
        }
        node->unk29 = 0;
    }

    node->unk18 = 0x96;
    node->first = arg2;
    if (arg2 != 0) {
        node->unk10 = arg2->unk08;
    }
}
