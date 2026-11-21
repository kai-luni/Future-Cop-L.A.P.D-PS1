// Categories: Gameplay > Entity Script | Gameplay > Damage | Physics > Collision
#include "game/types.h"

extern int gAttachPartEffect();

/* Second half of the original 8005C344.c: the catalogue treats this as its own
   function (0x8005CA04, 380 bytes), the answer delivered both in one translation unit. */

struct Tgt;
struct Seg;

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Tag {
    u8 unk00[4];
    u8 type;
};

struct Owner {
    u8 unk00[0x20];
    struct Tag *unk20;
    u8 unk24[0x44];
    struct Seg *segs;
    u8 unk6C[0x24];
    s16 step;
};

struct Target {
    u8 unk00[8];
    struct Tgt *unk08;
    u8 unk0C[0xC];
    s32 unk18;
    u16 unk1C;
    u8 unk1E;
    u8 unk1F[9];
    u8 flags;
    u8 unk29;
    u8 unk2A;
    u8 unk2B;
    struct Vec3 pos;
};

struct Mid {
    u8 unk00[0x48];
    struct Target **unk48;
};

struct Obj2 {
    u8 unk00[0xF];
    u8 unkF;
    u8 unk10;
    u8 unk11[0xB];
    struct Mid *unk1C;
    struct Tag *unk20;
    s32 (*handler)(s32 id, struct Obj2 *self, s32 a, s32 b);
    u8 unk28[0x66];
    u16 unk8E;
};

extern s32 gSweepBeamSegments(struct Owner *owner, struct Target *t) __asm__("func_8005C344");
s32 gHandleBeamSweepEffect(u32 msg, struct Obj2 *obj, s32 arg2, s32 arg3) __asm__("func_8005CA04");


/**
 * @brief Message handler for beam/segment-sweep attack instances installed at node +0x24 by gAttachPartEffect.
 *
 * Dispatches messages 1 (tick/sweep), 2 (latch target flag), and 3 (activate flags) when
 * obj->unk20->type (+0x04) == 0x57; message 1 ticks target->unk18 lifetime, retires on
 * underflow or missing target->unk08, and invokes func_8005C344 to advance the segment sweep.
 *
 * @param msg Message id (1 tick, 2 set target flag 1, 3 activate).
 * @param obj Obj2 instance (+0x0F/+0x10 state bytes, +0x1C Mid, +0x20 Tag, +0x24 handler, +0x8E flags).
 * @param arg2,arg3 Elapsed tick delta subtracted from (*obj->unk1C->unk48)->unk18 in msg 1; arg3 unused.
 * @return Always 0.
 *
 * @par Globals
 * None directly (func_8005C344 uses D_800A8F90 and D_8009F374).
 * @par Control flow
 * Switch on msg (1, 2, 3) with nested type == 0x57, lifetime underflow, and null-target checks.
 * @par Calls
 * func_8005C344 - advances segment chain and tests hits against D_800A8F90 in msg 1;
 * indirect obj->handler(8, obj, 0, 0) (+0x24) when t->unk18 drops below 0.
 * @par Side effects
 * Mutates obj flags (+0x0F, +0x10, +0x8E), target lifetime/flags (+0x18, +0x28), and runs segment damage sweep.
 * @par Callers
 * Confirmed: address taken as D_8005CA04 in gAttachPartEffect (0x8005CDE0, stored at node+0x24). / Likely: entity update loop via node+0x24.
 */
s32 gHandleBeamSweepEffect(u32 msg, struct Obj2 *obj, s32 arg2, s32 arg3)
{
    struct Target *t;
    s32 v;

    switch (msg) {
    case 3:
        if (obj->unk20->type == 0x57) {
            obj->unk8E = (obj->unk8E & 0xFFF7) | 1;
        }
        obj->unk10 |= 1;
        break;
    case 1:
        if (obj->unk20->type == 0x57) {
            t = *obj->unk1C->unk48;
            if (t->unk18 >= 0) {
                v = t->unk18 - arg2;
                t->unk18 = v;
                if (v < 0) {
                    t->flags |= 2;
                    if (obj != 0) {
                        obj->handler(8, obj, 0, 0);
                    }
                }
            }
            if (t->unk08 == 0) {
                t->flags |= 2;
                obj->unkF |= 1;
            }
            gSweepBeamSegments((struct Owner *)obj, t);
        }
        return 0;
    case 2:
        (*obj->unk1C->unk48)->flags |= 1;
        break;
    }
    return 0;
}
