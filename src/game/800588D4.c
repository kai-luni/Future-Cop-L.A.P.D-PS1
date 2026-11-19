// Categories: Gameplay > Entity Update | Gameplay > Damage | Graphics > Model Renderer | Gameplay > Entity Script
#include "game/types.h"

struct Unk5A190;
struct Unk584BC;
struct Unk55E10;

struct Unk588D4Sub28 {
    u8 unk00[0x34];
    u8 *unk34;
};

struct Unk588D4SubDC {
    u8 unk00[0x24];
    u32 unk24;
};

struct Unk588D4Global {
    u8 unk00[0x5C];
    s32 unk5C;
    u8 unk60[0x24];
    s32 unk84[2];
};

struct Unk588D4Data {
    u8 unk00[0x24];
    u32 unk24;
    s32 unk28[17];
    s32 unk6C;
    u8 unk70[0x3D];
    s8 unkAD;
    u8 unkAE[0x13];
    s8 unkC1;
    u8 unkC2[0x1C];
    u8 unkDE;
    u8 unkDF[0x21D];
    s8 unk2FC;
    u8 unk2FD[0x1B];
    s16 unk318;
    s16 unk31A;
    u8 unk31C[8];
    s16 unk324;
    s16 unk326;
};

struct Unk588D4 {
    u8 unk00[0x0C];
    u8 unk0C;
    u8 unk0D[0x07];
    u16 unk14;
    u8 unk16[0x06];
    s16 unk1C;
    u8 unk1E[0x06];
    u32 unk24;
    struct Unk588D4Sub28 *unk28;
    u8 unk2C[0x24];
    u8 unk50[0x04];
    s32 unk54;
    u8 unk58[0x04];
    u8 unk5C[0x14];
    s16 unk70;
    u8 unk72[0x3A];
    struct Unk588D4Data *unkAC;
    u8 unkB0[0x06];
    s8 unkB6;
    s8 unkB7;
    u8 unkB8[0x24];
    struct Unk588D4SubDC *unkDC;
};

struct Unk588D4Msg {
    u8 unk00[0x44];
    u8 *unk44;
    u8 unk48;
    u8 unk49;
    u8 unk4A[6];
};

struct Unk584BCHitLocal {
    u32 unk00;
    s16 damage;
    u8 kind;
    u8 unk07;
    union {
        void *attacker;
        s32 source;
    } from;
    u8 unk0C[4];
};

struct Unk588D4Locals {
    s32 out18;
    s32 out1C;
    struct Unk588D4Msg msg;
    struct Unk584BCHitLocal hit;
};

extern s32 gScreenState __asm__("D_8009FA00");
extern struct Unk588D4Global *gGameSession __asm__("D_8009FA94");
extern u32 gFrameTime __asm__("D_8009FAA4");

extern void func_8005A190(struct Unk5A190 *object, s32 index, s32 value);
extern void func_800585B8(struct Unk588D4 *object);
extern s32 gGetGroundHeight(void *object, s32 arg1, s32 arg2, s32 *arg3) __asm__("func_80066F78");
extern void func_80058378(struct Unk584BC *object, s32 amount, s32 source);
extern void gRespawnPlayerEntity(struct Unk588D4 *object) __asm__("func_80058714");
extern void gTranslatePadButtons(void *object) __asm__("func_800576A8");
extern s32 func_800586D4(void *object, s32 arg1, s32 arg2, s32 *arg3);
extern s32 gSpawnTypeAtOwner(void *owner, u8 column, void *arg2, s32 row, s32 arg4) __asm__("func_800402DC");
extern s32 func_80046C84(s32 id, s32 arg1);
extern s32 func_800584BC(s32 message, struct Unk584BC *object,
                         struct Unk584BCHitLocal *hit, s32 arg3, s32 arg4);
extern void func_8006C34C(struct Unk55E10 *object, s32 arg1);
extern void gGteLoadMatrix(void *arg) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *arg0, void *arg1, s32 arg2) __asm__("func_80010234");
extern void gSlewEntityAim(void *object, s32 arg1) __asm__("func_80057E40");
extern void func_8006C3E0(void *object, s32 arg1);
extern void gWeaponSlotInput(struct Unk55E10 *object, s32 arg1) __asm__("func_80055E10");
extern void gUpdateChaseCamera(void *object, s32 arg1, s32 arg2) __asm__("func_800569EC");
s32 gUpdatePlayerEntity(struct Unk588D4 *object, s32 arg1) __asm__("func_800588D4");


/**
 * @brief Per-frame tick of a player/vehicle entity: services timers, contact damage and
 *        hit reporting, runs the sub-updates, refreshes the basis matrix and world
 *        position, and drives the controller polling and the camera.
 *
 * @param object Entity block. +0xAC is the shared state block, +0xDC a companion block,
 *        +0x28 a source block whose +0x34 is vertex data, +0x50 the collision body,
 *        +0x54 a stored distance, +0x14 a reference number, +0x1C a value handed to the
 *        sub-update, +0x24 and +0x2C two flag/basis words, +0x5C a matrix block, +0x70
 *        a part index, +0xB6/+0xB7 two fractional clock accumulators.
 * @param arg1 Elapsed ticks for this frame; subtracted from the timers and added to the
 *        clock accumulators.
 * @return Always 0, including the early return.
 *
 * @par Confirmed order
 * 1. state + 0x2FC is set to -1 and func_8005A190(state, 0, object + 0x1C) re-arms the
 *    model; a non-zero +0x0C triggers func_800585B8.
 * 2. With state + 0xDE set, func_80066F78(object + 0x50, 0, 0, out) measures the ground
 *    distance, and a value beyond object + 0x54 + 0x3800 adds one point of damage
 *    through func_80058378(object, object + 0x1C + 1, 0).
 * 3. Timers state + 0x31A and +0x318 count down; when +0x318 reaches zero the flag
 *    words object + 0x24 and companion + 0x24 lose mask 0xA6, func_80058714 runs and
 *    the function returns.
 * 4. Two accumulators build the sub-tick arguments: +0xB6 keeps the low three bits and
 *    yields eighths of arg1, +0xB7 keeps the low bit and yields halves.
 * 5. In game mode D_8009FA94 + 0x5C == 1 a counter at state + (state + 0xAD shifted 2)
 *    + 0x28 is advanced by arg1, and while state + 0x24 is still zero and
 *    D_8009FA94 + 0x84[index] is positive it is stamped from D_8009FAA4.
 *
 * @par Confirmed gameplay half
 * For an index below 2 and screen state D_8009FA00 == 4, func_800576A8(state + 0x30)
 * transforms the input word and func_800586D4 returns a damage amount. A set bit 0 in
 * its out word posts a message record (fields 0x40 and state + 0x28) through
 * func_800402DC(object, 4, record, 1, 0) and plays sound func_80046C84(0x15, object);
 * a non-zero damage posts a hit record (kind 1, the damage, object as attacker, object
 * + 0x14 as reference) through func_800584BC(0x0E, object, record, 0, 0). A zero
 * state + 0x6C then makes func_8006C34C(object, 1) run.
 *
 * @par Confirmed transform half
 * func_80010750 loads object + 0x2C into the GTE and func_80010234 transforms the
 * vertex at source + 0x34, offset by (part index shifted 4) + 8, into object + 0x5C.
 * The sub-systems follow: func_80057E40(companion, arg1), func_8006C3E0(object, arg1)
 * for its three value words, gWeaponSlotInput only in screen state 4 - the controller
 * polling - and func_800569EC(object, arg1, halves) for the camera. Timers
 * state + 0x324 and +0x326 are decremented last.
 *
 * @par Likely role
 * The message 6 handler of a player-controlled unit: fall damage, hit feedback to the
 * scripting layer, input and chase-camera updates on top of the transform refresh.
 *
 * @par Unknown
 * Whether the counter stepped at state + 0xAD is wall time or a scoring clock, and the
 * meaning of the 0xA6 flag mask.
 *
 * @par Callers
 * func_8005908C, message 6 of its switch - the entity script dispatcher of this class.
 *
 * @par Environment path
 * Per-frame tick of the player unit, called from its class handler
 * func_8005908C: it resets state +0x2FC, re-arms the model with func_8005A190
 * and, when state byte +0xDE is set, kills the unit through func_80058378
 * with damage health + 1 if the ground height from func_80066F78 lies more
 * than 0x3800 above it. It counts the +0x31A/+0x318 timers down (returning
 * early while +0x318 runs and clearing the hit flags when it expires), splits
 * the step into the +0xB6/+0xB7 sub-tick accumulators, and in game mode 1
 * (session +0x5C) adds the step to the per-player time and stamps +0x24 with
 * the frame time D_8009FAA4. For player slots 0/1 in screen state 4 it runs
 * the movement/collision half (func_800576A8, func_800586D4), posting a
 * message via func_800402DC or self-damage via func_800584BC, then refreshes
 * the frame vertex and drives func_80057E40, func_8006C3E0, gWeaponSlotInput
 * (func_80055E10, screen state 4 only) and, unconditionally,
 * gUpdateChaseCamera (func_800569EC).
 */
s32 gUpdatePlayerEntity(struct Unk588D4 *object, s32 arg1)
{
    struct Unk588D4Data *data;
    register s32 s2 __asm__("$18");
    register s32 s4 __asm__("$20");
    register s32 v __asm__("$2");
    register u32 hitcode __asm__("$3");
    register s32 hitarg __asm__("$7");
    register s32 offset __asm__("$4");
    struct Unk588D4Locals locals;
    s32 ret;
    s32 c1;

    object->unkAC->unk2FC = -1;
    func_8005A190((struct Unk5A190 *)object->unkAC, 0, object->unk1C);

    if (object->unk0C != 0) {
        func_800585B8(object);
    }

    if (object->unkAC->unkDE != 0) {
        ret = gGetGroundHeight(object->unk50, 0, 0, &locals.out18);
        if (object->unk54 + 0x3800 < ret) {
            func_80058378((struct Unk584BC *)object, object->unk1C + 1, 0);
        }
    }

    if (object->unkAC->unk31A > 0) {
        object->unkAC->unk31A -= arg1;
    }

    if (object->unkAC->unk318 > 0) {
        object->unkAC->unk318 -= arg1;
        if (object->unkAC->unk318 > 0) {
            return 0;
        }
        object->unk24 &= -0xA6;
        object->unkDC->unk24 &= -0xA6;
        gRespawnPlayerEntity(object);
        return 0;
    }

    v = object->unkB6;
    data = *(struct Unk588D4Data *volatile *)&object->unkAC;
    s2 = arg1 + v;
    object->unkB6 = s2 & 7;
    s2 >>= 3;
    v = object->unkB7;
    s4 = arg1 + v;
    object->unkB7 = s4 & 1;
    s4 >>= 1;

    c1 = data->unkC1;
    if (gGameSession->unk5C == 1) {
        *(s32 *)((u32)data + (data->unkAD << 2) + 0x28) += arg1;
        if (object->unkAC->unk24 == 0 && *(s32 *)((c1 << 2) + (u32)gGameSession + 0x84) > 0) {
            object->unkAC->unk24 = gFrameTime;
        }
    }

    if (c1 < 2) {
        if (gScreenState == 4) {
            gTranslatePadButtons((u8 *)data + 0x30);
            s2 = func_800586D4(object, arg1, s2, &locals.out1C);
            if (locals.out1C & 1) {
                locals.msg.unk49 = 0x40;
                locals.msg.unk44 = (u8 *)object->unkDC + 0x28;
                gSpawnTypeAtOwner(object, 4, &locals.msg, 1, 0);
                func_80046C84(0x15, object);
            }
            if (s2 != 0) {
                hitarg = 0;
                hitcode = object->unk14;
                locals.hit.kind = 1;
                locals.hit.damage = s2;
                locals.hit.from.attacker = object;
                locals.hit.unk00 = hitcode;
                func_800584BC(0xE, (struct Unk584BC *)object, &locals.hit, hitarg, 0);
            }
        }
        if (data->unk6C == 0) {
            func_8006C34C((struct Unk55E10 *)object, 1);
        }
    }

    gGteLoadMatrix(object->unk2C);
    offset = object->unk70 << 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    offset += 8;
    v = (s32)object->unk28->unk34;
    gGteTransformVerticesWide((void *)(v + offset), object->unk5C, 1);
    gSlewEntityAim(object->unkDC, arg1);
    func_8006C3E0(object, arg1);
    if (gScreenState == 4) {
        gWeaponSlotInput((struct Unk55E10 *)object, arg1);
    }
    gUpdateChaseCamera(object, arg1, s4);

    if (data->unk324 > 0) {
        data->unk324 -= arg1;
    }
    if (data->unk326 > 0) {
        data->unk326 -= arg1;
    }
    return 0;
}
