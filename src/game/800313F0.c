#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Entity {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C;
    u8 unk0D[0x09];
    u8 unk16;
    u8 unk17[0x11];
    u8 unk28[0x04];
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    struct Vec3 unk50;
    struct Vec3 unk5C;
    u8 unk68[0x38];
    u8 unkA0;
    u8 unkA1;
    s16 unkA2;
    s32 unkA4;
    struct Entity *unkA8;
    s32 unkAC;
    s8 unkB0;
    s8 unkB1;
};

extern struct Entity *func_8004D69C(s32 tag);
extern void func_80017668(void *instance, s32 part, s32 *out);
extern s32 func_80030DE0(struct Entity *self, void *data, s32 arg);
extern u8 func_8002F8B8(u8 arg0, s16 arg1, void *arg2, s32 arg3);
extern s32 func_80031238(struct Entity *self, void *data);
extern s32 func_80031120(struct Entity *self, void *data, s32 arg);
extern s32 func_8002AD58(s32 message, struct Entity *self, void *data);
extern void func_80031004(struct Entity *self);
extern s32 D_8009FAA4;
extern void *jtbl_8001123C[];

void func_800313F0(struct Entity *self)
{
    struct Entity *other;
    s32 id;

    if (self->unkB1 != 0) {
        self->unkB1 = 0;
        other = func_8004D69C(self->unkAC);
        if (other != 0) {
            id = other->unk08;
            self->unkA8 = other;
            self->unkAC = id;
        }
    }

    other = self->unkA8;
    if (other != 0 && other->unk08 == self->unkAC) {
        self->unk2C = other->unk2C;
        self->unk30 = other->unk30;
        self->unk34 = other->unk34;
        self->unk38 = other->unk38;
        self->unk3C = other->unk3C;
        self->unk40 = other->unk40;
        self->unk44 = other->unk44;
        self->unk48 = other->unk48;
        self->unk4C = other->unk4C;

        if (self->unkB0 < 0) {
            self->unk50 = other->unk50;
        } else {
            func_80017668((u8 *)other + 0x28, self->unkB0, (s32 *)&self->unk50);
        }

        self->unk5C = self->unk50;
    }
}
