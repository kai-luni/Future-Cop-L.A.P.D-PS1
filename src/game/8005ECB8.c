#include "game/types.h"

struct Unk5EE04Vec {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk5EE04Ent {
    u8 unk00[0x14];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19[0x0B];
    u32 unk24;
    u8 unk28[0x50];
    struct Unk5EE04Vec unk78;
    u32 unk84;
};

struct Unk5EE04Obj {
    u16 unk00;
    s16 unk02;
    u16 unk04;
    s16 unk06;
    u8 unk08;
    u8 unk09;
    s16 unk0A;
    s16 unk0C;
    u8 unk0E;
    u8 unk0F[3];
    u8 unk12;
};

struct Unk5EE04Args {
    u32 flags;
    u8 kind;
    u8 unk05[3];
    struct Unk5EE04Vec position;
    u32 unk14;
    u16 unk18;
    u16 unk1A;
    u8 unk1C;
    u8 unk1D;
    u8 unk1E;
    u8 unk1F;
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    u32 unk24;
    struct Unk5EE04Vec unk28;
    u32 unk34;
    u32 unk38;
    u32 unk3C;
    u32 unk40;
    u32 unk44;
    u8 unk48;
    u8 unk49;
    u16 unk4A;
};

struct Unk5EE04Request {
    void (*handler)();
    u32 unk04;
    struct Unk5EE04Args *args;
    u32 unk0C;
};

extern void D_80037708();
extern void *func_8004D444(struct Unk5EE04Request *request);

void *func_8005ECB8(struct Unk5EE04Obj *obj, s32 **ptr, struct Unk5EE04Vec *pos,
                    struct Unk5EE04Ent *ent, u32 flags)
{
    struct Unk5EE04Request request;
    struct Unk5EE04Args args;

    request.handler = D_80037708;
    request.unk04 = 0;
    request.args = &args;
    request.unk0C = 0;

    args.flags = ent->unk14 | 0x8000;
    if (ent->unk16 == 0x12) {
        args.flags |= 0x4000;
    }
    args.unk1D = ent->unk17;
    args.unk1C = ent->unk18;
    args.unk14 = flags | 0x8;
    if (ent->unk24 & 0x100) {
        args.unk14 = flags | 0x108;
    } else {
        args.unk14 &= ~0x100;
    }
    args.kind = 0xF;
    args.unk18 = 0x3E8;
    args.unk1A = obj->unk00;
    args.unk38 = obj->unk02 << 4;
    args.unk1E = 0;
    args.unk22 = 0;
    args.unk23 = 0;
    args.unk1F = 0;
    args.unk34 = 0x10000;
    args.unk24 = **ptr;
    args.unk3C = -1;
    args.unk48 = obj->unk0E;
    args.unk21 = obj->unk12;
    args.unk40 = obj->unk0C;
    args.unk44 = (u32)ent;
    args.unk20 = 0;
    args.unk4A = obj->unk04;
    args.unk28.x = 0;
    args.position = *pos;
    args.unk28.y = 0;
    args.unk28.z = 0;

    return func_8004D444(&request);
}
