// Categories: Gameplay > Object Spawning | Game Engine > Object Setup | Physics > Collision > Ground Query
#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Source {
    u8 pad00[0x34];
    u8 unk34;
    s8 unk35;
    s16 unk36;
    union {
        u16 u;
        s16 s;
    } unk38;
    s16 unk3A;
    s16 unk3C;
    s16 unk3E;
    s16 unk40;
    s16 unk42;
    s16 unk44;
    u8 unk46;
    u8 unk47;
    u8 unk48;
    u8 unk49;
    u8 unk4A;
    u8 unk4B;
    u8 unk4C;
};

struct Sub {
    u8 pad00[0x10];
    u8 *unk10;
};

struct Obj {
    u8 pad00[0x20];
    u8 unk20;
    u8 unk21;
    u8 pad22[2];
    u32 flags;
    struct Sub *unk28;
    u8 pad2C[0x24];
    struct Vec3 pos;
    u8 pad5C[0x1C];
    struct Vec3 off;
    s32 unk84;
    u8 pad88[0x40];
    u8 unkC8[0x14];
    u8 unkDC;
    u8 unkDD;
    u16 unkDE;
    u16 unkE0;
    u16 unkE2;
    u16 unkE4;
    u8 unkE6;
    u8 padE7;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    struct Vec3 tmp;
    struct Vec3 unk100;
    u8 unk10C;
    u8 unk10D;
    u8 unk10E;
    u8 unk10F;
    u8 unk110;
    u8 unk111;
    u8 unk112;
    u8 unk113;
};

struct Arg {
    u8 pad00[8];
    struct Source *unk8;
};

extern s32 func_80067684(struct Vec3 *pos, s32 sel);
extern void gBuildRotationMatrix(s32 yaw, s32 pitch, s32 roll, void *matrix) __asm__("func_800188D0");
extern s32 func_8001A100(s32 *vector);
extern void gSetAnimClipTable(u8 *data, void *track, u8 forward, s32 index) __asm__("func_80044988");
extern void func_80060668(void *in, void *out);

/**
 * @brief Spawn setup shared by the numbered entity handlers: places the object on
 *        the terrain, derives its rates and heading, mirrors the offset vector and
 *        binds the definition's animation and attack descriptor.
 *
 * @param obj Render object: column byte +0x21, flag word +0x24, model record +0x28,
 *        matrix +0x2C, position +0x50, speed +0x84, attack descriptor +0xA0,
 *        animation track +0xC8, class byte +0xDD, +0xDC, rates +0xDE..+0xE8,
 *        offset copy +0x100, appearance bytes +0x10C..+0x113.
 * @param arg Spawn argument block; the definition descriptor it points at is at
 *        +0x08.
 * @return Always 0.
 *
 * @par Confirmed
 * The terrain selector is derived from the descriptor byte at +0x35: 0 maps to -1,
 * 1 to -2 and every other value to value - 2. The position y becomes
 * func_80067684(pos, selector) plus (descriptor +0x36 << 4), i.e. the ground height
 * under the object plus a lift.
 * Heading: +0xE4 receives descriptor +0x38 as an unsigned half-word and
 * func_800188D0(descriptor +0x38, 0, 0, obj + 0x2C) builds the matrix from the same
 * half-word read as a signed angle.
 * Rates: +0xE8 = (descriptor +0x40 << 4) / 300, raised to 1 when the descriptor
 * value is positive and the quotient is zero; +0xDE and +0xE2 both take
 * (descriptor +0x42 << 12) / 218 + 1; +0xE0 takes
 * (descriptor +0x44 << 12) / 218 + 1; +0xE6 becomes 0 and +0x84 becomes 0.
 * The class byte +0xDD is loaded from descriptor +0x34, and the appearance bytes
 * +0x10C..+0x110 and +0x112 are copied from descriptor +0x46..+0x4A and +0x4C while
 * +0x111 takes descriptor +0x4B with bit 0 forced.
 * The offset vector is descriptor +0x3C into x, +0x3E into y and +0x3A into z, each
 * shifted left by 4. With bit 3 of the class byte set the offset is added to the
 * position, the sum becomes the new position, the offset is negated and the
 * pre-move position is kept at the scratch vector; otherwise the scratch vector only
 * takes the position. +0x100 receives the offset, +0xEC is cleared and +0xF0 is its
 * length from func_8001A100. +0xDC is cleared and bit 1 of the class byte sets flag
 * 0x80.
 * When the model record's +0x10 is non-zero, func_80044988 attaches that data to the
 * animation track at +0xC8, and func_80060668 copies the descriptor's +0x24 block
 * into the attack descriptor at +0xA0.
 * With bit 0x40 of the class byte set the column byte +0x21 moves to +0x113 and +0x21
 * is cleared.
 *
 * @par Likely role
 * The placement half that the numbered handlers call from their message-2 case (the
 * type-27 handler func_8002ED7C is the direct caller): it is the counterpart of
 * func_8002FA20 for the other descriptor format, and the two do the same job, ground
 * snapping, rate derivation, matrix build and animation binding. Bit 3 of the class
 * byte reads like "offset back from the surface", and bit 0x40 like "this object
 * occupies a map column".
 *
 * @par Unknown
 * - Whether the /300 and /218 divisors convert frames to something else or the other
 *   way round; both appear in the sibling setup functions with the same shape.
 * - What the class byte bits 2, 3 and 0x40 select in game terms, beyond the branches
 *   they take here.
 * - Whether forcing bit 0 of the animation index at +0x111 exists only to avoid the
 *   0xFF "no animation" marker the type-27 handler tests for.
 *
 * @par Callers
 * func_8002ED7C, case 2 of its switch, after the base spawn hook has run.
 */
s32 func_8002DFC4(struct Obj *obj, struct Arg *arg)
{
    struct Source *st;
    s32 sel;
    u16 h;
    u8 t;

    st = arg->unk8;
    sel = st->unk35 == 0 ? -1 : st->unk35 == 1 ? -2 : st->unk35 - 2;
    obj->pos.y = func_80067684(&obj->pos, sel) + (st->unk36 << 4);
    obj->unkE4 = st->unk38.u;
    gBuildRotationMatrix(st->unk38.s, 0, 0, (u8 *)obj + 0x2C);

    obj->unkE8 = (st->unk40 << 4) / 300;
    if (st->unk40 > 0 && obj->unkE8 == 0) {
        obj->unkE8 = 1;
    }

    obj->unk84 = 0;
    obj->unkDD = st->unk34;
    h = (st->unk42 << 12) / 218 + 1;
    obj->unkE2 = h;
    obj->unkDE = h;
    obj->unkE0 = (st->unk44 << 12) / 218 + 1;
    obj->unkDD = st->unk34;
    obj->unkE6 = 0;

    obj->unk10C = st->unk46;
    obj->unk10D = st->unk47;
    obj->unk10E = st->unk48;
    obj->unk10F = st->unk49;
    obj->unk110 = st->unk4A;
    obj->unk111 = st->unk4B | 1;
    obj->unk112 = st->unk4C;

    obj->off.x = st->unk3C << 4;
    obj->off.y = st->unk3E << 4;
    obj->off.z = st->unk3A << 4;

    if (obj->unkDD & 8) {
        obj->tmp.x = obj->pos.x + obj->off.x;
        obj->tmp.y = obj->pos.y + obj->off.y;
        obj->tmp.z = obj->pos.z + obj->off.z;
        obj->pos = obj->tmp;
        obj->off.x = -obj->off.x;
        obj->off.y = -obj->off.y;
        obj->off.z = -obj->off.z;
    } else {
        obj->tmp = obj->pos;
    }

    obj->unk100 = obj->off;
    obj->unkEC = 0;
    obj->unkF0 = func_8001A100(&obj->off.x);

    obj->unkDC = 0;
    if (obj->unkDD & 2) {
        obj->flags |= 0x80;
    }

    if (obj->unk28->unk10 != 0) {
        gSetAnimClipTable(obj->unk28->unk10, obj->unkC8, 1, 0);
    }
    func_80060668((u8 *)st + 0x24, (u8 *)obj + 0xA0);

    if (obj->unkDD & 0x40) {
        t = obj->unk21;
        obj->unk21 = 0;
        obj->unk113 = t;
    }
    return 0;
}
