#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Object {
    u8 unk00[0x14];
    u16 tag;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u16 unk1A;
    u16 unk1C;
    u8 unk1E[6];
    u32 flags;
    u8 coord[0x6A];
    u8 unk92;
    u8 unk93;
};

struct Args {
    s32 tag;
    u8 kind;
    u8 pad05[3];
    struct Vec3s32 position;
    u32 flags;
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
    s32 unk24;
    s16 unk28;
    u8 unk2A;
    u8 unk2B;
    s32 unk2C;
    void *owner;
};

struct Request {
    void (*handler)();
    s32 unk04;
    struct Args *args;
    s32 unk0C;
};

extern void func_80039298();
extern void func_80017668(u8 *coord, s32 part, struct Vec3s32 *out);
extern void *func_8004D444(struct Request *request);

void *func_80039418(struct Object *object, s32 arg1, s16 arg2, s32 arg3, u8 arg4, u8 arg5, s32 arg6)
{
    struct Request request;
    struct Args args;

    request.handler = func_80039298;
    request.args = &args;
    request.unk04 = 0;
    request.unk0C = 0;
    args.kind = 0x15;
    args.unk2C = arg6;
    args.unk2B = arg4;
    args.tag = object->tag;
    args.unk1D = object->unk17;
    args.unk1C = object->unk18;
    args.unk1E = 0;
    args.flags = object->flags & 0x7FFFDFFF;
    args.unk18 = object->unk1C;
    args.unk1A = object->unk1A;
    args.unk24 = arg1;
    args.owner = object;
    args.unk28 = arg2;
    args.unk2A = arg3;
    args.unk20 = arg5;
    args.unk21 = 0;
    args.unk1F = object->unk19 & 0xF;
    args.unk22 = object->unk92;
    args.unk23 = object->unk93;
    func_80017668(object->coord, arg3, &args.position);
    return func_8004D444(&request);
}
