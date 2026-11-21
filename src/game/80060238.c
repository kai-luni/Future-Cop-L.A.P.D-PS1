// Categories: Gameplay > AI > Attack Scheduling | Graphics > Animation > Timeline | Game Engine > Frame
#include "game/types.h"

typedef struct Func80060238Inner {
    u8 pad00[1];
    u8 unk01;
    u8 pad02[4];
    u16 unk06;
    s16 unk08;
    s16 unk0A;
} Func80060238Inner;

typedef struct Func80060238Obj18 {
    u8 pad00[8];
    s32 unk08;
} Func80060238Obj18;

typedef struct Func80060238Sub9C {
    u8 unk00;
} Func80060238Sub9C;

typedef struct Func80060238Arg2 {
    u8 pad00[0xC];
    u8 unk0C;
    u8 pad0D[0xF];
    s16 unk1C;
    u8 pad1E[6];
    u32 unk24;
    u8 pad28[0x74];
    Func80060238Sub9C *unk9C;
} Func80060238Arg2;

typedef struct Func80060238Obj {
    Func80060238Inner *unk00;
    u16 flags04;
    u8 unk06;
    u8 pad07[3];
    s16 unk0A;
    u8 pad0C[4];
    s32 unk10;
    u8 pad14[2];
    u16 unk16;
    Func80060238Obj18 *unk18;
    s32 unk1C;
    s16 unk20;
    s8 unk22;
    u8 unk23;
    u16 unk24;
    s16 unk26;
} Func80060238Obj;

extern s32 gCommandListHead[] __asm__("D_800A8F90");

extern void gApplyAttackCommands(Func80060238Obj *arg0, Func80060238Arg2 *arg1) __asm__("func_8005FFB8");
extern s32 func_80019668(void);
extern s32 gCheckTargetRangeAndCone(Func80060238Arg2 *arg0, Func80060238Obj18 *arg1, s32 arg2, s32 arg3) __asm__("func_8004E5F0");
extern void gInterpretEntityScript(Func80060238Sub9C *arg0, s32 arg1, s32 arg2, Func80060238Arg2 *arg3) __asm__("func_80069D68");
extern s32 gAcquireAttackTarget(Func80060238Obj *arg0, s32 arg1, Func80060238Arg2 *arg2) __asm__("func_800600E8");
extern s32 gFireEntityWeapon(Func80060238Obj *arg0, Func80060238Arg2 *arg1, Func80060238Arg2 *arg2, s32 arg3) __asm__("func_8005F6E8");
s32 gStepAttackScheduler(Func80060238Obj *arg0, s32 arg1, Func80060238Arg2 *arg2, Func80060238Arg2 *arg3, u8 arg4) __asm__("func_80060238");


/**
 * @brief Frame tick of an attack descriptor: advances its frame
 *        timeline, merges scripted commands, keeps or reacquires a
 *        target and fires through func_8005F6E8.
 *
 * @param arg0 Descriptor, normally the block at object +0xA0. Definition
 *        pointer +0x00 (its +0x01 enable byte, +0x06 repeat count, +0x08
 *        and +0x0A the two durations), flag word +0x04, target class
 *        +0x06, target range +0x0A, search mask +0x10, retry timer
 *        +0x16, target pointer +0x18, target id +0x1C, gate +0x20,
 *        playing byte +0x22, toggle +0x23, frame accumulator +0x24,
 *        repeat counter +0x26.
 * @param arg1 Frame delta: added to +0x24, subtracted from +0x16 and
 *        forwarded to the sub-tests.
 * @param arg2 Owner object: command byte +0x0C, +0x1C, flag word +0x24
 *        (bit 0x100000 suppresses a spawn) and the block pointer +0x9C.
 * @param arg3 Optional second object of the same shape; its command byte
 *        +0x0C is merged as well.
 * @param arg4 Byte forwarded to func_8005F6E8 as its fire flag, where 0
 *        means "do not fire".
 * @return 0 when a gate failed or nothing fired, otherwise the result of
 *         func_8005F6E8 (1 when the attack sequence ran).
 *
 * @par Confirmed
 * Budget gate: the function returns 0 while D_800A8F90[1] -
 * D_800A8F90[0] < 10, i.e. while the entity table (count at +0x00, limit
 * at +0x04) has fewer than ten free slots. A missing definition (+0x00)
 * also returns 0.
 * Command merge: when arg3 is non-zero with a non-zero +0x0C, and when
 * arg2->unk0C is non-zero, func_8005FFB8(arg0, that object) runs; it
 * walks up to four (opcode, argument) pairs at object +0x0C and applies
 * them to the descriptor.
 * Gates: definition +0x01 == 0, flag bit 0x100 or +0x20 == 0 return 0. A
 * set +0x22 skips the timeline entirely.
 * Timeline with repeats (+0x26 > 0): +0x24 += arg1 and the signed low
 * half is compared with the duration +0x0A. On expiry flag 0x20 toggles
 * +0x23, +0x26 is decremented, the step becomes +0x0A (or
 * (|v| >> 1) + random % |v| when it is negative), +0x22 = 1 and
 * +0x24 -= step.
 * Timeline without repeats: +0x24 += arg1 and the step +0x08 is
 * subtracted after the same randomised expansion of a negative
 * duration; +0x24 is then clamped to +0x0A and +0x26 is reloaded from
 * definition +0x06.
 * While +0x22 is clear only the retry timer runs: with flag 0x4000
 * +0x16 -= arg1 (clamped at 0) and the function returns 0.
 * Target class +0x06 == 0 jumps straight to the fire tail. Otherwise a
 * cached target is validated (target +0x08 against +0x1C) and dropped,
 * and a surviving target is kept only while
 * func_8004E5F0(arg2, target, +0x0A << 4, +0x10) >= 0, a range and
 * height window test.
 * Spawn gate: with no target, flag 0x8000 set, a live block at
 * arg2->unk9C (+0x00 non-zero) and arg2->unk24 bit 0x100000 clear,
 * func_80069D68(arg2->unk9C, arg2->unk1C < 0, 0, arg2) runs.
 * Fire gate: a target with flag 0x4000 clear fires at once; with flag
 * 0x4000 set the result of func_800600E8(arg0, arg1, arg2), which
 * searches again through func_8004E80C and reloads +0x16, decides; with
 * no target only a non-zero func_800600E8 result (or the target it
 * installs) fires. Everything else returns 0. The fire itself is
 * func_8005F6E8(arg0, arg2, arg3, arg4), truncated to a byte.
 *
 * @par Likely role
 * The weapon controller carried by each object: the two durations are
 * the fire interval and the hold time of the animation stage, +0x26 the
 * number of bursts, +0x1C the id of the target the descriptor was aimed
 * at, +0x20 a "armed/ready" gate, flag 0x4000 "attack enabled" and 0x8000
 * "may spawn a projectile through the context block". The callers use
 * the returned flag to switch their own state or clip, so the return
 * value is the "a shot happened" report.
 *
 * @par Unknown
 * The meaning of +0x23 (only toggled, never read), of +0x20 and of flag
 * bits 0x20, 0x100, 0x8000 and 0x4000 beyond their branch position, and
 * the opcode set that func_8005FFB8 accepts.
 *
 * @par Callers
 * func_8002B784 (0x8002B8A0, 0x8002B95C), func_8002CC30 (0x8002CC90),
 * func_8002E630 (0x8002E7A4, 0x8002E968, 0x8002E9A0), func_800324E0
 * (0x80032EA4), func_8003532C (0x80035644, 0x80035660), func_800382B4
 * (0x800385DC) and func_80038A00 (0x800391CC, 0x80039210); all
 * analysis/edges.json kind "direct". Three of them pass the block at
 * object +0xA0, func_8003532C passes the two blocks at +0x50 and +0x78
 * of its entity.
 */
s32 gStepAttackScheduler(Func80060238Obj *arg0, s32 arg1, Func80060238Arg2 *arg2, Func80060238Arg2 *arg3, u8 arg4)
{
    Func80060238Inner *inner;
    Func80060238Obj18 *obj18;
    Func80060238Arg2 *call_arg2;
    register u32 bytearg __asm__("$22");
    s32 s0;

    bytearg = arg4;

    if (gCommandListHead[1] - gCommandListHead[0] < 10) {
        return 0;
    }
    if (arg0->unk00 == 0) {
        goto L60600;
    }
    if (arg3 != 0 && arg3->unk0C != 0) {
        gApplyAttackCommands(arg0, arg3);
    }
    if (arg2->unk0C != 0) {
        gApplyAttackCommands(arg0, arg2);
    }
    inner = arg0->unk00;
    if (inner->unk01 == 0) {
        goto L60600;
    }
    if (arg0->flags04 & 0x100) {
        goto L60600;
    }
    if (arg0->unk20 == 0) {
        goto L60600;
    }
    if (arg0->unk22 != 0) {
        goto L604B0;
    }

    if (arg0->unk26 > 0) {
        arg0->unk24 += arg1;
        if ((s16)arg0->unk24 < inner->unk0A) {
            goto L60468;
        }
        if (arg0->flags04 & 0x20) {
            arg0->unk23 = 1 - arg0->unk23;
        }
        arg0->unk26--;
        s0 = inner->unk0A;
        if (s0 < 0) {
            s0 = -s0;
            s0 = (s0 >> 1) + (func_80019668() % (u32)s0);
        }
        arg0->unk22 = 1;
        arg0->unk24 -= s0;
        goto L60468;
    }

    arg0->unk24 += arg1;
    if ((s16)arg0->unk24 < inner->unk08) {
        goto L60468;
    }
    s0 = inner->unk08;
    if (s0 < 0) {
        s0 = -s0;
        s0 = (s0 >> 1) + (func_80019668() % (u32)s0);
    }
    arg0->unk24 -= s0;
    if (inner->unk0A < (s16)arg0->unk24) {
        arg0->unk24 = inner->unk0A;
    }
    arg0->unk26 = inner->unk06;

L60468:
    if (arg0->unk22 == 0) {
        if (!(arg0->flags04 & 0x4000)) {
            goto L60600;
        }
        arg0->unk16 -= arg1;
        if ((s16)arg0->unk16 < 0) {
            arg0->unk16 = 0;
        }
        goto L60600;
    }

L604B0:
    if (arg0->unk06 == 0) {
        goto L605EC;
    }
    obj18 = arg0->unk18;
    if (obj18 == 0) {
        goto L60584;
    }
    if (obj18->unk08 != arg0->unk1C) {
        arg0->unk18 = 0;
    }
    if (arg0->unk18 != 0) {
        if (gCheckTargetRangeAndCone(arg2, arg0->unk18, arg0->unk0A << 4, arg0->unk10) < 0) {
            arg0->unk18 = 0;
        }
    }
    if (obj18 == 0) {
        goto L60584;
    }
    if (arg0->unk18 != 0) {
        goto L60594;
    }
    if (!(arg0->flags04 & 0x8000)) {
        goto L60584;
    }
    if (arg2->unk9C == 0) {
        goto L60584;
    }
    if (arg2->unk9C->unk00 == 0) {
        goto L60584;
    }
    if (arg2->unk24 & 0x100000) {
        goto L60584;
    }
    gInterpretEntityScript(arg2->unk9C, arg2->unk1C < 0, 0, arg2);

L60584:
    if (arg0->unk18 == 0) {
        goto L605A8;
    }
L60594:
    if (!(arg0->flags04 & 0x4000)) {
        goto L605EC;
    }
L605A8:
    if ((u8)gAcquireAttackTarget(arg0, arg1, arg2) != 0) {
        goto L605EC;
    }
    if (!(arg0->flags04 & 0x4000)) {
        goto L60600;
    }
    if (arg0->unk18 != 0) {
        goto L605EC;
    }
L60600:
    return 0;

L605EC:
    return (u8)gFireEntityWeapon(arg0, arg2, arg3, bytearg);
}
