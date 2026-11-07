#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Sub {
    u8 pad00[0x24];
    s32 unk24;
    s32 unk28;
    s16 unk2C;
    s8 unk2E;
    u8 unk2F;
    u8 unk30;
    u8 unk31;
    u16 unk32;
    u16 unk34;
    s16 unk36;
};

struct Spawn {
    u8 pad00[8];
    struct Sub *unk08;
};

struct Entity {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[0x28];
    s32 unk50;
    s32 unk54;
    u8 pad58[8];
    s32 unk60;
    u8 pad64[4];
    s32 unk68;
    u16 unk6C;
    u16 unk6E;
    u8 pad70[2];
    u8 unk72;
    u8 pad73[0x2D];
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
    u8 unkAC;
    u8 unkAD;
    u16 unkAE;
};

extern s32 func_8002AD58(s32 message, struct Entity *self, struct Spawn *data, s32 arg3);
extern s32 func_80067684(struct Vec3 *pos, s32 mode);

s32 func_80030AD4(struct Entity *self, struct Spawn *data, s32 arg2)
{
    struct Sub *sub;
    s32 type;
    s32 mode;
    s32 value;
    u16 size;

    func_8002AD58(2, self, data, arg2);
    sub = data->unk08;

    self->unkA8 = sub->unk24 << 1;
    self->unkA0 = sub->unk28 << 1;
    self->unkA4 = 0;

    type = sub->unk2E;
    if (type != 0) {
        if (type == 1) {
            mode = -2;
        } else {
            mode = type - 2;
        }
    } else {
        mode = -1;
    }
    self->unkA4 = func_80067684((struct Vec3 *)&self->unkA0, mode) + (sub->unk2C << 4);

    type = sub->unk2F;
    if (type != 0) {
        if (type == 1) {
            mode = -2;
        } else {
            mode = type - 2;
        }
    } else {
        mode = -1;
    }
    value = func_80067684((struct Vec3 *)&self->unk50, mode) + (sub->unk36 << 4);
    self->unk60 = value;
    self->unk54 = value;

    size = sub->unk34;
    self->unk6C = size;
    self->unk6E = size;
    self->unk68 = (s16)sub->unk34 << 4;
    self->unkAD = sub->unk31;
    self->unkAE = sub->unk32;

    if (self->flags & 0x80000) {
        self->unk72 &= 0xEF;
    }

    self->unkAC = sub->unk30;
    self->flags |= 0xA00011;

    if ((*(u32 *)&self->unkAC & 0xFFFFFF00) == 0x10100) {
        self->unkAD = 8;
    }
    return 0;
}
