#include "game/types.h"

struct Model;

struct ModelInstance {
    struct Model *model;
    u8 coord[0x30];
    s32 x;
    s32 y;
    s32 z;
    u8 unk40[8];
    s16 frame;
};

struct Unk402DCOwner {
    u8 unk00[8];
    s32 unk08;
};

struct Unk5BDE4Arg2 {
    u8 unk00[8];
    void *unk08;
};

struct Unk5BDE4Source {
    u16 unk00;
    u8 unk02[0xC];
    u8 unk0E;
    u8 unk0F;
};

struct Unk5BDE4Param {
    u8 unk00[0x44];
    struct ModelInstance *instance;
    u8 part;
    u8 flags;
    u8 pad4[4];
};

struct Unk5BDE4Vec {
    s32 x;
    s32 y;
    s32 z;
};

struct DestinationTriple {
    s32 second;
    s32 zero;
    s32 first;
};

struct Unk5BCD4Node {
    struct Unk5BCD4Node *next;
    void *first;
    void *second;
    s32 unk0C;
    void *unk10;
    s32 unk14;
    s32 unk18;
    u16 unk1C;
    u8 unk1E;
    u8 unk1F;
    struct Unk5BCD4Node *self;
    s32 unk24;
    u8 unk28[2];
    u8 flags;
    u8 unk2B;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
};

struct Unk5BCD4Pool {
    void *storage;
    struct Unk5BCD4Node *active;
    struct Unk5BCD4Node *free;
};

struct ListNode {
    struct ListNode *previous;
    struct ListNode *next;
    struct ListNode *tail;
};

extern s32 D_8009FA0C[];
extern struct Unk5BCD4Pool D_800A22A0;
extern u8 D_800A6578[];
extern u8 D_8005B934[];

extern void func_80069580(u32 index, struct DestinationTriple *destination);
extern s32 func_8001A100(s32 *vector);
extern struct Unk5BCD4Node *func_8005BCD4(struct Unk5BCD4Pool *pool, void *target);
extern struct ListNode *func_8003BEF0(struct ListNode *node);
extern void func_80017668(struct ModelInstance *instance, s32 part, s32 *out);
extern s32 func_8003BF50(struct Unk402DCOwner *owner);
extern s32 func_800402DC(struct Unk402DCOwner *owner, u8 column, s32 arg2, s32 row, s32 arg4);

struct Locals5BDE4 {
    struct DestinationTriple local;
    u8 pad24[4];
    struct Unk5BDE4Vec diff;
    u8 pad34[4];
    struct Unk5BDE4Param param;
};

void func_8005BDE4(struct Unk402DCOwner *owner, s32 arg1, struct Unk5BDE4Arg2 *arg2,
                   struct ModelInstance *instance, u8 part, struct Unk5BDE4Source *in_source)
{
    register struct Unk5BDE4Source *source __asm__("$21") = in_source;
    struct Locals5BDE4 scratch;
    struct Unk5BCD4Pool *pool;
    struct Unk5BCD4Node *node;
    s32 i;
    s32 row;
    u32 found;

    __asm__("" : : "r"(source) : "$2");
    for (i = 0; i < D_8009FA0C[0]; i++) {
        func_80069580(i, &scratch.local);
        scratch.diff.x = instance->x - scratch.local.second;
        scratch.diff.y = instance->y - scratch.local.zero;
        scratch.diff.z = instance->z - scratch.local.first;
        if (func_8001A100((s32 *)&scratch.diff) <= 0x1FFFF) {
            found = 1;
        }
        __asm__ volatile("" ::: "memory");
    }

    if (found & 0xFF) {
        pool = &D_800A22A0;
        node = func_8005BCD4(pool, owner);
        if (node == 0) {
            node = (struct Unk5BCD4Node *)func_8003BEF0((struct ListNode *)pool);
            if (node == 0) {
                return;
            }

            node->second = owner;
            node->unk0C = arg1;
            node->first = arg2;
            if (node->second != 0) {
                node->unk14 = ((struct Unk402DCOwner *)node->second)->unk08;
            }
            if (arg2 != 0) {
                node->unk10 = arg2->unk08;
            }

            scratch.param.flags = 0;
            if (instance != 0) {
                scratch.param.instance = instance;
                scratch.param.flags = 0x40;
            }

            if ((s8)part >= 0) {
                scratch.param.part = part;
                scratch.param.flags |= 0x20;
                func_80017668(instance, (s8)part, &node->unk2C);
            }

            row = func_8003BF50(owner);
    {
        u8 *p = D_800A6578;
        p[(source->unk0F << 2) + (row << 10)] &= 0xFB;
    }

            node->unk24 = (s32)D_8005B934;
            node->self = node;
            node->unk18 = 0x32;

            if (func_800402DC(owner, source->unk0F,
                              scratch.param.flags ? (s32)&scratch.param : 0, row,
                              (s32)&node->self) == 0) {
                node->flags = 1;
                return;
            }

            node->unk1C = source->unk00;
            node->unk1E = source->unk0E;
            node->flags = 0;
        } else {
            node->unk18 = 0x32;
            node->first = arg2;
            if (arg2 != 0) {
                node->unk10 = arg2->unk08;
            }
        }
    }
}
