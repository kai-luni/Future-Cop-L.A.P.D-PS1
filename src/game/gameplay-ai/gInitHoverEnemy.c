// Categories: Gameplay > AI > Hover Enemy Behaviour | Game Engine > Object Setup | Gameplay > Object Spawning
#include "game/types.h"

struct Unk320E4 {
    u8 unk00[0x16];
    u32 kind;
    u8 unk17[0xB9];
    s32 baseX;
    s32 baseY;
    s32 baseZ;
    s32 targetX;
    s32 targetY;
    s32 targetZ;
    u8 unkE8[0x12];
    u8 state;
    u8 unkFB[0x11];
    s16 spreadZ;
    s16 spreadX;
};

struct AnimClip {
    u8 unk00[4];
    s16 start;
    s16 end;
    u8 unk08[8];
};

struct AnimTrack {
    struct AnimClip *clips;
    struct AnimClip *clip;
    s32 elapsed;
    s16 frame;
    s16 target;
    s8 step;
    u8 index;
    u8 count;
    u8 flags;
};

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk32180Sub {
    u8 pad00[0x10];
    u8 *unk10;
};

struct Unk32180Source {
    u8 pad00[0x34];
    u8 unk34;
    u8 unk35;
    u16 unk36;
    u8 unk38;
    u8 unk39;
    u16 unk3A;
    u16 unk3C;
    u16 unk3E;
    u16 unk40;
    u16 unk42;
    u16 unk44;
    s16 unk46;
    u16 unk48;
    u16 unk4A;
    s16 unk4C;
    s16 unk4E;
    s16 unk50;
    s16 unk52;
};

struct Unk32180Arg {
    u8 pad00[8];
    struct Unk32180Source *unk8;
};

struct Unk32180 {
    u8 pad00[0x28];
    struct Unk32180Sub *unk28;
    u8 unk2C[0x24];
    struct Vec3 pos;
    u8 pad5C[0x10];
    s16 unk6C;
    u8 pad6E[0x0A];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
    u8 pad88[0x40];
    s32 unkC8;
    u8 padCC[4];
    struct Vec3 unkD0;
    struct Vec3 unkDC;
    s32 unkE8;
    s32 unkEC;
    s32 unkF0;
    s32 unkF4;
    s16 unkF8;
    u8 unkFA;
    u8 unkFB;
    u16 unkFC;
    u16 unkFE;
    u16 unk100;
    s16 unk102;
    u8 unk104;
    u8 unk105;
    u16 unk106;
    u16 unk108;
    u16 unk10A;
    s16 unk10C;
    s16 unk10E;
    s16 unk110;
    s16 unk112;
    s16 unk114;
    s16 unk116;
    s32 unk118;
    s32 unk11C;
    struct AnimTrack unk120;
};

extern void gEntityBaseHandler(s32, struct Unk32180 *, struct Unk32180Arg *) __asm__("func_8002AD58");
extern void gInitAttackSchedulerSlot(void *, void *) __asm__("func_80060668");
extern void gPickHoverWanderTarget(struct Unk320E4 *) __asm__("func_800320E4");
extern s32 gGetGroundHeight(s32 *, s32, s32, s32) __asm__("func_80066F78");
extern s32 func_80019668(void);
extern void gSinCos(u32, s32 *, s32 *) __asm__("func_8001846C");
extern void gBuildBasisFromAxis(s32 *, void *) __asm__("func_800193F8");
extern void gSetAnimClipTable(u8 *, struct AnimTrack *, u8, s32) __asm__("func_80044988");
s32 gInitHoverEnemy(s32 arg0, struct Unk32180 *obj, struct Unk32180Arg *arg) __asm__("func_80032180");


/**
 * @brief Message-2 spawn initialiser of the hovering flyer class: copies the
 *        spawn record into the object, places it on the terrain, prepares the
 *        wander point and the altitude band and starts the animation.
 *
 * @param arg0 Message id; both call sites pass 2 and the body never reads it.
 * @param obj Object record; the fields written here reach from +0x78 to +0x120
 *        and the model instance pointer sits at +0x28.
 * @param arg Spawn record wrapper; its +0x08 points at the source record, whose
 *        bytes +0x24..+0x52 are read.
 * @return Always 0.
 *
 * @par Confirmed
 * func_8002AD58(arg0, obj, arg) runs first - the class-independent handler,
 * whose message-2 case installs the owner/team data - then func_80060668 copies
 * the 16 bytes at source +0x24 to obj +0xA0.
 * State and timers: +0xFA = source +0x38 (kind), remapped to 1 when it is 2;
 * +0xFB = source +0x39 (flag byte); +0xFC = (source +0x4C * 75) >> 6; +0xFE =
 * (source +0x4E * 75) >> 6; +0x100 = +0xFA ? +0xFE : +0xFC; +0x102 = -1;
 * +0x116 = -1; +0xC8 = 0.
 * Parameters: +0x104 = source +0x34, +0x105 = source +0x35, +0x106 = source
 * +0x36 (the class triple the tick later hands to func_8004E80C), +0x108 =
 * source +0x3A, +0x10A = source +0x3C, +0x10C = (s16)source +0x48 >> 5, +0x10E =
 * (s16)source +0x4A >> 5, +0x110 = source +0x3E, +0x112 = source +0x42, +0x114 =
 * source +0x40 (the altitude band), +0xF8 = source +0x44, +0xF4 = (source +0x46
 * << 12) / 300, +0x84 = +0xF4 >> 8.
 * Placement: with bit 3 of +0xFB the spawn x/z are taken from source +0x52 and
 * +0x50 shifted left by 9; y is then the terrain height from
 * func_80066F78(&obj->+0x50, 0, 0, 0) plus (+0x114 << 4), and +0xD0 = the
 * position, which makes +0xD0 the wander anchor. With bit 0 of +0xFB
 * func_800320E4 re-rolls a target around +0xD0 into +0xDC, the position is taken
 * from +0xDC and y is recomputed the same way. Bit 1 adds 0x20000 to y, or,
 * when bit 1 is clear and bit 2 set, switches +0xFA to 4 and sets y from the
 * height probed before the +0xDC copy plus (+0x6C << 4) - a stale value the code
 * reuses as written.
 * Then +0xE8/+0xEC/+0xF0 are cleared, func_8001846C turns a random 12-bit angle
 * into (sine, cosine) at +0x78 and +0x80 with +0x7C = 0, func_800193F8 builds
 * the basis at +0x2C from that heading, and func_800320E4 runs a second time.
 * +0x118 = 0x80000000 / ((+0x112 - +0x114) << 3) and +0x11C = 0x80000000 /
 * ((+0x114 - +0x110) << 3) - the two unsigned reciprocals the altitude clamp
 * later multiplies by. Finally, when the model instance at +0x28 reports +0x10
 * non-zero, func_80044988 attaches that data to the animation track at +0x120.
 * The callers declare four parameters and pass (2, obj, arg2, arg3); this
 * definition uses three, so the fourth is dropped in the original as well.
 *
 * @par Likely role
 * The spawn half of the flyer whose per-frame half is func_800324E0: the fields
 * written here (+0xFA state, +0xFB spawn flags, +0xFC/+0xFE timers, +0x104..
 * +0x106 search class, +0x110/+0x112/+0x114 altitude band, +0x118/+0x11C
 * reciprocals, +0x120 animation) are exactly the ones that tick consumes, and
 * the wander point it seeds at +0xD0/+0xDC is where the flyer loiters between
 * attacks. +0xFB reads like a placement selector: a scripted x/z, a random
 * wander spot, a higher spawn or a lower one.
 *
 * @par Unknown
 * What the class triple +0x104..+0x106 means in game terms, why kind 2 becomes 1
 * and why the two timers are scaled by 75/64, and whether +0xF8 (source +0x44)
 * is a radius or a period.
 *
 * @par Callers
 * func_80032F3C (message 2 of the class message table) and func_800331CC.
 */
s32 gInitHoverEnemy(s32 arg0, struct Unk32180 *obj, struct Unk32180Arg *arg)
{
    struct Unk32180Source *src;
    s32 *pos;
    s32 value;
    s32 sc[2];
    u32 kind;

    gEntityBaseHandler(arg0, obj, arg);
    src = arg->unk8;
    gInitAttackSchedulerSlot((u8 *)src + 0x24, (u8 *)obj + 0xA0);

    kind = src->unk38;
    obj->unkFA = kind;
    if (kind == 2) {
        obj->unkFA = 1;
    }

    obj->unkFB = src->unk39;
    obj->unkFC = (src->unk4C * 75) >> 6;
    obj->unkFE = (src->unk4E * 75) >> 6;
    obj->unk100 = obj->unkFA ? obj->unkFE : obj->unkFC;
    obj->unk102 = -1;
    obj->unk116 = -1;
    obj->unkC8 = 0;

    obj->unk104 = src->unk34;
    obj->unk105 = src->unk35;
    obj->unk106 = src->unk36;
    obj->unk10A = src->unk3C;
    obj->unk108 = src->unk3A;
    obj->unk10C = (s16)src->unk48 >> 5;
    obj->unk10E = (s16)src->unk4A >> 5;
    obj->unk110 = src->unk3E;
    obj->unk114 = src->unk40;
    obj->unk112 = src->unk42;
    obj->unkF8 = src->unk44;
    obj->unkF4 = (src->unk46 << 12) / 300;
    obj->unk84 = obj->unkF4 >> 8;

    if (obj->unkFB & 8) {
        obj->pos.z = src->unk50 << 9;
        obj->pos.x = src->unk52 << 9;
    }

    pos = &obj->pos.x;
    value = gGetGroundHeight(pos, 0, 0, 0);
    obj->pos.y = value + (obj->unk114 << 4);
    obj->unkD0 = obj->pos;

    if (obj->unkFB & 1) {
        gPickHoverWanderTarget((struct Unk320E4 *)obj);
        obj->pos = obj->unkDC;
        value = gGetGroundHeight(pos, 0, 0, 0);
        obj->pos.y = value + (obj->unk114 << 4);
    }

    if (obj->unkFB & 2) {
        obj->pos.y += 0x20000;
    } else if (obj->unkFB & 4) {
        obj->unkFA = 4;
        obj->pos.y = value + (obj->unk6C << 4);
    }

    obj->unkE8 = 0;
    obj->unkEC = 0;
    obj->unkF0 = 0;

    gSinCos(func_80019668() & 0xFFF, &sc[0], &sc[1]);
    obj->unk78 = sc[0];
    obj->unk80 = sc[1];
    obj->unk7C = 0;
    gBuildBasisFromAxis(&obj->unk78, obj->unk2C);
    gPickHoverWanderTarget((struct Unk320E4 *)obj);

    obj->unk118 = (u32)0x80000000 / (u32)((obj->unk112 - obj->unk114) << 3);
    obj->unk11C = (u32)0x80000000 / (u32)((obj->unk114 - obj->unk110) << 3);

    if (obj->unk28->unk10 != 0) {
        gSetAnimClipTable(obj->unk28->unk10, &obj->unk120, 1, 0);
    }

    return 0;
}
