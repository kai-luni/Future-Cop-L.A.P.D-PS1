#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Def {
    u8 pad00[0x24];
    u16 unk24;
    s16 unk26;
    u8 unk28;
    u8 unk29;
    u16 unk2A;
    s16 unk2C;
    u8 unk2E;
    u8 pad2F;
    u16 unk30;
    u16 unk32;
    u16 unk34;
};

struct Arg {
    u8 pad00[8];
    struct Def *def;
};

struct Entity {
    u8 pad00[0x16];
    u8 unk16;
    u8 pad17[0x0D];
    u32 flags;
    u8 pad28[0x28];
    struct Vec3 unk50;
    struct Vec3 unk5C;
    s32 unk68;
    s16 unk6C;
    s16 unk6E;
    u8 pad70[2];
    u8 flags72;
    u8 pad73[0x2D];
    u8 unkA0;
    u8 unkA1;
    s16 unkA2;
    s32 unkA4;
    void *unkA8;
    s32 unkAC;
    s8 unkB0;
    s8 unkB1;
    s16 unkB2;
    s16 unkB4;
    s16 unkB6;
};

extern s32 func_8002AD58(s32 message, struct Entity *self, struct Arg *arg, s32 arg3);
extern s32 func_80095900(s32 arg0);
extern s32 func_80067684(struct Vec3 *pos, s32 mode);

s32 func_80030DE0(struct Entity *self, struct Arg *arg, s32 arg2)
{
    struct Def *def;
    s32 s1;
    s32 s0;
    s32 mode;
    s32 type;

    func_8002AD58(2, self, arg, arg2);
    def = arg->def;

    self->unk68 = (s16)def->unk24 << 4;
    self->unk6E = self->unk6C = def->unk24;
    self->unkA1 = def->unk29;
    self->unkA2 = def->unk2A;
    self->unkA0 = def->unk28;
    self->unkA4 = 0;

    if (self->unk16 == 0x26) {
        self->unkA8 = 0;
        self->unkAC = def->unk2C;
        self->unkB0 = def->unk2E - 1;
        self->unkB1 = 1;
        self->unkB2 = (s16)def->unk32 / 2;
        self->unkB4 = (s16)def->unk34 / 2;
        self->unkB6 = (s16)def->unk30 / 2;
        s1 = (self->unkB2 * self->unkB2 + self->unkB6 * self->unkB6) >> 8;
        s0 = s1 + ((self->unkB4 * self->unkB4) >> 8);
        s1 = func_80095900(s1) << 2;
        s0 = func_80095900(s0) << 2;
        self->unk68 = s0;
        self->unk6E = s1 >> 4;
        self->unk6C = (s16)def->unk34 / 2;
    }

    if (self->flags & 0x80000) {
        self->flags72 &= ~0x10;
    }

    type = def->unk26;
    if (type != 0) {
        if (type == 1) {
            mode = -2;
        } else {
            mode = type - 2;
        }
    } else {
        mode = -1;
    }

    self->unk50.y = self->unk5C.y = func_80067684(&self->unk50, mode);

    self->flags |= 0xA00011;
    if (self->unkA0 & 8) {
        self->unkA1 = 7;
    }

    return 0;
}
