// Categories: Gameplay > Spawn Placement
#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Sub {
    u8 pad00[0x24];
    s32 unk24;
    s32 unk28;
    s16 unk2C;
    s8 unk2E;
    u8 unk2F;
    u8 unk30;
    u8 unk31;
    u16 unk32;
    u16 unk34;
    s16 unk36;
};

struct Spawn {
    u8 pad00[8];
    struct Sub *unk08;
};

struct Entity {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[0x28];
    s32 unk50;
    s32 unk54;
    u8 pad58[8];
    s32 unk60;
    u8 pad64[4];
    s32 unk68;
    u16 unk6C;
    u16 unk6E;
    u8 pad70[2];
    u8 unk72;
    u8 pad73[0x2D];
    s32 unkA0;
    s32 unkA4;
    s32 unkA8;
    u8 unkAC;
    u8 unkAD;
    u16 unkAE;
};

extern s32 gEntityBaseHandler(s32 message, struct Entity *self, struct Spawn *data, s32 arg3) __asm__("func_8002AD58");
extern s32 func_80067684(struct Vec3 *pos, s32 mode);
s32 gInitEntityGroundPair(struct Entity *self, struct Spawn *data, s32 arg2) __asm__("func_80030AD4");


/**
 * @brief Positions a freshly spawned entity from its spawn record.
 *
 * The base spawn (message 2) runs first. The sub-record at 0x08 then supplies
 * the fields: 0x24 and 0x28 doubled become the y and x offsets, the ground
 * height is queried with func_80067684 using the mode encoded in 0x2E (-1 for
 * type 0, -2 for type 1, type - 2 otherwise) plus 0x2C << 4, and the same is
 * done for the second position (0x50/0x54/0x60) with 0x2F and 0x36. The size
 * halfword 0x34 fills 0x6C/0x6E and 0x68, 0x31 and 0x32 are copied to 0xAD
 * and 0xAE, and flags 0xA00011 are raised. An entity carrying flag 0x80000
 * gets bit 0x10 of 0x72 cleared, and the byte pair 0x10100 forces the subtype
 * at 0xAD to 8.
 *
 * @param self Entity that is placed.
 * @param data Spawn record with the sub-record pointer at 0x08.
 * @param arg2 Third argument passed on to the base spawn.
 * @return Always 0.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Straight-line assignment with two ground queries.
 *
 * @par Calls
 * func_8002AD58 - base spawn. func_80067684 - ground height query.
 *
 * @par Side effects
 * Fills the entity geometry and flags.
 *
 * @par Callers
 * Likely: func_80030D3C (same block).
 *
 * @par Environment path
 * Spawn step (message 2) of the entity class handled by func_80030D3C
 * (message 1 there reports size 0xB0), which calls it for message 2;
 * func_80030D3C itself has no direct caller, so this runs once per spawned
 * instance. After gEntityBaseHandler(2, ...) it sets the point +0xA0/+0xA8
 * from the doubled record words +0x28/+0x24 and its y +0xA4 to a surface
 * height from func_80067684 plus record +0x2C << 4, then sets the entity's
 * own y (+0x54, copied to +0x60) the same way with record +0x2F/+0x36; the
 * record mode byte maps 0 to the highest surface (-1), 1 to the lowest (-2)
 * and n to index n-2. It also copies the size half-word +0x34 to +0x6C/+0x6E
 * (and << 4 to +0x68), record bytes +0x30/+0x31 and half-word +0x32 to
 * +0xAC/+0xAD/+0xAE, raises flags 0xA00011, clears bit 0x10 of +0x72 for flag
 * 0x80000 and forces +0xAD to 8 when the +0xAC word is 0x10100.
 */
s32 gInitEntityGroundPair(struct Entity *self, struct Spawn *data, s32 arg2)
{
    struct Sub *sub;
    s32 type;
    s32 mode;
    s32 value;
    u16 size;

    gEntityBaseHandler(2, self, data, arg2);
    sub = data->unk08;

    self->unkA8 = sub->unk24 << 1;
    self->unkA0 = sub->unk28 << 1;
    self->unkA4 = 0;

    type = sub->unk2E;
    if (type != 0) {
        if (type == 1) {
            mode = -2;
        } else {
            mode = type - 2;
        }
    } else {
        mode = -1;
    }
    self->unkA4 = func_80067684((struct Vec3 *)&self->unkA0, mode) + (sub->unk2C << 4);

    type = sub->unk2F;
    if (type != 0) {
        if (type == 1) {
            mode = -2;
        } else {
            mode = type - 2;
        }
    } else {
        mode = -1;
    }
    value = func_80067684((struct Vec3 *)&self->unk50, mode) + (sub->unk36 << 4);
    self->unk60 = value;
    self->unk54 = value;

    size = sub->unk34;
    self->unk6C = size;
    self->unk6E = size;
    self->unk68 = (s16)sub->unk34 << 4;
    self->unkAD = sub->unk31;
    self->unkAE = sub->unk32;

    if (self->flags & 0x80000) {
        self->unk72 &= 0xEF;
    }

    self->unkAC = sub->unk30;
    self->flags |= 0xA00011;

    if ((*(u32 *)&self->unkAC & 0xFFFFFF00) == 0x10100) {
        self->unkAD = 8;
    }
    return 0;
}
