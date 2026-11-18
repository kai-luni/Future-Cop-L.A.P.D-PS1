// Categories: Gameplay > Entity Script | Graphics > Model Renderer > Instance Setup | Game Engine > Entity Lifecycle
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

/**
 * @brief Message 2 (initialise) of the model-backed entity handled by func_80036624:
 *        binds the descriptor's model to the object's instance, starts its animation
 *        and copies the descriptor's colours, extents, rates and linked records.
 *
 * @param self Live entity; instance at +0x28 (model at +0, matrix from +0x04), position
 *        +0x50 and +0x5C, animation track +0xE8, class bytes +0xDC/+0xDD, bound
 *        records +0xC4 and +0xC8 with counts +0xC0 and +0xCC.
 * @param arg1 Descriptor holder; +0x08 is the descriptor block read below.
 * @param arg2 Forwarded to the base message-2 handler.
 * @return Nothing.
 *
 * @par Confirmed
 * Binding:
 * func_8002AD58(2, self, arg1, arg2) runs first (the base initialiser func_8002A5F4).
 * func_80013BD8(&self->+0x28, descriptor +0x24, 0) then binds the model to the
 * instance, and when the bound model has animation data at its +0x10,
 * func_80044988(model + 0x10, &self->+0xE8, 1, 0) starts that track at clip 0.
 *
 * Field copy:
 * +0xDC = descriptor +0x4D, +0xDD = +0x50, +0xD8 = +0x4E, +0x20 = +0x4C and +0x24
 * gains 0x40000; +0xE0 = +0x30, +0x73 = +0x51 and +0xD6 = func_80019668() & 0xFFF
 * (a random heading). The instance position is copied whole into +0xA8 and +0x5C and
 * its three words are also stored shifted right by 8 into the short fields +0xA0,
 * +0xA2 and +0xA4. +0x84 = +0x2C, +0xD4 = +0x48, +0xD2 = +0x4A and the rate words
 * start at +0xB8 = 0x10000, +0xB4 = 0, +0xBC = 0. +0x1A = descriptor +0x1A and +0x21 = descriptor +0x40;
 * when the class byte +0xDC is 0x15, +0x1C = descriptor +0x1A * 2.
 *
 * Linked records:
 * +0xC4 takes descriptor +0x34 and its count word +0x08: a non-positive count drops
 * the pointer, otherwise the count is kept at +0xC0; if the pointer survived, class
 * 0x13 with +0xE0 == 1 bumps +0xE0 to 2. +0xC8 takes descriptor +0x38 the same way
 * into +0xCC. +0xA6 = +0x3C >> 8, +0x78 = the vector at descriptor +0x28,
 * +0xD0 = +0x44 >> 8 and +0xDA = 0x5DC.
 *
 * Tail:
 * +0x24 = (+0x24 | 0x800002) & ~0x2000 (alive and script-driven, ground clamp off),
 * then func_800193F8(&self->+0x78, &self->+0x28 matrix) builds the orthonormal basis
 * whose third row is the stored axis.
 *
 * @par Likely role
 * The spawn-time half of the class whose dispatcher func_80036624 answers 0xFC for
 * message 1: the descriptor read here is the shared entity definition record, so this
 * is where a placed object turns into a live, animated, collidable entity.
 *
 * @par Unknown
 * What +0xE0 is (some mode word that is bumped to 2 for class 0x13), what the
 * +0xD2/+0xD4 pair feeds, and whether the counts at +0x08 of the two linked records
 * are polygon or node counts.
 *
 * @par Callers
 * func_80036624, message 2 of its switch.
 */
void gProjectileInit(struct Self35904 *self, struct Arg35904 *arg1, s32 arg2) __asm__("func_80035904");

void gProjectileInit(struct Self35904 *self, struct Arg35904 *arg1, s32 arg2)
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
