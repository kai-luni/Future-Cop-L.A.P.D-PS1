// Categories: Gameplay > Object Spawning | Gameplay > Combat
#include "game/types.h"

struct Unk5EE04Vec {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk5EE04Ent {
    u8 unk00[0x14];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19[0x0B];
    u32 unk24;
    u8 unk28[0x50];
    struct Unk5EE04Vec unk78;
    u32 unk84;
};

struct Unk5EE04Obj {
    u16 unk00;
    s16 unk02;
    u16 unk04;
    s16 unk06;
    u8 unk08;
    u8 unk09;
    s16 unk0A;
    s16 unk0C;
    u8 unk0E;
    u8 unk0F[3];
    u8 unk12;
};

struct Unk5EE04Args {
    u32 flags;
    u8 kind;
    u8 unk05[3];
    struct Unk5EE04Vec position;
    u32 unk14;
    u16 unk18;
    u16 unk1A;
    u8 unk1C;
    u8 unk1D;
    u8 unk1E;
    u8 unk1F;
    u8 unk20;
    u8 unk21;
    u8 unk22;
    u8 unk23;
    u32 unk24;
    struct Unk5EE04Vec unk28;
    u32 unk34;
    u32 unk38;
    u32 unk3C;
    u32 unk40;
    u32 unk44;
    u8 unk48;
    u8 unk49;
    u16 unk4A;
};

struct Unk5EE04Request {
    void (*handler)();
    u32 unk04;
    struct Unk5EE04Args *args;
    u32 unk0C;
};

extern void D_80036E6C();
extern void *func_8004D444(struct Unk5EE04Request *request);

/**
 * @brief Packs a kind-7 entity spawn request with handler D_80036E6C and queues creation via func_8004D444.
 *
 * Confirmed: builds a request referencing D_80036E6C (func_80036E6C) and a 0x4C-byte args block with kind = 7, unk18 = 0x28, flags = ent->unk14 | 0x8000 (+0x4000 if ent->unk16 == 0x12), unk14 = flags | 0x8 (+0x100 if ent->unk24 & 0x100), model **ptr, position *pos, and target arg4, then calls func_8004D444; arg5 is unused. Category Gameplay > Object Spawning | Gameplay > Combat follows spawn queue func_8004D444 and weapon-dispatch callers. Likely: spawns a guided/linked attack entity (case 6). Unknown: meaning of unk18 = 0x28, obj fields, and why arg5 is unused.
 *
 * @param obj Weapon descriptor (+0x00, +0x02, +0x0A, +0x0E, +0x12 copied/shifted into args).
 * @param ptr Double pointer to model handle (**ptr stored in args.unk24).
 * @param pos Launch position vector copied into args.position.
 * @param ent Firing entity (+0x14 flags, +0x16 kind, +0x17/+0x18 bytes, +0x24 bit 0x100); saved in args.unk44.
 * @param arg4 Target/link pointer stored in args.unk40.
 * @param arg5 Unused parameter (passed as target position by callers).
 * @param flags Base flag word combined with 0x8 and optionally 0x100 into args.unk14.
 * @return Pointer returned by func_8004D444 (newly spawned entity).
 * @par Globals
 * None directly accessed; takes address of D_80036E6C (func_80036E6C).
 * @par Control flow
 * Straight-line struct initialization with two conditional flag checks on ent->unk16 == 0x12 and ent->unk24 & 0x100, ending in func_8004D444.
 * @par Calls
 * func_8004D444 - allocates entity via D_80036E6C message 1, assigns id, runs message 2 init, and links into active list.
 * @par Side effects
 * Allocates and links a new func_80036E6C entity through func_8004D444.
 * @par Callers
 * Confirmed: func_8005F6E8 (0x8005F9A4, action case 6) and func_8006ACB0 (0x8006B0CC, action case 6) / Likely: none / Unknown: none.
 */
void *func_8005EB74(struct Unk5EE04Obj *obj, s32 **ptr, struct Unk5EE04Vec *pos,
                    struct Unk5EE04Ent *ent, u32 arg4, void *arg5, u32 flags)
{
    struct Unk5EE04Request request;
    struct Unk5EE04Args args;

    request.handler = D_80036E6C;
    request.unk04 = 0;
    request.args = &args;
    request.unk0C = 0;

    args.flags = ent->unk14 | 0x8000;
    if (ent->unk16 == 0x12) {
        args.flags |= 0x4000;
    }
    args.unk1D = ent->unk17;
    args.unk1C = ent->unk18;
    args.unk14 = flags | 0x8;
    if (ent->unk24 & 0x100) {
        args.unk14 = flags | 0x108;
    } else {
        args.unk14 &= ~0x100;
    }
    args.kind = 7;
    args.unk18 = 0x28;
    args.unk1A = obj->unk00;
    args.unk38 = obj->unk02 << 4;
    args.unk34 = obj->unk0A << 4;
    args.unk1E = 0;
    args.unk22 = 0;
    args.unk23 = 0;
    args.unk1F = 0;
    args.unk24 = **ptr;
    args.unk48 = obj->unk0E;
    args.unk3C = obj->unk12;
    args.unk40 = arg4;
    args.unk44 = (u32)ent;
    args.unk20 = 0;
    args.unk28.x = 0;
    args.position = *pos;
    args.unk28.y = 0;
    args.unk28.z = 0;

    return func_8004D444(&request);
}
