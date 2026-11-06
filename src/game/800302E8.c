#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Matrix3x3 {
    s32 m[3][3];
};

struct Model {
    u8 pad00[0x10];
    void *unk10;
    u8 pad14[0x20];
    u8 *unk34;
};

struct ModelInstance {
    struct Model *model;
    struct Matrix3x3 matrix;
};

struct Sub {
    u8 pad00[0x18];
    void **unk18;
    u8 pad1C[0x08];
    u8 unk24;
    u8 unk25;
    u16 unk26;
    s16 unk28;
    u16 unk2A;
    u32 unk2C;
    s16 unk30;
    u16 unk32;
    u16 unk34;
};

struct Spawn {
    u8 pad00[4];
    struct Sub *unk04;
    struct Sub *unk08;
};

struct Entity {
    u8 pad00[0x24];
    u32 flags;
    struct Model *model;
    u8 coord[0x24];
    struct Vec3 pos;
    u8 verts[0x14];
    s16 unk70;
    u8 pad72[0x2E];
    u8 unkA0[0x14];
    u32 unkB4;
    u8 padB8[2];
    u16 unkBA;
    u8 unkBC;
    u8 unkBD;
    u16 unkBE;
    u8 padC0[4];
    u16 unkC4;
    s16 unkC6;
    u16 unkC8;
    u8 padCA[2];
    u16 unkCC;
    u8 padCE[6];
    struct ModelInstance d4;
    u8 unkFC[0x24];
    u8 unk120[0x20];
};

extern s32 func_8002AD58(s32 message, struct Entity *self, struct Spawn *data, s32 arg3);
extern s32 func_80067684(struct Vec3 *pos, s32 mode);
extern void func_800188D0(s32 yaw, s32 pitch, s32 roll, void *matrix);
extern void func_80010750(void *coord);
extern void func_80010234(void *vertices, void *out, s32 count);
extern void func_80044988(void *data, void *track, u8 forward, s32 index);
extern void func_80013BD8(struct ModelInstance *instance, struct Model *model, s32 options);
extern void func_80017668(void *instance, s32 part, void *out);

void func_800302E8(struct Entity *self, struct Spawn *data, s32 arg2)
{
    struct Spawn *copy;
    struct Sub *sub;
    struct ModelInstance *inst;
    struct Model *model;
    s8 type;
    s32 mode;
    s32 value;

    func_8002AD58(2, self, data, arg2);

    copy = data;
    sub = copy->unk08;

    self->unkC6 = sub->unk2A;
    self->unkBC = sub->unk25;
    self->unkBA = sub->unk26;
    self->unkB4 = sub->unk2C | 0x1000000;
    self->unkC4 = sub->unk34;
    self->unkBE = sub->unk32;
    self->unkBD = 0;
    self->unkCC = 0;

    type = sub->unk24;
    if (type != 0) {
        if (type == 1) {
            mode = -2;
        } else {
            mode = type - 2;
        }
    } else {
        mode = -1;
    }

    value = func_80067684(&self->pos, mode);
    self->pos.y = value + (sub->unk28 << 4);
    self->flags |= 0x800001;
    func_800188D0(sub->unk30, 0, 0, self->coord);
    func_80010750(self->coord);
    func_80010234(self->model->unk34 + ((self->unk70 << 4) + 8), self->verts, 1);
    func_80044988(self->model->unk10, self->unkA0, 1, 0);

    inst = &self->d4;
    model = 0;
    if (copy->unk04->unk18 != 0) {
        model = (struct Model *)*copy->unk04->unk18;
    }
    func_80013BD8(inst, model, 0);

    if (self->d4.model->unk10 != 0) {
        func_80044988(self->d4.model->unk10, self->unk120, 1, 0);
    }

    func_80017668(&self->model, 0, self->unkFC);
    self->unkC8 = 0;
}
