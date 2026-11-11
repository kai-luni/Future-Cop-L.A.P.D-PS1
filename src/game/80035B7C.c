#include "game/types.h"

struct Vec3_35B7C { s32 x, y, z; };

struct ModelFrame {
    u16 unk00;
    s16 unk02;
    s16 radius;
    u16 unk06;
    u8 unk08[8];
};

struct ModelFrames {
    u8 header[0x10];
    struct ModelFrame frames[1];
};

struct Model {
    u8 unk00[0x10];
    void *unk10;
    u8 unk14[0x20];
    struct ModelFrames *frames;
};

struct ModelInstance {
    struct Model *model;
    u8 coord[0x3C];
    s32 radius;
    u16 unk44;
    u16 unk46;
    s16 frame;
    u8 flags;
    s8 unk4B;
};

struct Obj35B7C {
    u8 pad00[0x8];
    void *unk08;
    u16 unk0C;
    u16 pad0E;
    u8 pad10[4];
    u16 unk14;
    u8 unk16;
    u8 pad17;
    u8 unk18;
    u8 unk19;
    s16 unk1A;
    s16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 pad21[3];
    s32 unk24;
    struct ModelInstance mi;
    u8 pad74[4];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
    u8 pad88[0x18];
    s16 unkA0;
    s16 unkA2;
    s16 unkA4;
    s16 unkA6;
    s32 unkA8;
    s32 unkAC;
    s32 unkB0;
    s32 unkB4;
    s32 unkB8;
    s32 unkBC;
    s32 unkC0;
    struct Obj35B7C *unkC4;
    struct Obj35B7C *unkC8;
    s32 unkCC;
    s16 unkD0;
    s16 unkD2;
    s16 unkD4;
    u16 unkD6;
    s16 unkD8;
    u16 unkDA;
    u8 unkDC;
    u8 unkDD;
    u8 padDE[2];
    s32 unkE0;
    u32 unkE4;
    u8 unkE8[4];
    void *unkEC;
    u8 padF0[4];
    s16 unkF4;
};

struct SpawnInfo35B7C {
    u16 unk00;
    s16 unk02;
    s16 unk04;
    u16 unk06;
    u16 unk08;
    s16 unk0A;
    u16 unk0C;
    u8 unk0E;
    u8 pad0F;
    u8 pad10;
    u8 pad11;
    u8 unk12;
};

extern void func_800175F8(struct ModelInstance *instance, s32 frame);
extern s32 func_8001A13C(s32 *a, s32 *b);
extern s32 func_8001A4E8(s32 *a, s32 *b);
extern void func_8001A180(s32 *vector);
extern s32 func_8001A314(s32 *vector);
extern s32 func_8001A6F8(s32 *a, s32 *b);
extern s32 func_8001AB00(s32 *a, s32 *b);
extern void func_8001ACA4(s32 *a, s32 *b, s32 delta, s32 *out);
extern s32 func_800684AC(s32 *a, s32 *b, s32 *out, s32 arg3, s32 arg4);
extern s32 func_80066F78(s32 *a, s32 b, s32 c, s32 d);
extern s32 func_80069A38(s32 *a);
extern void func_800188D0(s32 a, s32 b, s32 angle, s32 *out);
extern void func_80019310(s32 *first, s32 *second, s32 *basis);
extern void func_80010820(s32 *basis);
extern void func_8001006C(s32 *a, s32 *b, s32 c);
extern void func_80010750(s32 *a);
extern void func_80010234(s32 *a, s32 *b, s32 c);
extern void func_80010974(s32 *a, s32 *b, s32 *out);
extern s32 func_80019FCC(s32 *vector);
extern s32 func_80095900(s32 angle);
extern void func_800410BC(s32 *pos, s32 speed, s32 angle, struct Obj35B7C *target);
extern s32 func_8002AD58(s32 state, struct Obj35B7C *self, struct Obj35B7C *other, s32 arg3);
extern void func_80035904(struct Obj35B7C *self, struct Obj35B7C *other, s32 arg3);
extern s32 func_800402DC(void *owner, u8 column, s32 arg2, s32 row, s32 arg4);
extern void func_8003B898(u8 arg0, struct Obj35B7C *self, s32 arg2, s32 arg3, s32 arg4);
extern void *func_8004E80C(struct Obj35B7C *target, u8 arg1, s16 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6);
extern void *func_8005E47C(struct SpawnInfo35B7C *info, struct Model *model, s32 *vec80, s32 *vec70, struct Obj35B7C *unkC8, struct Obj35B7C *s1p, s32 unk24, s32 arg7, s32 arg8, s32 arg9, s32 arg10, u32 unkE4);
extern void *func_8006C224(struct Obj35B7C *target, struct Obj35B7C *arg1, s32 arg2, s32 arg3);
extern void func_80044B34(struct Obj35B7C *self, void *unkE8, s32 delta, s32 arg3);

void func_80035B7C(struct Obj35B7C *self, s32 delta)
{
    struct Obj35B7C *p1;
    struct Obj35B7C *p2;
    struct Obj35B7C *target;
    s32 s3;
    s32 s4;
    s32 i;
    s32 temp;
    s32 vec30[3];
    s32 vec40[3];
    s32 sp50[3];
    s32 sp60[3];
    s32 vec70[3];
    s32 vec80[3];
    struct SpawnInfo35B7C info;
    s32 vecA8[3];
    s32 spB8[9];
    s32 spE0[3];

    if (self->unkE0 == 4) {
        func_8003B898(self->unk20, self, 0, (self->unk19 >> 3) & 0x1E, 2);
        if (self->unkDD != 0xFF) {
            target = self->unkC8;
            if (target == 0) {
                target = self;
            }
            p1 = (struct Obj35B7C *)func_8004E80C(target, self->unkDD, self->unkD8, 1,
                                                   self->unkA6 << 8, 0xFFFF0000, 0);
        } else {
            if (self->unkC8 != 0) {
                target = self->unkC8;
                if (target->unk16 == 0x12) {
                    target = (struct Obj35B7C *)target->unkB4;
                }
                p1 = (struct Obj35B7C *)func_8006C224(target, self->unkC4, self->unkA6 << 8, -1);
            } else {
                p1 = 0;
            }
        }
        if (p1 != 0) {
            register s32 val __asm__("$2");
            register s32 speed __asm__("$3");
            self->unkDA = 0x5DC;
            self->unkE0 = 1;
            val = *(s32 *)((u8 *)p1 + 8);
            speed = self->unkD4;
            self->unkC4 = p1;
            self->unkC0 = val;
            self->unk84 = speed;
        } else {
            self->unkE0 = 0;
        }
    }

    if (self->mi.model != 0 && self->mi.model->unk10 != 0) {
        switch (((u8 *)self->unkEC)[1]) {
        case 1:
            temp = delta;
            break;
        case 2:
            temp = self->unk84 * delta;
            break;
        default:
            temp = 0;
            break;
        }
        func_80044B34(self, self->unkE8, temp, 0);
        func_800175F8(&self->mi, self->unkF4);
    }

    if (self->unkC8 != 0) {
        if (self->unkCC != (s32)self->unkC8->unk08) {
            self->unkC8 = 0;
        }
    }

    vec30[0] = self->unkA0 << 8;
    vec30[1] = self->unkA2 << 8;
    vec30[2] = self->unkA4 << 8;
    s4 = func_8001A4E8(vec30, (s32 *)((u8 *)self + 0x50));

    if (self->unkDC == 0x12 && s4 >= 0x3001) {
        info.unk06 = self->unkD4;
        info.unk08 = self->unkD2;
        info.unk00 = self->unk1A;
        info.unk0A = self->unkA6 << 4;
        info.unk12 = 0;
        info.unk0E = self->unk20;
        info.unk02 = self->unkD0 >> 4;
        for (i = 0; i < 3; i++) {
            if (self->unkDD != 0xFF) {
                target = self->unkC8;
                if (target == 0) target = self;
                p2 = (struct Obj35B7C *)func_8004E80C(target, self->unkDD, self->unkD8, 1,
                                                       self->unkA6 << 8, 0xFFFF0000, 0);
            } else {
                target = self->unkC8;
                if (target != 0) {
                    register struct Obj35B7C *arg __asm__("$5");
                    if (target->unk16 == 0x12) target = (struct Obj35B7C *)target->unkB4;
                    if (i > 0) {
                        arg = ((struct Obj35B7C *)vecA8[i - 1])->unkC4;
                    } else {
                        arg = self->unkC4;
                    }
                    p2 = (struct Obj35B7C *)func_8006C224(target, arg, self->unkA6 << 8, 0x9C40);
                } else {
                    p2 = 0;
                }
            }
            if (p2 == 0) {
                if ((i & 1) == 0) {
                    p2 = self->unkC4;
                } else {
                    p2 = ((struct Obj35B7C *)vecA8[i - 1])->unkC4;
                }
                if (p2 == 0) {
                    break;
                }
            }
            *(struct Vec3_35B7C *)vec80 = *(struct Vec3_35B7C *)((u8 *)self + 0x50);
            vec70[0] = *(s32 *)((u8 *)p2 + 0x5C) - *(s32 *)((u8 *)self + 0x5C);
            vec70[1] = *(s32 *)((u8 *)p2 + 0x60) - *(s32 *)((u8 *)self + 0x60);
            vec70[2] = *(s32 *)((u8 *)p2 + 0x64) - *(s32 *)((u8 *)self + 0x64);
            switch (i) {
            case 0:
                vec70[0] += 0x2000;
                vec70[2] += 0x2000;
                break;
            case 1:
                vec70[0] -= 0x2000;
                vec70[2] -= 0x2000;
                break;
            case 2:
                vec70[1] += 0x2000;
                break;
            }
            func_8001A180(vec70);
            temp = func_8001AB00((s32 *)((u8 *)self + 0x44), vec70);
            if (temp > -0xEC83) {
                void *created = func_8005E47C(&info, self->mi.model, vec80, vec70,
                                              self->unkC8, p2, self->unk24, 3, 0, 0, 0,
                                              self->unkE4);
                vecA8[i] = (s32)created;
                if (created != 0) {
                    if (self->mi.unk4B != 0) {
                        func_800402DC((void *)vecA8[i], (u8)self->mi.unk4B, 0, 1, 0);
                    }
                }
            } else {
                break;
            }
        }
        self->unkDC = 3;
    }

    if ((self->unkA6 << 8) < s4) self->unk1C = -1;

    if (self->unk1C >= 0) {
        *(struct Vec3_35B7C *)&self->unkA8 = *(struct Vec3_35B7C *)((u8 *)self + 0x50);
        {
            s32 accel;
            s32 lim;
            lim = self->unkD4 << 4;
            accel = self->unkD2;
            if (accel != 0) {
                if (self->unk84 < lim) {
                    self->unk84 += (accel * delta) / 16;
                    if (self->unk84 > lim) self->unk84 = lim;
                }
            }
        }
        s3 = delta * self->unk84;
        p1 = 0;
        switch ((u32)self->unkE0) {
        case 2:
            if (*(s32 *)((u8 *)self + 0x54) - (self->unkA2 << 8) >= 0x4001) self->unkE0 = 1;
            { register s32 y __asm__("$3"); s32 c; c = 0x18000; y = self->unk7C; y *= 3; self->unk7C = (y + c) / 4; }
            func_8001A180(&self->unk78);
            func_80010974(&self->unk78, (s32 *)((u8 *)self + 0x2C), &self->unkB4);
            break;
        case 1:
        case 3:
            self->unkDA = (u16)(self->unkDA - delta);
            if ((s16)self->unkDA < 0) {
                self->unk84 /= 2;
                self->unkD4 = (s16)(self->unk84 >> 4);
                self->unkDA += 0x12C;
                if (self->unk84 < 16) {
                    self->unk1C = -1;
                }
            }
            p1 = self->unkC4;
            if (p1 != 0 && p1->unk08 != (void *)self->unkC0) p1 = 0;
            if (p1 != 0) {
                *(struct Vec3_35B7C *)sp60 = *(struct Vec3_35B7C *)((u8 *)p1 + 0x5C);
                if ((s32)p1->unk08 > (s32)self->unk08) {
                    func_8001ACA4(sp60, (s32 *)((u8 *)p1 + 0x78), p1->unk84 * delta, sp60);
                }
                vec30[0] = sp60[0] - *(s32 *)((u8 *)self + 0x50);
                vec30[1] = sp60[1] - *(s32 *)((u8 *)self + 0x54);
                vec30[2] = sp60[2] - *(s32 *)((u8 *)self + 0x58);
                s4 = func_8001A314(vec30);
                if (s4 <= 0x7FFF) {
                    if ((p1->unk16 == 1 &&
                         *(s16 *)(*(u8 **)((u8 *)p1 + 0xAC) + 0x324) > 0) ||
                        (p1->unk16 == 0x12 &&
                         *(s16 *)(*(u8 **)(*(u8 **)((u8 *)p1 + 0xB4) + 0xAC) + 0x324) > 0)) {
                        self->unkE0 = 0;
                    }
                }
                if (self->unkE0 == 3) {
                    vec30[1] += 0xA000;
                    s4 = func_8001A314(vec30);
                }
                if (s3 >= s4) {
                    s3 = s4;
                    if (s3 < 8) {
                    zero:
                        self->unkE0 = 0;
                    } else {
                        *(struct Vec3_35B7C *)&self->unk78 = *(struct Vec3_35B7C *)vec30;
                    }
                } else {
                    vec40[0] = vec30[0] - self->unk78;
                    vec40[1] = vec30[1] - self->unk7C;
                    vec40[2] = vec30[2] - self->unk80;
                    func_8001A6F8(vec30, &self->unk78);
                    func_8001A180(&self->unk78);
                    vec40[1] += 0x2000;
                    func_8001A180(vec40);
                    func_8001A6F8(vec40, &self->unkB4);
                }
            } else {
                goto zero;
            }
            break;
        case 0:
            break;
        default:
            goto skip;
        }

        {
            func_8001ACA4((s32 *)((u8 *)self + 0x50), &self->unk78, s3,
                          (s32 *)((u8 *)self + 0x50));
            if (self->unkDC == 0x15) {
                if (self->unkE0 == 3) self->unkE0 = 1;
                if (p1 != 0) {
                    if (func_8001A13C((s32 *)((u8 *)self + 0x50), sp60) >= 0x301) {
                        if (*(s32 *)((u8 *)self + 0x54) - func_80069A38((s32 *)((u8 *)self + 0x50)) < 0x800) {
                            *(s32 *)((u8 *)self + 0x54) += 0x200;
                            self->unkE0 = 3;
                            self->unk84 /= 2;
                        }
                    }
                }
            }
            if (func_800684AC((s32 *)((u8 *)self + 0xA8), (s32 *)((u8 *)self + 0x50),
                              sp50, 0, 0) != 0) {
                self->unk1C = -1;
                *(struct Vec3_35B7C *)((u8 *)self + 0x50) = *(struct Vec3_35B7C *)sp50;
            } else {
                temp = func_80066F78((s32 *)((u8 *)self + 0x50), 0, 0, 0);
                if (*(s32 *)((u8 *)self + 0x54) < temp) {
                    self->unk1C = -1;
                    *(s32 *)((u8 *)self + 0x50) =
                        (*(s32 *)((u8 *)self + 0x50) + self->unkA8) >> 1;
                    *(s32 *)((u8 *)self + 0x54) =
                        (*(s32 *)((u8 *)self + 0x54) + self->unkAC) >> 1;
                    *(s32 *)((u8 *)self + 0x58) =
                        (*(s32 *)((u8 *)self + 0x58) + self->unkB0) >> 1;
                }
            }
        }
    }

skip:
    spE0[0] = self->unkB4;
    spE0[1] = self->unkB8 + 0x20000;
    spE0[2] = self->unkBC;
    if (self->unkE4 & 2) {
        s32 ang;
        ang = self->unkD6;
        ang += (delta << 12) >> 6;
        ang &= 0xFFF;
        self->unkD6 = ang;
        func_800188D0(0, 0, ang, (s32 *)((u8 *)self + 0x2C));
        func_80019310((s32 *)((u8 *)self + 0x78), spE0, spB8);
        func_80010820(spB8);
        func_8001006C((s32 *)((u8 *)self + 0x2C), (s32 *)((u8 *)self + 0x2C), 3);
    } else {
        func_80019310((s32 *)((u8 *)self + 0x78), spE0, (s32 *)((u8 *)self + 0x2C));
    }
    func_80010750((s32 *)((u8 *)self + 0x2C));
    func_80010234((s32 *)((u8 *)self->mi.model->frames + ((self->mi.frame << 4) + 8)),
                  (s32 *)((u8 *)self + 0x5C), 1);
}

