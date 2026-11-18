// Categories: Gameplay > Object Dynamics | Physics > Motion | Graphics > Effects > Particles | Game Engine > Entity Lifecycle
#include "game/types.h"

struct Unk4ABE4Inner {
    u8 pad00[0x34];
    u8 *unk34;
};

struct Unk4ABE4Parent {
    u8 pad00[0x20];
    s32 unk20;
};

struct Unk4ABE4Alloc {
    s32 unk00;
    s16 unk04;
    s16 unk06;
    u16 unk08;
    u16 unk0A;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    u16 unk18;
    u8 pad1A[2];
};

struct Unk4ABE4Arg1 {
    u8 pad00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    u8 pad10[0x30];
    u16 unk40;
    u8 pad42[7];
    u8 unk49;
};

struct Unk4ABE4 {
    u8 pad00[0x0C];
    s16 unk0C;
    u8 unk0E;
    u8 unk0F;
    u8 unk10;
    u8 pad11[5];
    u16 unk16;
    u8 pad18[2];
    u16 unk1A;
    struct Unk4ABE4Inner *unk1C;
    struct Unk4ABE4Parent *unk20;
    u8 pad24[0x41];
    u8 unk65;
    u8 pad66[4];
    s16 unk6A;
    s16 unk6C;
    s16 unk6E;
    s16 unk70;
    s16 unk72;
    s16 unk74;
    u16 unk76;
    u16 unk78;
    u8 pad7A[2];
    s8 unk7C;
    u8 pad7D[3];
    struct Unk4ABE4Alloc *unk80;
    s32 unk84;
};

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern s32 func_80019668(void);
extern void gGteLoadRotation(u8 *arg0) __asm__("func_80010820");
extern void gGteTransformWords(void *arg0, void *arg1, s32 arg2) __asm__("func_8001006C");
extern void func_8003E984(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4);
s32 gLaunchDebrisObject(struct Unk4ABE4 *object, struct Unk4ABE4Arg1 *arg1) __asm__("func_8004ABE4");


/**
 * @brief Burst variant of an object's death handling: allocates a 28-byte motion
 *        record, fills it with randomly interpolated direction, speed and scale, rotates
 *        it into world space and spawns the object's configured child effect.
 *
 * @param object Object block. +0x1C is the inner block whose +0x34 is the basis
 *        context, +0x1A a flag word, +0x20 a parent block, +0x65 the child type, +0x10
 *        a variant bit, +0x16 a lifetime, +0x76/+0x78 a speed range, +0x6A/+0x6C a
 *        scale range, +0x70/+0x72/+0x74 a direction vector, +0x7C a signed percentage.
 * @param arg1 Optional driver block (0x40 a magnitude, +0x04/+0x08/+0x0C a rotation
 *        vector, 0x49 a flag byte); may be 0.
 * @return 1 when the record was built and the child handed on; 0 when the basis context
 *         or the allocation is missing, and in the first case +0x0F gains bit 0.
 *
 * @par Confirmed construction
 * The record comes from func_80017EBC(0x1C, 1) and is stored at object + 0x80. Its
 * first word is (((random and 0x1F) + 0xA0) shifted 3), scaled by the signed
 * percentage +0x7C over 100 and then multiplied by -16: a large fixed impulse. Words
 * at +8 and +0xA receive the same value, +0x76 plus (range times a random 12-bit
 * fraction) shifted 12 when +0x76 is below +0x78, else +0x78. Words +0xC, +0x10 and
 * +0x14 are +0x70, +0x72 and +0x74 multiplied by a scale drawn the same way from
 * +0x6A/+0x6C and shifted 8, or zero when the scale is zero. Word +0x18 is +0x16
 * while the parent's +0x20 is non-zero, and zero otherwise.
 *
 * @par Confirmed transforms
 * When the flag word +0x1A has bit 0x100 or 0x004, func_80010820 loads the matrix at
 * context + 4 into the GTE and func_8001006C rotates the three-word direction by it in
 * place. With arg1 present and its +0x49 bit 1 set, a magnitude of arg1 + 0x40 shifted
 * 3 is negated when +0x1A has bit 0x200, forced to zero when it has 0x400, and each
 * axis then receives (arg1 + 0x04/+0x08/+0x0C shifted 4) times that magnitude, shifted
 * 8.
 *
 * @par Confirmed tail
 * +0x0C, +0x16 and +0x84 are cleared; a non-zero child type +0x65 passes the inner
 * block, 0, the type, 0 and (object + 0x10 shifted once) and 1 to func_8003E984, the
 * spawn call.
 *
 * @par Likely role
 * The explode-without-splitting-the-model half of the pair with func_8004C324: one
 * debris/particle impulse and one child effect instead of a full polygon fracture.
 *
 * @par Unknown
 * The unit of the first word and what the two duplicated words at +8/+0xA feed.
 *
 * @par Callers
 * func_8004C324, message 0, on the branch where the object flags carry bit 8;
 * the fracture path (bit 0x10) sits in the same handler.
 *
 * @par Environment path
 * Starts the ballistic flight of a debris object when it is destroyed;
 * func_8004C324 calls it from message 0 when flag 8 is set. It allocates a
 * 0x1C-byte flight record with func_80017EBC(0x1C, 1) and fills it with a
 * random lift impulse scaled by the percentage at +0x7C, two random spin
 * rates between the +0x76 and +0x78 bounds and a velocity made from the
 * +0x70/+0x72/+0x74 direction times a random scale between +0x6A and +0x6C.
 * When flag 0x100 or 0x004 at +0x1A is set the velocity is rotated into world
 * space (func_80010820, func_8001006C), and a driver argument with bit 2 at
 * +0x49 adds its own velocity scaled from its +0x40 value (inverted by flag
 * 0x400, suppressed by 0x200). It then copies the timer, clears three fields
 * and spawns the child type at +0x65 through func_8003E984 when it is
 * non-zero; without a basis context it sets bit 0 at +0x0F and returns 0.
 */
s32 gLaunchDebrisObject(struct Unk4ABE4 *object, struct Unk4ABE4Arg1 *arg1)
{
    u8 *context;
    s32 value;
    s32 delta;
    s32 scale;
    s16 turnScale;

    context = 0;
    if (object->unk1C != 0) {
        context = object->unk1C->unk34;
    }

    if (context == 0) {
        object->unk0F |= 1;
        return 0;
    }

    object->unk80 = gAllocMemory(0x1C, 1);
    if (object->unk80 == 0) {
        return 0;
    }

    object->unk80->unk00 = ((func_80019668() & 0x1F) + 0xA0) << 3;
    object->unk80->unk00 = (object->unk80->unk00 * object->unk7C) / 100;
    object->unk80->unk00 = (-object->unk80->unk00) * 16;

    delta = (s16)object->unk78 - (s16)object->unk76;
    object->unk80->unk04 = 0;
    object->unk80->unk06 = 0;

    if (delta > 0) {
        object->unk80->unk08 = object->unk76 + (((u32)(delta * (func_80019668() & 0xFFF))) >> 12);
        object->unk80->unk0A = object->unk76 + (((u32)(delta * (func_80019668() & 0xFFF))) >> 12);
    } else {
        object->unk80->unk08 = object->unk78;
        object->unk80->unk0A = object->unk78;
    }

    scale = (s16)object->unk6C - (s16)object->unk6A;
    if (scale > 0) {
        scale = object->unk6A + (((u32)(scale * (func_80019668() & 0xFFF))) >> 12);
    } else {
        scale = object->unk6C;
    }

    if (scale != 0) {
        object->unk80->unk0C = (object->unk70 * scale) >> 8;
        object->unk80->unk10 = (object->unk72 * scale) >> 8;
        object->unk80->unk14 = (object->unk74 * scale) >> 8;
    } else {
        object->unk80->unk0C = 0;
        object->unk80->unk10 = 0;
        object->unk80->unk14 = 0;
    }

    if (object->unk1A & 0x104) {
        gGteLoadRotation(context + 4);
        gGteTransformWords((u8 *)object->unk80 + 0x0C, (u8 *)object->unk80 + 0x0C, 1);
    }

    if (arg1 != 0 && (arg1->unk49 & 2)) {
        value = 0;
        if (!(object->unk1A & 0x200)) {
            if (object->unk1A & 0x400) {
                value = -(arg1->unk40 << 3);
            } else {
                value = arg1->unk40 << 3;
            }
        }

        turnScale = (s16)value;
        if (turnScale != 0) {
            object->unk80->unk0C += ((arg1->unk04 >> 4) * turnScale) >> 8;
            object->unk80->unk10 += ((arg1->unk08 >> 4) * turnScale) >> 8;
            object->unk80->unk14 += ((arg1->unk0C >> 4) * turnScale) >> 8;
        }
    }

    if (object->unk20->unk20 != 0) {
        object->unk80->unk18 = object->unk16;
    } else {
        object->unk80->unk18 = 0;
    }

    object->unk0C = 0;
    object->unk16 = 0;
    object->unk84 = 0;

    if (object->unk65 != 0) {
        func_8003E984(object->unk1C, 0, object->unk65, 0, (object->unk10 >> 1) & 1);
    }

    return 1;
}
