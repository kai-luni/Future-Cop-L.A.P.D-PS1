#include "game/types.h"

struct Vec { s32 x, y, z; };
struct Obj {
    u16 unk00; s16 unk02; u16 unk04; u16 unk06; s16 unk08;
    s16 unk0A; s16 unk0C; u8 unk0E; u8 pad[3]; u8 unk12;
};
struct Ent {
    u8 pad[0x14]; u16 unk14; u8 unk16, unk17, unk18;
    u8 pad19[0xB]; u32 unk24;
};
struct Args {
    u32 flags; u8 kind; u8 pad05[3]; struct Vec position; u32 unk14;
    u16 unk18, unk1A; u8 unk1C, unk1D, unk1E, unk1F;
    u8 unk20, unk21, unk22, unk23; void *unk24; void *unk28;
    s32 unk2C, unk30; void *unk34; struct Ent *unk38;
    s32 unk3C, unk40, unk44; s16 unk48, unk4A;
    u8 unk4C, unk4D; u16 unk4E; u8 unk50, unk51; u8 pad52[2]; s32 unk54; u8 pad58[8];
};
struct Request { void (*handler)(); s32 unk04; struct Args *args; s32 unk0C; };
extern void D_80036624();
extern void *func_8004D444(struct Request *);

void *func_8005E47C(struct Obj *obj, void *a1, struct Vec *pos, void *a3,
                    struct Ent *ent, void *a5, u32 flags, u8 a7, u8 a8,
                    u16 a9, u8 a10, s32 a11)
{
    struct Request request;
    struct Args args;
    s16 value;

    request.handler = D_80036624;
    request.unk04 = 0;
    request.args = &args;
    request.unk0C = 0;
    args.kind = 2;
    args.unk54 = a11;
    args.flags = ent->unk14 | 0x8000;
    if (ent->unk16 == 0x12) args.flags |= 0x4000;
    args.unk1D = ent->unk17;
    args.unk1C = ent->unk18;
    args.unk1E = 0;
    args.unk22 = 0;
    args.unk23 = 0;
    args.unk1F = 0;
    args.unk4D = a7;
    args.unk50 = a8;
    args.unk4E = a9;
    args.unk51 = a10;
    args.unk14 = flags | 8;
    if (ent->unk24 & 0x100) args.unk14 = flags | 0x108;
    else args.unk14 &= ~0x100;
    args.unk18 = 0x28;
    args.position = *pos;
    args.unk28 = a3;
    args.unk24 = a1;
    value = obj->unk06;
    args.unk48 = value;
    if (obj->unk08) {
        args.unk2C = value << 1;
        args.unk4A = obj->unk08;
    } else {
        args.unk2C = value << 4;
        args.unk4A = 0;
    }
    args.unk1A = obj->unk00;
    args.unk3C = obj->unk0A << 4;
    args.unk34 = a5;
    args.unk38 = ent;
    args.unk40 = obj->unk12;
    args.unk4C = obj->unk0E;
    args.unk20 = 0;
    if (a5) args.unk30 = 1;
    else args.unk30 = 0;
    args.unk44 = obj->unk02 << 4;
    return func_8004D444(&request);
}
