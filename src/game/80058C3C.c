#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk58C3CData {
    u8 unk00[0xAC];
    s8 unkAC;
};

struct Unk58C3CObject {
    u8 unk00[0x14];
    u16 tag;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u8 unk1A[0x92];
    struct Unk58C3CData *unkAC;
};

struct Unk58C3CArgs {
    u32 flags;
    u8 kind;
    u8 unk05[3];
    struct Vec3s32 position;
    s32 unk14;
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
    void *owner;
};

struct Unk58C3CSubArg {
    u8 unk00[0xC];
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    void *unk1C;
    u8 unk20;
    u8 unk21[3];
};

struct Unk58C3CRequest {
    void (*handler)();
    s32 unk04;
    struct Unk58C3CArgs *args;
    s32 unk0C;
};

extern void D_80036E6C();
extern void *D_8009F9E4;

extern void func_80017B60(s32 id);
extern s32 func_80019668(void);
extern void func_8005E624(s32, void *, void *, void *, s32);
extern void func_8004D444(struct Unk58C3CRequest *);

void func_80058C3C(void *owner, struct Vec3s32 *position, struct Unk58C3CObject *object)
{
    struct Unk58C3CRequest request;
    struct Unk58C3CArgs args;
    struct Unk58C3CSubArg sub;
    void *old;

    request.handler = D_80036E6C;
    request.unk04 = 0;
    request.args = &args;
    request.unk0C = 0;

    args.flags = object->tag | 0x8000;
    if (object->unk16 == 0x12) {
        args.flags |= 0x4000;
    }
    args.unk1D = object->unk17;
    args.unk1C = object->unk18;
    args.unk14 = 9;
    args.kind = 7;
    args.unk18 = 1;
    args.unk1A = 0;
    args.unk1E = 0;
    args.unk22 = 0;
    args.unk23 = 0;
    sub.unk10 = 0;
    args.unk1F = 0;
    sub.unk0C = 0xA000;
    args.owner = owner;
    sub.unk14 = 0;
    sub.unk20 = 0;
    sub.unk18 = 0;
    sub.unk1C = object;
    args.unk20 = 0;
    args.position = *position;

    old = D_8009F9E4;
    func_80017B60(object->unkAC->unkAC);
    func_8005E624((func_80019668() & 0xF) | 0xE0, &sub, position,
                  D_8009F9E4, sub.unk0C);
    D_8009F9E4 = old;
    func_8004D444(&request);
}
