// Categories: Gameplay > Entity Initialization | Game Engine > Object Setup
#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Def {
    u8 pad00[0x24];
    u16 unk24;
    s16 unk26;
    u8 unk28;
    u8 unk29;
    u16 unk2A;
    s16 unk2C;
    u8 unk2E;
    u8 pad2F;
    u16 unk30;
    u16 unk32;
    u16 unk34;
};

struct Arg {
    u8 pad00[8];
    struct Def *def;
};

struct Entity {
    u8 pad00[0x16];
    u8 unk16;
    u8 pad17[0x0D];
    u32 flags;
    u8 pad28[0x28];
    struct Vec3 unk50;
    struct Vec3 unk5C;
    s32 unk68;
    s16 unk6C;
    s16 unk6E;
    u8 pad70[2];
    u8 flags72;
    u8 pad73[0x2D];
    u8 unkA0;
    u8 unkA1;
    s16 unkA2;
    s32 unkA4;
    void *unkA8;
    s32 unkAC;
    s8 unkB0;
    s8 unkB1;
    s16 unkB2;
    s16 unkB4;
    s16 unkB6;
};

extern s32 func_8002AD58(s32 message, struct Entity *self, struct Arg *arg, s32 arg3);
extern s32 func_80095900(s32 arg0);
extern s32 func_80067684(struct Vec3 *pos, s32 mode);

/**
 * @brief Spawn initializer of the 0xB8-byte trigger/attachment volume class.
 *
 * Forwards message 2 to the base handler, then copies the definition's size
 * and behaviour bytes into the entity. For the box type 0x26 it derives the
 * half-extents (definition words halved) and computes horizontal and full 3D
 * bounding radii with the integer square root. The terrain surface mode comes
 * from the definition type byte (0 top, 1 bottom, else index type-2); both
 * position copies are snapped to that height and the active flags are set.
 *
 * @param self Entity block of this class.
 * @param arg Spawn payload; +0x08 points at the definition record.
 * @param arg2 Forwarded to the base handler.
 * @return Always 0.
 *
 * @par Globals
 * None touched directly.
 *
 * @par Control flow
 * Base call, then a type-0x26 branch computing two radii. Flag 0x80000 clears
 * bit 0x10 of +0x72. Three-way terrain mode selection, then the height snap.
 * Behaviour bit 8 forces +0xA1 to 7.
 *
 * @par Calls
 * func_8002AD58 - base entity handler with message 2.
 * func_80095900 - integer square root for the box radii.
 * func_80067684 - terrain surface height for the selected mode.
 *
 * @par Side effects
 * Writes radii +0x68/+0x6C/+0x6E, behaviour +0xA0..+0xA4, the box fields
 * +0xA8..+0xB6 for type 0x26, heights +0x54/+0x60 and flags |= 0xA00011.
 * These fields feed func_80031238 (box overlap) and func_800313F0 (attach).
 *
 * @par Callers
 * Confirmed: func_8003151C (message 2 of the class handler whose size query
 * returns 0xB8; call site 0x80031570).
 */
s32 func_80030DE0(struct Entity *self, struct Arg *arg, s32 arg2)
{
    struct Def *def;
    s32 s1;
    s32 s0;
    s32 mode;
    s32 type;

    func_8002AD58(2, self, arg, arg2);
    def = arg->def;

    self->unk68 = (s16)def->unk24 << 4;
    self->unk6E = self->unk6C = def->unk24;
    self->unkA1 = def->unk29;
    self->unkA2 = def->unk2A;
    self->unkA0 = def->unk28;
    self->unkA4 = 0;

    if (self->unk16 == 0x26) {
        self->unkA8 = 0;
        self->unkAC = def->unk2C;
        self->unkB0 = def->unk2E - 1;
        self->unkB1 = 1;
        self->unkB2 = (s16)def->unk32 / 2;
        self->unkB4 = (s16)def->unk34 / 2;
        self->unkB6 = (s16)def->unk30 / 2;
        s1 = (self->unkB2 * self->unkB2 + self->unkB6 * self->unkB6) >> 8;
        s0 = s1 + ((self->unkB4 * self->unkB4) >> 8);
        s1 = func_80095900(s1) << 2;
        s0 = func_80095900(s0) << 2;
        self->unk68 = s0;
        self->unk6E = s1 >> 4;
        self->unk6C = (s16)def->unk34 / 2;
    }

    if (self->flags & 0x80000) {
        self->flags72 &= ~0x10;
    }

    type = def->unk26;
    if (type != 0) {
        if (type == 1) {
            mode = -2;
        } else {
            mode = type - 2;
        }
    } else {
        mode = -1;
    }

    self->unk50.y = self->unk5C.y = func_80067684(&self->unk50, mode);

    self->flags |= 0xA00011;
    if (self->unkA0 & 8) {
        self->unkA1 = 7;
    }

    return 0;
}
