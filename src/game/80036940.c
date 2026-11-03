#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Matrix3x3 {
    s32 m[3][3];
};

struct ModelInstance {
    void *model;
    struct Matrix3x3 matrix;
};

struct EntityLink {
    u8 pad[8];
    s32 unk08;
    u8 pad0C[0x50];
    struct Vec3s32 unk5C;
};

struct UnkChild {
    u8 pad[8];
    s32 unk08;
};

struct EntityData {
    u8 pad00[8];
    struct Vec3s32 unk08;
    u8 pad14[4];
    u16 unk18;
    u16 unk1A;
    u8 pad1C[8];
    void *unk24;
    struct Vec3s32 unk28;
    s32 unk34;
    s32 unk38;
    u8 unk3C;
    u8 pad3D[3];
    struct UnkChild *unk40;
    struct EntityLink *unk44;
    u8 unk48;
    u8 unk49;
    u8 unk4A;
};

struct EntitySpawn {
    u8 pad[8];
    struct EntityData *unk08;
};

struct Entity {
    u8 pad00[0x1A];
    u16 unk1A;
    u16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 unk21;
    u8 pad22[2];
    u32 flags;
    struct ModelInstance unk28;
    struct Vec3s32 unk50;
    u8 pad5C[0x44];
    struct Vec3s32 unkA0;
    s32 unkAC;
    s32 unkB0;
    u8 padB4_pad[0];
    u8 unkB4;
    u8 unkB5;
    u8 padB6[2];
    s32 unkB8;
    struct UnkChild *unkBC;
    struct EntityLink *unkC0;
    s32 unkC4;
    struct Vec3s32 unkC8;
};

extern void func_8002AD58(s32 message, struct Entity *self, struct EntitySpawn *other, s32 arg3);
extern void func_80013BD8(struct ModelInstance *instance, void *model, s32 options);

void func_80036940(struct Entity *self, struct EntitySpawn *other, s32 arg2)
{
    struct EntityData *data;

    func_8002AD58(2, self, other, arg2);
    data = other->unk08;

    self->unk20 = data->unk48;
    self->unk50 = data->unk08;
    func_80013BD8(&self->unk28, data->unk24, 0);

    self->unkB5 = data->unk4A;
    self->unkB4 = data->unk49;
    if (self->unkB4 == 20) {
        self->flags |= 0x200000;
    }

    self->unk1C = data->unk18;
    self->unk1A = data->unk1A;
    self->unkAC = data->unk34;
    self->unkB0 = data->unk38;
    self->unk21 = data->unk3C;
    self->unkBC = data->unk40;
    self->unkC0 = data->unk44;
    self->unkC4 = self->unkC0->unk08;
    self->unkC8 = self->unkC0->unk5C;
    self->unkA0 = data->unk28;

    if (self->unkBC != 0) {
        self->unkB8 = self->unkBC->unk08;
    } else {
        self->unkB8 = 0;
    }

    self->flags = (self->flags | 0x840002) & ~0x2000;
}
