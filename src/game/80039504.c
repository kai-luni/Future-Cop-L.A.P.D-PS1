// Categories: Gameplay > Object Spawning | Gameplay > Entity Script
#include "game/types.h"

struct Unk39504Sub04 {
    u8 pad00[0x20];
    s32 unk20;
};

struct Unk39504Sub08 {
    u8 pad00[0x48];
    s16 unk48;
    s16 unk4A;
    union {
        s32 unk4C_word;
        struct {
            u8 unk4C;
            u8 unk4D;
        } b;
    } u;
};

struct Unk39504Other {
    s32 unk00;
    struct Unk39504Sub04 *unk04;
    struct Unk39504Sub08 *unk08;
};

struct Unk39504Child {
    u8 pad00[0x24];
    s32 unk24;
};

struct Unk39504Self {
    u8 pad00[0xA0];
    s32 unkA0;
    u8 padA4[0xB4];
    struct Unk39504Child *unk158;
};

extern void gHandleType5Entity(s32 arg0, struct Unk39504Self *self, struct Unk39504Other *other, s32 arg3) __asm__("func_800382B4");
extern struct Unk39504Child *gSpawnMountedPartChild(struct Unk39504Self *self, s32 arg1, s16 arg2, s32 arg3, u8 arg4, u8 arg5, s32 arg6) __asm__("func_80039418");
extern void gQueueOrApplyMapOccupancy(struct Unk39504Child *child) __asm__("func_8003FA34");
void gSetupMountedTurretCarrier(s32 arg0, struct Unk39504Self *self, struct Unk39504Other *other, s32 arg3) __asm__("func_80039504");


/**
 * @brief Initializes a class-5 entity and spawns and registers its kind-0x15 child object.
 *
 * Forwards setup to func_800382B4, spawns a child object via func_80039418 from fields in
 * other->unk08/unk04, clears self->unkA0, sets flag 0x10 on the child, and registers it.
 *
 * @param arg0 Message code forwarded to func_800382B4 (2 during spawn).
 * @param self Class-5 entity receiving the child pointer at +0x158 and cleared +0xA0.
 * @param other Spawn descriptor carrying sub-records unk04 (+0x04) and unk08 (+0x08).
 * @param arg3 Context word forwarded to func_800382B4.
 *
 * @par Globals
 * None directly.
 * @par Control flow
 * Straight-line sequence: base handler call, flag extraction ((unk4C_word & 6) == 2) << 1, child spawn, and child registration.
 * @par Calls
 * func_800382B4 (base setup), func_80039418 (spawns kind-0x15 child at +0x158), func_8003FA34 (links child into active/track list).
 * @par Side effects
 * Writes self->unk158 and self->unkA0 = 0, ORs 0x10 into child->unk24, and registers child via func_8003FA34.
 * @par Callers
 * Confirmed: func_800395A8 (message 2 spawn handler).
 */
void gSetupMountedTurretCarrier(s32 arg0, struct Unk39504Self *self, struct Unk39504Other *other, s32 arg3)
{
    struct Unk39504Sub08 *data;
    s32 flags;

    gHandleType5Entity(arg0, self, other, arg3);
    data = other->unk08;
    flags = ((data->u.unk4C_word & 6) == 2) << 1;
    self->unk158 = gSpawnMountedPartChild(self, data->unk48 << 8, data->unk4A, flags, data->u.b.unk4C, data->u.b.unk4D, other->unk04->unk20);
    self->unkA0 = 0;
    self->unk158->unk24 |= 0x10;
    gQueueOrApplyMapOccupancy(self->unk158);
}
