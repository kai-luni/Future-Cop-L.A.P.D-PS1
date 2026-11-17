// Categories: Gameplay > Entity Script | Graphics > Animation | Game Engine > Entity Lifecycle
#include "game/types.h"

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

struct ModelFrame {
    u16 unk00;
    s16 unk02;
    s16 radius;
    u16 unk06;
    u8 unk08[8];
};

struct ModelFrames {
    u8 header[0x10];
    struct ModelFrame frames[1];
};

struct Model {
    u8 unk00[0x34];
    struct ModelFrames *frames;
};

struct ModelInstance {
    struct Model *model;
    u8 coord[0x3C];
    s32 radius;
    u16 unk44;
    u16 unk46;
    s16 frame;
    u8 flags;
};

struct ObjD0_306A0 {
    u8 pad00[0x14];
    u16 unk14;
    u8 unk16;
    u8 pad17[0xAC - 0x17];
    u32 unkAC;
    u8 padB0[0xDC - 0xB0];
    u32 unkDC;
};

struct Actor306A0 {
    u8 pad00[0x24];
    u32 flags24;
    struct ModelInstance instance28;
    u8 pad28[0x28];
    void *ptr9C;
    struct AnimTrack animA0;
    u32 unkB4;
    u8 padB8[0x02];
    s16 unkBA;
    u8 unkBC;
    u8 stateBD;
    s16 unkBE;
    s32 unkC0;
    s16 unkC4;
    u8 padC6[0x02];
    u16 unkC8;
    u8 padCA[0x02];
    u16 unkCC;
    u8 padCE[0x02];
    struct ObjD0_306A0 *ptrD0;
    struct ModelInstance instanceD4;
    u8 padD4[0x58 - sizeof(struct ModelInstance)];
    s16 unk12C;
};

extern u32 gDiscShadeByte __asm__("D_8009FAA4");
extern void *jtbl_800111DC[];

extern void func_8002FC90(void *, u32, s16, s32, s32);
extern void func_80069D68(void *, s32, u32, void *);
extern s32 func_800402DC(void *, u8, s32, s32, s32);
extern void *func_800448D8(struct AnimTrack *, s32, s32, u8);
extern void func_800175F8(struct ModelInstance *, s32);
extern void func_800188D0(s16, s32, s32, void *);
extern s32 func_80044B34(void *, void *, s32, s32);
extern void func_800302E8(struct Actor306A0 *, s32, s32);
extern u8 func_8002F8B8(u8, s16, s32, s32);
extern void func_80030498(struct Actor306A0 *, s32);
extern s32 func_8002AD58(s32, struct Actor306A0 *, s32);
extern u8 func_80017508(struct ModelInstance *, u32 *, s32);

/**
 * @brief Per-frame state machine of an animated actor: impact countdown, two
 *        animation tracks, a spin angle and a timed transition.
 *
 * @param actor Actor block: +0x24 flag word, +0x28 and +0xD4 the two model
 *        instances (+0x12C the frame of the second), +0x9C the bytecode script,
 *        +0xA0 the animation track, +0xB4 the effect flags, +0xBC and
 *        +0xBD two state bytes (+0xBD the machine state), +0xBE the delay,
 *        +0xC0 the timer, +0xC4 the effect value, +0xC8 the spin angle,
 *        +0xCC the countdown, +0xD0 the owner object and +0xD8 a matrix
 *        destination.
 * @param delta Frame step, subtracted from the countdown and the timer and
 *        multiplied into the spin.
 *
 * @return Always 0.
 *
 * @par Confirmed
 * State 5 with a countdown of exactly 0x190 runs the impact step once:
 * func_8002FC90(owner, +0xB4, +0xC4, 0, 0) applies the effect record, the script
 * at +0x9C is executed with func_80069D68 unless flag 0x100000 of +0x24 is set,
 * and when the owner's +0x16 is 1 the word at owner->+0xAC + 0xA0 is raised and,
 * if owner->+0xDC is non-zero, func_800402DC spawns a cell object with a
 * hand-built parent block (byte 0x40 at +0x49, block owner->+0xDC + 0x28 at
 * +0x44) into column 0xC8 of row 1. +0xCC then loses delta and, at zero, with a
 * valid owner whose +0x16 is 1, clears the same word, starts clip 3 through
 * func_800448D8(+0xA0, 3, 1, 1) and moves to state 3.
 * In states 2 and 5 the second instance animates when its model's +0x10 word is
 * non-zero: func_80044B34(0, actor + 0x120, delta, 0) then
 * func_800175F8(+0xD4, +0x12C). The spin angle +0xC8 then decreases by
 * delta << 4 when +0xB4 & 0xF0 is set and increases by delta << 2 otherwise,
 * both masked with 0xFFF, and func_800188D0 supplies the rotation matrix at
 * +0xD8.
 * The main track always advances: temp = func_80044B34(actor, +0xA0, delta, 0),
 * func_800175F8(+0x28, track frame), then the low six bits of the event mask
 * select state 2 (event 1) or state 4 with timer = +0xBE * 300 (event 2, only
 * when the delay is non-negative). State 4 with a non-negative delay drains
 * +0xC0 and at zero restarts clip 0 (func_800448D8(+0xA0, 0, 1, 0)) and returns
 * to state 0.
 *
 * @par Likely role
 * The actor's animation-driven behaviour machine, called from the tick message
 * of its handler: state 5 is the hit/dying latch (one effect application, one
 * script run, one spawned cell object and a 400-tick countdown), state 4 a
 * delayed transition armed by clip event 2, and the spin of the secondary model
 * the idle or turn animation whose direction follows the +0xB4 nibble.
 *
 * @par Unknown
 * What +0xB4's low and high nibbles mean beyond selecting the spin direction,
 * what +0xC4 is inside func_8002FC90 (and why its accumulator arguments are
 * passed as null, which only works while flag 0x1000000 of +0xB4 is set), and
 * what the 0xC8 column of the spawned cell object selects.
 *
 * @par Callers
 * func_80030960, case 6 of its jump table (the object-type-32 tick message).
 */
s32 func_800306A0(struct Actor306A0 *actor, s32 delta)
{
    struct ObjD0_306A0 *obj;
    u8 local[0x50];
    u32 temp;

    if (actor->stateBD == 5) {
        if ((s16)actor->unkCC == 0x190) {
            obj = actor->ptrD0;
            func_8002FC90(obj, actor->unkB4, actor->unkC4, 0, 0);
            if (actor->ptr9C != 0 && *(u8 *)actor->ptr9C != 0 && (actor->flags24 & 0x100000) == 0) {
                func_80069D68(actor->ptr9C, 0, obj->unk14 & 0x7FFF, actor);
            }
            if (obj->unk16 == 1) {
                *(u16 *)(obj->unkAC + 0xA0) = 1;
                if (obj->unkDC != 0) {
                    local[0x49] = 0x40;
                    *(u32 *)(local + 0x44) = obj->unkDC + 0x28;
                    func_800402DC(obj, 0xC8, (s32)local, 1, 0);
                }
            }
        }
        actor->unkCC = actor->unkCC - delta;
        if ((s16)actor->unkCC <= 0) {
            register struct ObjD0_306A0 *obj2 __asm__("$7");
            obj2 = actor->ptrD0;
            if (obj2 != 0 && obj2->unk16 == 1) {
                *(u16 *)(obj2->unkAC + 0xA0) = 0;
                func_800448D8(&actor->animA0, 3, 1, 1);
                actor->stateBD = 3;
            }
        }
    }

    if (actor->stateBD == 2 || actor->stateBD == 5) {
        if (*(u32 *)((u8 *)actor->instanceD4.model + 0x10) != 0) {
            s32 zero = 0;
            func_80044B34(zero, (u8 *)actor + 0x120, delta, zero);
            func_800175F8(&actor->instanceD4, actor->unk12C);
        }
        if (actor->unkB4 & 0xF0) {
            actor->unkC8 = (actor->unkC8 - (delta << 4)) & 0xFFF;
        } else {
            actor->unkC8 = (actor->unkC8 + (delta << 2)) & 0xFFF;
        }
        func_800188D0((s16)actor->unkC8, 0, 0, (u8 *)actor + 0xD8);
    }

    temp = func_80044B34(actor, &actor->animA0, delta, 0);
    {
        register u32 shift __asm__("$16");
        register struct ModelInstance *inst __asm__("$4");
        register s32 frame __asm__("$5");
        shift = temp;
        inst = &actor->instance28;
        frame = actor->animA0.frame;
        __asm__ volatile ("" : "+r" (shift), "+r" (inst), "+r" (frame));
        shift >>= 1;
        func_800175F8(inst, frame);
        temp = shift;
    }
    temp &= 0x3F;

    switch (temp) {
    case 1:
        actor->stateBD = 2;
        break;
    case 2:
        actor->stateBD = 4;
        if (actor->unkBE >= 0) {
            actor->unkC0 = actor->unkBE * 300;
        }
        break;
    }

    if (actor->stateBD == 4 && actor->unkBE >= 0) {
        actor->unkC0 -= delta;
        if (actor->unkC0 <= 0) {
            func_800448D8(&actor->animA0, 0, 1, 0);
            actor->stateBD = 0;
        }
    }

    return 0;
}

