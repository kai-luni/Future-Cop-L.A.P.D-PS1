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
    u8 unk14[0x20];
    u8 *unk34;
};

struct AnimTrack {
    u8 unk00[0x14];
};

struct Blob {
    s32 unk00;
    u8 unk04[2];
    u8 unk06;
    u8 unk07[0x21];
};

struct Holder {
    struct Model *model;
};

struct Sub {
    u8 unk00[0x9C];
    s32 unk9C;
    struct Blob unkA0;
    u8 unkC8[0x50];
    u32 unk118;
    u8 unk11C[0xE];
    u16 unk12A;
};

struct Data {
    u8 unk00[0x8];
    struct Vec3 unk08;
    u8 unk14[0x10];
    s32 unk24;
    s16 unk28;
    u8 unk2A;
    u8 unk2B;
    struct Holder *unk2C;
    struct Sub *unk30;
};

struct Arg {
    u8 unk00[0x8];
    struct Data *unk08;
};

struct Self {
    u8 unk00[0x24];
    u32 unk24;
    struct ModelInstance unk28;
    struct Vec3 unk50;
    struct Vec3 unk5C;
    s32 unk68;
    u8 unk6C[0x4];
    s16 unk70;
    u8 unk72[0x2];
    s32 unk74;
    u8 unk78[0x24];
    s32 unk9C;
    struct Blob unkA0;
    struct AnimTrack unkC8;
    s16 unkDC;
    s16 unkDE;
    s32 unkE0;
    u16 unkE4;
    u8 unkE6;
    u8 unkE7;
    union {
        u16 h;
        u32 w;
    } unkE8;
    struct Sub *unkEC;
};

extern s32 func_8002AD58(s32 message, struct Self *self, struct Arg *arg, s32 arg2);
extern void func_80013BD8(struct ModelInstance *instance, struct Model *model, s32 options);
extern void func_80044988(u8 *data, struct AnimTrack *track, u8 forward, s32 index);
extern void func_80010750(void *basis);
extern void func_80010234(void *a, void *b, s32 c);

void func_80038754(struct Self *self, struct Arg *arg1, s32 arg2)
{
    struct Data *data;
    struct Sub *p;
    s32 i;
    u32 f;

    func_8002AD58(2, self, arg1, arg2);
    data = arg1->unk08;

    self->unkE8.h = data->unk2B;
    self->unkEC = data->unk30;
    p = self->unkEC;

    if (data->unk2C != 0) {
        func_80013BD8(&self->unk28, data->unk2C->model, 0);
        if (self->unk28.model->unk10 != 0) {
            func_80044988(self->unk28.model->unk10, &self->unkC8, 1, 0);
        }
    } else {
        self->unk28.model = 0;
        self->unk5C = data->unk08;
        self->unk68 = 0;
    }
    self->unkE7 = 0;

    self->unk74 = 0;
    i = data->unk28;
    if (i < -2) {
        self->unkE8.h |= 0x4000;
        i = -i;
    }
    self->unkE4 = i;
    self->unkE6 = data->unk2A;

    if (self->unkE8.h & 0x20) {
        if (self->unkEC->unk118 & 0x1000) {
            self->unkE4 = (self->unkE4 + p->unk12A) & 0xFFF;
            self->unkDC = self->unkE4;
        } else {
            self->unkDC = self->unkE4;
        }
    } else {
        self->unkDC = 0;
    }
    self->unkDE = 0;
    self->unkE0 = (data->unk24 / 218) >> 4;
    self->unk50 = data->unk08;

    if (!(self->unkE8.h & 4)) {
        self->unkA0 = p->unkA0;
        self->unk9C = p->unk9C;
    } else {
        self->unkA0.unk06 = 0;
    }

    self->unk24 |= 0x800000;
    if (!(self->unkE8.h & 1)) {
        self->unk24 |= 0x200000;
    }
    if (!(self->unkE8.w & 0x41)) {
        self->unk24 |= 1;
    } else {
        f = self->unk24 & ~1;
        self->unk24 = f & ~0x20000;
    }

    func_80010750(&self->unk28.matrix);
    func_80010234(self->unk28.model->unk34 + (((s32)self->unk70 << 4) + 8), &self->unk5C, 1);
}
