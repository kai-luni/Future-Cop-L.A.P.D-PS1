#include "game/types.h"

struct Model;

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Matrix3x3 {
    s32 m[3][3];
};

struct ModelInstance {
    struct Model *model;
    struct Matrix3x3 matrix;
};

struct Model {
    u8 unk00[0x10];
    u8 *unk10;
};

struct DataNode {
    u8 unk00[0x8];
    s32 unk08;
};

struct AnimTrack;

struct Data35904 {
    u8 unk00[0x1A];
    s16 unk1A;
    u8 unk1C[0x8];
    struct Model *unk24;
    struct Vec3 *unk28;
    s32 unk2C;
    s32 unk30;
    struct DataNode *unk34;
    struct DataNode *unk38;
    s32 unk3C;
    u8 unk40;
    u8 unk41[3];
    s32 unk44;
    u16 unk48;
    u16 unk4A;
    u8 unk4C;
    u8 unk4D;
    u16 unk4E;
    u8 unk50;
    u8 unk51;
    u8 unk52[2];
    s32 unk54;
};

struct Arg35904 {
    u8 unk00[8];
    struct Data35904 *unk08;
};

struct Self35904 {
    u8 unk00[0x1A];
    s16 unk1A;
    s16 unk1C;
    u8 unk1E[2];
    u8 unk20;
    u8 unk21;
    u8 unk22[2];
    u32 unk24;
    struct ModelInstance unk28;
    struct Vec3 unk50;
    struct Vec3 unk5C;
    u8 unk68[0xB];
    u8 unk73;
    u8 unk74[4];
    struct Vec3 unk78;
    s32 unk84;
    u8 unk88[0x18];
    s16 unkA0;
    s16 unkA2;
    s16 unkA4;
    s16 unkA6;
    struct Vec3 unkA8;
    s32 unkB4;
    s32 unkB8;
    s32 unkBC;
    s32 unkC0;
    struct DataNode *unkC4;
    struct DataNode *unkC8;
    s32 unkCC;
    s16 unkD0;
    u16 unkD2;
    u16 unkD4;
    s16 unkD6;
    u16 unkD8;
    s16 unkDA;
    u8 unkDC;
    u8 unkDD;
    u8 unkDE[2];
    s32 unkE0;
    s32 unkE4;
    u8 unkE8[0x14];
};

extern s32 func_8002AD58(s32 message, struct Self35904 *object, struct Arg35904 *data, s32 arg3);
extern void func_80013BD8(struct ModelInstance *instance, struct Model *model, s32 options);
extern void func_80044988(u8 *data, struct AnimTrack *track, u8 forward, s32 index);
extern s32 func_80019668(void);
extern void func_800193F8(struct Vec3 *axis, void *basis);

void func_80035904(struct Self35904 *self, struct Arg35904 *arg1, s32 arg2)
{
    struct Data35904 *data;

    func_8002AD58(2, self, arg1, arg2);
    data = arg1->unk08;

    func_80013BD8(&self->unk28, data->unk24, 0);

    self->unkDC = data->unk4D;
    self->unkDD = data->unk50;
    self->unkD8 = data->unk4E;
    self->unk20 = data->unk4C;
    self->unk24 |= 0x40000;

    if (self->unk28.model != 0 && self->unk28.model->unk10 != 0) {
        func_80044988(self->unk28.model->unk10, (struct AnimTrack *)self->unkE8, 1, 0);
    }

    self->unkE0 = data->unk30;
    self->unk73 = data->unk51;
    self->unkD6 = func_80019668() & 0xFFF;

    self->unkA0 = self->unk50.x >> 8;
    self->unkA2 = self->unk50.y >> 8;
    self->unkA4 = self->unk50.z >> 8;

    self->unkA8 = self->unk50;
    self->unk5C = self->unk50;

    self->unk84 = data->unk2C;
    self->unkD4 = data->unk48;
    self->unkD2 = data->unk4A;
    self->unkB8 = 0x10000;
    self->unkB4 = 0;
    self->unkBC = 0;

    if (self->unkDC == 0x15) {
        self->unk1C = data->unk1A * 2;
    }

    self->unk1A = data->unk1A;
    self->unk21 = data->unk40;

    self->unkC4 = data->unk34;
    self->unkE4 = data->unk54;

    if (self->unkC4 != 0) {
        self->unkC0 = self->unkC4->unk08;
        if (self->unkC0 <= 0) {
            self->unkC4 = 0;
        }
    }

    if (self->unkC4 != 0 && self->unkDC == 0x13 && self->unkE0 == 1) {
        self->unkE0 = 2;
    }

    self->unkC8 = data->unk38;
    if (self->unkC8 != 0) {
        self->unkCC = self->unkC8->unk08;
        if (self->unkCC <= 0) {
            self->unkC8 = 0;
        }
    }

    self->unkA6 = data->unk3C >> 8;
    self->unk78 = *data->unk28;
    self->unkD0 = data->unk44 >> 8;
    self->unkDA = 0x5DC;

    self->unk24 = (self->unk24 | 0x800002) & ~0x2000;

    func_800193F8(&self->unk78, &self->unk28.matrix);
}
