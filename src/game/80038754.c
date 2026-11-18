// Categories: Gameplay > Object Spawning | Game Engine > Object Setup > Message 2 | Graphics > Model Renderer | Graphics > Animation
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

extern s32 gEntityBaseHandler(s32 message, struct Self *self, struct Arg *arg, s32 arg2) __asm__("func_8002AD58");
extern void func_80013BD8(struct ModelInstance *instance, struct Model *model, s32 options);
extern void gSetAnimClipTable(u8 *data, struct AnimTrack *track, u8 forward, s32 index) __asm__("func_80044988");
extern void gGteLoadMatrix(void *basis) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *a, void *b, s32 c) __asm__("func_80010234");
void gInitActorFromSpawn(struct Self *self, struct Arg *arg1, s32 arg2) __asm__("func_80038754");


/**
 * @brief Spawn initialisation of one object class: binds the model instance and its
 *        animation, sets heading, speed factor, shape reference and object flags from the
 *        spawn descriptor, then builds the matrix and the world position.
 *
 * @param self Object block. +0x24 the object flag word, +0x28 the model instance,
 *        +0x50/+0x5C two position vectors, +0x70 an offset index, +0x9C/+0xA0 two shape
 *        words, +0xC8 an animation track, +0xDC/+0xDE two angles, +0xE0 a speed factor,
 *        +0xE4 the heading, +0xE6 a type byte, +0xE7 a state byte, +0xE8 a flag half-word
 *        that is also read as a word, +0xEC the shared block.
 * @param arg1 Spawn descriptor: +0x08 the initial position, +0x24 a speed value, +0x28 a
 *        signed heading, +0x2A a type byte, +0x2B a flag half-word, +0x2C a model holder
 *        and +0x30 the shared block.
 * @param arg2 Forwarded unchanged to func_8002AD58 as its last argument.
 * @return Nothing.
 *
 * @par Confirmed sequence
 * 1. func_8002AD58(2, self, arg1, arg2) runs first - the shared message 2 hook.
 * 2. self + 0xE8 takes the half-word arg1 + 0x2B, self + 0xEC the pointer arg1 + 0x30.
 * 3. With a model holder, func_80013BD8 binds its model to the instance at self + 0x28,
 *    and a non-null model + 0x10 installs the animation track self + 0xC8 through
 *    func_80044988. Without a holder the instance model is cleared, self + 0x5C takes
 *    the spawn position and self + 0x68 is zeroed.
 * 4. self + 0xE7 and +0x74 are cleared; the signed heading arg1 + 0x28, when below -2,
 *    raises flag bit 0x4000 and is used as a magnitude. It lands in self + 0xE4 and the
 *    type byte arg1 + 0x2A in self + 0xE6.
 * 5. With flag bit 0x20 set the heading is offset by the shared block's +0x12A only if
 *    its +0x118 has bit 0x1000; in both cases it is masked to 0xFFF and copied into
 *    self + 0xDC. Without the bit, +0xDC is zero.
 * 6. self + 0xDE is cleared, self + 0xE0 becomes (arg1 + 0x24 divided by 218) shifted 4,
 *    and self + 0x50 the spawn position.
 * 7. Unless the flags have bit 4, the shape words self + 0x9C and +0xA0 are copied from
 *    the shared block; otherwise only the byte at +0xA2 is zeroed.
 *
 * @par Confirmed object flags
 * self + 0x24 gains 0x800000 unconditionally and 0x200000 when flag bit 0 is clear. It
 * also gains bit 0 when neither bit 0 nor bit 6 of the flag word is set, and otherwise
 * loses bit 0 and bit 0x20000 - the descriptor therefore decides orientation handling
 * and visibility.
 *
 * @par Confirmed finish
 * func_80010750 loads the instance matrix at self + 0x28 into the GTE with its
 * translation and func_80010234 transforms the vertex at model + 0x34, offset by
 * (self + 0x70 shifted 4) + 8, into self + 0x5C - the same pair func_800588D4 uses for
 * its per-frame refresh.
 *
 * @par Likely role
 * The message 2 hook of one moving object class: model, animation, heading and speed
 * all come from the spawn record, so the body is setup only and runs no simulation.
 *
 * @par Unknown
 * The constant 218 in the speed conversion, and whether +0xDC and +0xE4 separate visual
 * facing from movement direction.
 *
 * @par Callers
 * func_80039298, message 2 of its switch - the entity script dispatcher of this class.
 *
 * @par Environment path
 * Message-2 (init) handler of the object class whose dispatcher is
 * func_80039298; that dispatcher returns 0 for message 6, so this class has
 * no per-tick update of its own. After the shared func_8002AD58 hook it
 * stores the flag half-word and the shared block pointer, binds the
 * descriptor model with func_80013BD8 and installs its animation through
 * func_80044988, or clears the model and copies the spawn position when there
 * is none. It then derives heading (flag 0x4000 for negative values below
 * -2), the type byte, an angle optionally biased by the shared block's
 * +0x12A, a speed factor (descriptor value / 218), copied shape words and
 * object flag bits, and finishes with func_80010750 plus one func_80010234
 * vertex transform; it reads no globals.
 */
void gInitActorFromSpawn(struct Self *self, struct Arg *arg1, s32 arg2)
{
    struct Data *data;
    struct Sub *p;
    s32 i;
    u32 f;

    gEntityBaseHandler(2, self, arg1, arg2);
    data = arg1->unk08;

    self->unkE8.h = data->unk2B;
    self->unkEC = data->unk30;
    p = self->unkEC;

    if (data->unk2C != 0) {
        func_80013BD8(&self->unk28, data->unk2C->model, 0);
        if (self->unk28.model->unk10 != 0) {
            gSetAnimClipTable(self->unk28.model->unk10, &self->unkC8, 1, 0);
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

    gGteLoadMatrix(&self->unk28.matrix);
    gGteTransformVerticesWide(self->unk28.model->unk34 + (((s32)self->unk70 << 4) + 8), &self->unk5C, 1);
}
