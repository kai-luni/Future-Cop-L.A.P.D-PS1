#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Entity {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0x18];
    s32 unk24;
    u8 unk28[0x04];
    s32 unk2C[9];
    struct Vec3 unk50;
    u8 unk5C[0x18];
    void *unk74;
};

struct Unk31690Args {
    s32 unk00;
    u8 unk04;
    u8 unk05[3];
    struct Vec3 unk08;
    s32 unk14;
    s16 unk18;
    s16 unk1A;
    u8 unk1C;
    u8 unk1D;
    u8 unk1E;
    u8 unk1F;
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    void *unk24;
    void *unk28;
    s32 unk2C;
};

struct Unk31690Request {
    void (*handler)(void);
    s32 unk04;
    struct Unk31690Args *args;
    s32 unk0C;
};

struct Unk31690Sub {
    s32 unk00;
    u8 unk04[0x19];
    u8 unk1D;
    u8 unk1E;
    u8 unk1F[3];
    u8 unk22;
    u8 unk23[5];
};

extern void D_8003190C(void);
extern u8 func_80083A10(struct Unk31690Sub *arg0, void *arg1, s32 arg2, void *arg3);
extern s32 func_8004D4D4(struct Unk31690Request *request);

s32 func_80031690(struct Entity *self)
{
    struct Unk31690Request request;
    struct Unk31690Args args;
    struct Unk31690Sub sub;
    s32 range;

    if (self->unk24 & 0x1000) {
        sub.unk1D = 0;
        sub.unk1E = 0;
        sub.unk22 = 0;
        sub.unk00 = self->unk08 + 0x10000;
        range = (self->unk24 & 0x800) ? -1 : 0x4650;
        if (func_80083A10(&sub, self->unk74, range, self->unk2C) != 0) {
            return -1;
        }
    }

    request.handler = D_8003190C;
    request.unk04 = 0;
    request.args = &args;
    request.unk0C = 0;
    args.unk04 = 0x17;
    args.unk21 = 0;
    args.unk20 = 0;
    args.unk00 = 0;
    args.unk1D = 0;
    args.unk1E = 0;
    args.unk1F = 0;
    args.unk22 = 0;
    args.unk23 = 0;
    args.unk18 = 0;
    args.unk1A = 0;
    args.unk24 = self->unk74;
    args.unk28 = self->unk2C;
    args.unk14 = 0xD1;
    if (self->unk24 & 0x1000) {
        args.unk14 = 0xD5;
    } else {
        args.unk14 = 0xD1;
    }
    args.unk08 = self->unk50;
    return func_8004D4D4(&request);
}
