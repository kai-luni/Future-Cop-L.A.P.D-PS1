#include "game/types.h"

struct Vec3s32 { s32 x; s32 y; s32 z; };
struct Unk5EE04Obj {
    u16 unk00; s16 unk02; u8 unk04; u8 unk05; s16 unk06; u8 unk08;
    u8 unk09; s16 unk0A; s16 unk0C; u8 unk0E; u8 unk0F[3]; u8 unk12;
};
struct Unk5EE04Ent {
    u8 unk00[0x14]; u16 unk14; u8 unk16; u8 unk17; u8 unk18; u8 unk19[0x0B]; u32 unk24;
};
struct Sub34 { u8 pad[0x14]; s16 unk14; };
struct SubChild { u8 pad[0x34]; struct Sub34 *unk34; };
struct UnkArgs {
    u32 flags; u8 kind; u8 unk05[3]; struct Vec3s32 position; u32 unk14;
    u16 unk18; u16 unk1A; u8 unk1C; u8 unk1D; u8 unk1E; u8 unk1F;
    u8 unk20; u8 unk21; u8 unk22; u8 unk23; void *owner;
};
struct UnkSubArg {
    u8 unk00[0xC]; s32 unk0C; s32 unk10; s32 unk14; s32 unk18; void *unk1C; u8 unk20; u8 unk21[3];
};
struct UnkRequest { void (*handler)(); s32 unk04; struct UnkArgs *args; s32 unk0C; };
extern void D_80036E6C();
extern void func_8005E624(s32 a0, void *a1, struct Vec3s32 *a2, struct Vec3s32 *a3, s32 a4);
extern void *func_8004D444(struct UnkRequest *request);

void *func_8005E9E4(u8 a0, struct Unk5EE04Obj *obj, struct SubChild ***ptr,
                    struct Vec3s32 *pos, struct Unk5EE04Ent *ent, s32 a5,
                    struct Vec3s32 *a6, u32 flags, u8 a8)
{
    struct UnkRequest request;
    struct UnkArgs args;
    struct UnkSubArg sub;
    struct SubChild **p;

    request.handler = D_80036E6C;
    request.unk04 = 0;
    request.args = &args;
    request.unk0C = 0;

    sub.unk21[0] = a0;
    sub.unk21[1] = a8;

    args.flags = ent->unk14 | 0x8000;
    if (ent->unk16 == 0x12) {
        args.flags |= 0x4000;
    }

    args.unk1D = ent->unk17;
    args.unk1C = ent->unk18;
    args.unk14 = flags | 8;
    if (ent->unk24 & 0x100) {
        args.unk14 = flags | 0x108;
    } else {
        args.unk14 &= ~0x100;
    }

    args.kind = 7;
    args.unk18 = 0x28;
    args.unk1A = obj->unk00;
    args.unk1E = 0;
    args.unk22 = 0;
    args.unk23 = 0;
    sub.unk10 = obj->unk02 << 4;
    sub.unk0C = obj->unk0A << 4;
    args.unk1F = 0;

    p = *ptr;
    args.owner = *p;

    sub.unk14 = obj->unk12;
    sub.unk20 = obj->unk0E;
    sub.unk1C = ent;
    args.unk20 = 0;
    sub.unk18 = a5;

    pos->y += (*p)->unk34->unk14 << 4;
    args.position = *pos;

    func_8005E624(obj->unk06 << 4, &sub, pos, a6, sub.unk0C);
    return func_8004D444(&request);
}
