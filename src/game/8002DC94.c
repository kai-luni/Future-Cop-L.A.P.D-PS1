#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Basis {
    struct Vec3s32 rows[3];
};

struct Model {
    u8 pad00[0x10];
    u8 *anim;
    u8 pad14[0x20];
    u8 *unk34;
};

struct Instance {
    struct Model *model;
    struct Basis basis;
};

struct Self {
    u8 pad00[0x24];
    u32 flags;
    struct Instance inst;
    struct Vec3s32 pos;
    u8 unk5C[0x14];
    s16 unk70;
    u8 pad72[0x2E];
    u8 animTrack[0xC];
    s16 unkAC;
    u8 padAE[6];
    struct Thing *unkB4;
};

struct Thing {
    u8 pad00[0x100];
    s32 unk100;
};

struct Weapon {
    u8 pad00[4];
    u8 mode;
    u8 pad05[0x1F];
    s8 unk24;
    u8 unk25;
    s16 unk26;
    s32 unk28;
    u8 pad2C[8];
    s8 unk34;
    u8 pad35[5];
    s16 unk3A;
    u8 pad3C[8];
    u8 unk44;
    u8 pad45[1];
    s16 unk46;
};

struct Arg1 {
    u8 pad00[8];
    struct Weapon *weapon;
};

extern s32 func_8002AD58(s32 message, struct Self *self, struct Arg1 *arg1, s32 arg2);
extern s32 func_80067684(struct Vec3s32 *pos, s32 sel);
extern s32 func_80066F78(struct Vec3s32 *pos, struct Vec3s32 *out, s32 a2, s32 a3);
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);
extern void func_800192AC(struct Vec3s32 *first, struct Vec3s32 *second, struct Basis *basis);
extern void func_80044988(u8 *data, u8 *track, u8 forward, s32 index);
extern void func_80010750(struct Basis *basis);
extern void func_80010234(u8 *ptr, u8 *unk5C, s32 a2);

s32 func_8002DC94(struct Self *self, struct Arg1 *arg1, s32 arg2)
{
    struct Weapon *w;
    s32 mode;
    s32 sel;
    s32 base;
    u8 flag;
    s32 angle;
    struct Vec3s32 vec;
    struct Vec3s32 out;
    s32 sine;
    s32 cosine;
    s32 sine2;
    s32 cosine2;

    w = arg1->weapon;
    if (w->mode == 0x18) {
        w->mode = 0xB;
    }
    mode = w->mode;
    if (mode == 8 || mode == 0x21 || mode == 0x24) {
        func_8002AD58(2, self, arg1, arg2);
        self->flags = (self->flags | 0x11) & 0xFFEFDEF7;
        mode = w->unk34;
        flag = w->unk44;
        base = w->unk3A << 4;
        angle = w->unk46;
    } else {
        func_8002AD58(2, self, arg1, arg2);
        mode = w->unk24;
        base = w->unk28;
        flag = w->unk25;
        angle = w->unk26;
    }
    if (mode != 0) {
        if (mode == 1) {
            sel = -2;
        } else {
            sel = mode - 2;
        }
    } else {
        sel = -1;
    }
    self->pos.y = func_80067684(&self->pos, sel) + base;
    if (flag & 1) {
        self->pos.y = func_80066F78(&self->pos, &out, 0, 0) + base;
        func_8001846C(angle, &sine, &cosine);
        vec.x = cosine;
        vec.y = 0;
        vec.z = sine;
        func_800192AC(&out, &vec, &self->inst.basis);
    } else {
        func_8001846C(angle, &sine2, &cosine2);
        self->inst.basis.rows[2].y = 0;
        self->inst.basis.rows[1].z = 0;
        self->inst.basis.rows[1].x = 0;
        self->inst.basis.rows[0].y = 0;
        self->inst.basis.rows[1].y = 0x10000;
        self->inst.basis.rows[2].z = cosine2;
        self->inst.basis.rows[2].x = sine2;
        self->inst.basis.rows[0].x = cosine2;
        self->inst.basis.rows[0].z = -sine2;
    }
    if (self->inst.model->anim != 0) {
        func_80044988(self->inst.model->anim, self->animTrack, 1, 0);
    }
    func_80010750(&self->inst.basis);
    func_80010234(self->inst.model->unk34 + (((s32)self->unk70 << 4) + 8), self->unk5C, 1);
    self->unkB4 = 0;
    return 0;
}

