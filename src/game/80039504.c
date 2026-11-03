#include "game/types.h"

struct Unk39504Sub04 {
    u8 pad00[0x20];
    s32 unk20;
};

struct Unk39504Sub08 {
    u8 pad00[0x48];
    s16 unk48;
    s16 unk4A;
    union {
        s32 unk4C_word;
        struct {
            u8 unk4C;
            u8 unk4D;
        } b;
    } u;
};

struct Unk39504Other {
    s32 unk00;
    struct Unk39504Sub04 *unk04;
    struct Unk39504Sub08 *unk08;
};

struct Unk39504Child {
    u8 pad00[0x24];
    s32 unk24;
};

struct Unk39504Self {
    u8 pad00[0xA0];
    s32 unkA0;
    u8 padA4[0xB4];
    struct Unk39504Child *unk158;
};

extern void func_800382B4(s32 arg0, struct Unk39504Self *self, struct Unk39504Other *other, s32 arg3);
extern struct Unk39504Child *func_80039418(struct Unk39504Self *self, s32 arg1, s16 arg2, s32 arg3, u8 arg4, u8 arg5, s32 arg6);
extern void func_8003FA34(struct Unk39504Child *child);

void func_80039504(s32 arg0, struct Unk39504Self *self, struct Unk39504Other *other, s32 arg3)
{
    struct Unk39504Sub08 *data;
    s32 flags;

    func_800382B4(arg0, self, other, arg3);
    data = other->unk08;
    flags = ((data->u.unk4C_word & 6) == 2) << 1;
    self->unk158 = func_80039418(self, data->unk48 << 8, data->unk4A, flags, data->u.b.unk4C, data->u.b.unk4D, other->unk04->unk20);
    self->unkA0 = 0;
    self->unk158->unk24 |= 0x10;
    func_8003FA34(self->unk158);
}
