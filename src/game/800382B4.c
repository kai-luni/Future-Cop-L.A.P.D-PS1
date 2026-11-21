// Categories: Gameplay > Entity Script | Game Engine > Entity Lifecycle | Graphics > Model Renderer
#include "game/types.h"

typedef struct Unk382B4Sub {
    u8 pad00[0x10];
    u32 unk10;
} Unk382B4Sub;

typedef struct Unk382B4CC {
    u8 pad00;
    u8 unk01;
} Unk382B4CC;

typedef struct Unk382B4 {
    u8 pad00[0xC];
    u8 unk0C[8];
    u8 pad14[2];
    u8 unk16;
    u8 pad17[2];
    u8 unk19;
    u8 pad1A[2];
    s16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 pad21[3];
    u32 unk24;
    Unk382B4Sub *unk28;
    u8 pad2C[0x58];
    s32 unk84;
    u8 pad88[0x18];
    u32 unkA0;
    u8 padA4[0x24];
    u8 unkC8[4];
    Unk382B4CC *unkCC;
    u8 padD0[4];
    s16 unkD4;
    u8 padD6[6];
    u8 *unkDC;
    u8 padE0[0x22];
    s16 unk102;
    u8 pad104[0x14];
    u32 unk118;
    u8 pad11C[2];
    s16 unk11E;
    s16 unk120;
    u8 pad122[8];
    s16 unk12A;
    u8 pad12C[0x2C];
    u8 unk158;
    u8 unk159;
    u8 unk15A;
} Unk382B4;

extern void *jtbl_800113DC[16];

extern s32 gEntityBaseHandler(s32 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3) __asm__("func_8002AD58");
extern void gInitRailMoverEntity(s32 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3) __asm__("func_800378BC");
extern void gResolveRailMoverContact(s32 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3) __asm__("func_800379D4");
extern void gStepIdleActorTurn(Unk382B4 *arg1) __asm__("func_800381D4");
extern void func_800448D8(void *arg0, u8 arg1, s32 arg2, s32 arg3);
extern void gSpawnEntitySlotTypes(u8 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3, s32 arg4) __asm__("func_8003B898");
extern void gUpdateChaseApproachSpeed(s32 arg0, Unk382B4 *arg1, s32 arg2) __asm__("func_80037B28");
extern void gRemoveEntityCommand(Unk382B4 *arg0, s32 arg1) __asm__("func_8002A5B4");
extern s32 gAdvanceAnimTrack(Unk382B4 *arg0, void *arg1, s32 arg2, s32 arg3) __asm__("func_80044B34");
extern void gSetModelFrame(void *arg0, s32 arg1) __asm__("func_800175F8");
extern void gStepAttackScheduler(void *arg0, s32 arg1, Unk382B4 *arg2, s32 arg3, s32 arg4) __asm__("func_80060238");
extern void gUpdateFallMotion(s32 arg0, Unk382B4 *arg1, s32 arg2) __asm__("func_8003802C");
extern s32 func_8002ACB4(Unk382B4 *arg0, u8 arg1);
extern void func_80063194(s32 key);

register s32 zero __asm__("$0");
s32 gHandleType5Entity(s32 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3) __asm__("func_800382B4");


/**
 * @brief Message handler of entity class 5: dispatches messages 1..16 through
 *        jtbl_800113DC onto the class's part activation, animation and motion steps.
 *
 * @param arg0 Message id, 1..16; 0 and values above 16 go to the base handler.
 * @param arg1 Entity object; +0x0C four 2-byte part commands, +0x16 kind (5 for the
 *        animated variant), +0x19 part count byte, +0x1C state word, +0x20 cell
 *        index, +0x24 flag word, +0x28 instance block (+0x10 its model/data word),
 *        +0x84 scale, +0x102/+0x118/+0x11E/+0x120/+0x12A words, +0x158..+0x15A
 *        activation sub-state, +0xC8 animation track, +0xA0/+0xCC/+0xDC sub-records.
 * @param arg2 Forwarded value: the time step for messages 9/10, and the selector
 *        that message 11 compares against 6.
 * @param arg3 Forwarded to the base handler func_8002AD58.
 * @return Message 1 returns 0x15C (this class's size, against 0xA0 for the base
 *         class); the query messages return their field; everything else returns 0
 *         or the base handler's answer.
 *
 * @par Confirmed
 * Dispatch:
 * msgidx = arg0 - 1; only msgidx below 16 is indirect, and msgidx 15 plus every
 * out-of-range value lands on the shared tail, return func_8002AD58(message,
 * object, arg2, arg3).
 * 1: return 0x15C.
 * 2: func_800378BC(object, arg1, arg2, arg3) - the full part/animation/frame
 *    routine - then 0.
 * 3: 0.
 * 4: return 0 when +0x16 == 5 and +0x15A == 1, else func_8002AD58 then the shared
 *    +0x118 tail.
 * 5: the same guard, func_800379D4, then the shared tail.
 * 6: only the shared tail: when +0x118 has bit 0x2000 and +0x1C is negative, a
 *    positive +0x102 makes func_800381D4 re-randomise the motion/lifetime.
 * 7: needs +0x16 == 5, (+0x28)->+0x10 non-zero and +0x158 != 0xFF. With +0x15A == 0
 *    it activates: +0x1C = 1, +0x15A = 1, +0x118 |= 0x40, +0x24 |= 1,
 *    func_800448D8(+0xC8, +0x158, 1, 1) starts the track at clip +0x158 and
 *    func_8003B898(+0x20, object, 0, (+0x19 >> 3) & 0x1E, 0) spawns the cell parts.
 *    With +0x15A == 1 it only sets +0x1C = 1; any other sub-state falls back.
 * 8: func_80037B28(arg0, object, arg2), then 0.
 * 9: the physics step. The scale is arg2 unless (+0xCC)->+0x01 is not 1, in which
 *    case it is +0x84 * arg2. For the class-5 activated variant the four 2-byte
 *    commands at +0x0C are shifted down by func_8002A5B4(arg1, i) while their byte
 *    is 0x32..0x34. func_80044B34(object, +0xC8, scale, 0) advances the script and
 *    func_800175F8(+0x28, +0xD4) the model instance; a halved result of 3 with
 *    +0x15A == 1 ends the piece (+0x1C = -1, +0x15A = 2), otherwise the tail runs.
 * 10: func_80060238(+0xA0, arg2, object, 0, 1) advances the sub-simulation and
 *    func_8003802C(arg0, object, arg2) steps the falling/flying motion.
 * 11: returns +0x11E, but only when arg2 == 6.
 * 12: needs +0x16 == 5 and (+0x28)->+0x10 non-zero; returns
 *    func_8002ACB4(object, +0x159) (the animation advance result).
 * 13: reads +0x120 against the record count at (+0xDC)+2, indexes +0xDC by
 *    index * 12 and, when the low nibble of the byte at +0x8 is non-zero, rewrites
 *    that nibble of the word at +4 as c - 1; a set +0x118 bit 0x4000 then removes
 *    the object from the D_800A31A0 key table through func_80063194. Falls back.
 * 14: 1 while +0x16 is not 5 or (+0x28)->+0x10 is zero, else +0x159.
 * 15: +0x12A.
 *
 * @par Likely role
 * The scripted animated prop/piece of the map: activation picks a clip and spawns
 * its cell parts, messages 9/10 are the frame steps, 13 consumes one hit point of
 * the current piece, and 11/14/15 are the queries the scripting layer reads while
 * the piece is alive.
 *
 * @par Unknown
 * Field meanings beyond the offsets - the +0x84 scale, the (+0xCC)->+0x01 flag, the
 * +0x120 record index, the +0x158..+0x15A sub-state ladder - and what the 0x15C
 * size is used for by the allocator.
 *
 * @par Callers
 * func_8002AF94, func_80039504, func_800395A8, func_800397AC and func_800398EC (all
 * direct calls verified in their sources). The entity-type factory func_8002AE54
 * installs it as the class-5 handler.
 */
s32 gHandleType5Entity(s32 arg0, Unk382B4 *arg1, s32 arg2, s32 arg3)
{
    register s32 msgidx __asm__("$3");
    s32 scale;
    s32 i;
    s32 temp;
    s16 idx;
    u8 v;
    register u32 flag __asm__("$3");
    register s32 one __asm__("$2");
    register s32 multiplicand __asm__("$2");
    u8 *base;
    u8 *entry;
    u8 count;
    static void *const possible_targets[] = {
        &&L80038310, &&L80038318, &&L8003832C, &&L80038334,
        &&L8003836C, &&L8003839C, &&L800383E0, &&L80038498,
        &&L800384B0, &&L800385BC, &&L800385FC, &&L80038614,
        &&L80038650, &&L800386D8, &&L8003870C, &&L80038718
    };

    msgidx = arg0 - 1;
    if ((u32)msgidx < 16) {
        goto *jtbl_800113DC[msgidx];
    }
    goto L80038718;

L80038310:
    return 0x15C;

L80038318:
    gInitRailMoverEntity(arg0, arg1, arg2, arg3);

L8003832C:
    return zero;

L80038334:
    if (arg1->unk16 == 5 && arg1->unk15A == 1) {
        return 0;
    }
    gEntityBaseHandler(arg0, arg1, arg2, arg3);
    goto L8003839C;

L8003836C:
    if (arg1->unk16 == 5 && arg1->unk15A == 1) {
        return 0;
    }
    gResolveRailMoverContact(arg0, arg1, arg2, arg3);

L8003839C:
    if ((arg1->unk118 & 0x2000) != 0) {
        if (arg1->unk1C < 0) {
            if (arg1->unk102 > 0) {
                gStepIdleActorTurn(arg1);
            }
        }
    }
    return 0;

L800383E0: {
    register s32 state __asm__("$3");
    register s32 one __asm__("$2");
    if (arg1->unk16 != 5) {
        goto L8003871C;
    }
    if (arg1->unk28->unk10 == 0) {
        goto L8003871C;
    }
    if (arg1->unk158 == 0xFF) {
        goto L80038720;
    }
    state = arg1->unk15A;
    one = 1;
    if (state == 0) {
        register s32 six __asm__("$6");
        register s32 two __asm__("$2");
        register s32 seven __asm__("$7");
        register u8 *a0 __asm__("$4");
        register s32 five __asm__("$5");
        a0 = arg1->unkC8;
        __asm__ volatile ("" : "+r" (a0), "+r" (one));
        six = 1;
        __asm__ volatile ("" : "+r" (six));
        seven = six;
        __asm__ volatile ("" : "+r" (seven));
        five = arg1->unk158;
        __asm__ volatile ("" : : "r" (five));
        two = six;
        __asm__ volatile ("" : "+r" (two), "+r" (seven));
        arg1->unk1C = two;
        arg1->unk15A = two;
        arg1->unk118 |= 0x40;
        arg1->unk24 |= 1;
        func_800448D8((void *)a0, five, six, seven);
        gSpawnEntitySlotTypes(arg1->unk20, arg1, 0, (arg1->unk19 >> 3) & 0x1E, 0);
        return 0;
    }
    if (state != one) {
        goto L8003871C;
    }
    arg1->unk1C = state;
    goto L8003832C;
}

L80038498:
    gUpdateChaseApproachSpeed(arg0, arg1, arg2);
    return 0;

L800384B0:
    if (arg1->unk28->unk10 == 0) {
        goto L800385BC;
    }
    scale = arg2;
    flag = arg1->unkCC->unk01;
    one = 1;
    if (flag != one) {
        multiplicand = arg1->unk84;
        scale = multiplicand * arg2;
    }
    if (arg1->unk16 == 5) {
        flag = arg1->unk15A;
        one = 1;
        if (flag == one) {
            for (i = 0; i < 4; ) {
                v = arg1->unk0C[i * 2];
                if (v == 0) {
                    break;
                }
                if ((u8)(v - 0x32) < 3) {
                    gRemoveEntityCommand(arg1, i);
                } else {
                    i++;
                }
            }
        }
    }
    temp = gAdvanceAnimTrack(arg1, arg1->unkC8, scale, 0);
    gSetModelFrame((void *)&arg1->unk28, arg1->unkD4);
    temp >>= 1;
    if (arg1->unk16 == 5 && temp == 3 && arg1->unk15A == 1) {
        arg1->unk1C = -1;
        arg1->unk15A = 2;
        return 0;
    }

L800385BC:
    if (arg1->unkA0 != 0) {
        gStepAttackScheduler(&arg1->unkA0, arg2, arg1, 0, 1);
    }
    gUpdateFallMotion(arg0, arg1, arg2);
    return 0;

L800385FC:
    if (arg2 != 6) {
        goto L8003871C;
    }
    return arg1->unk11E;

L80038614:
    if (arg1->unk16 != 5) {
        goto L8003871C;
    }
    if (arg1->unk28->unk10 == 0) {
        goto L80038720;
    }
    return func_8002ACB4(arg1, arg1->unk159);

L80038650:
    {
        register u8 *b __asm__("$5");
        s32 off;
        register s32 c __asm__("$4");
        register s32 byte __asm__("$2");
        register u8 *p __asm__("$2");
        register s32 tmp __asm__("$2");
        off = arg1->unk120;
        if (off >= 0) {
            b = arg1->unkDC;
            if (off < *(s16 *)(b + 2)) {
                tmp = off * 3;
                off = tmp * 4;
                byte = b[off + 8];
                __asm__ volatile ("" : "+r" (byte));
                c = byte & 0xF;
                if (c > 0) {
                    p = b + (off + 4);
                    p[4] = (p[4] & 0xF0) | (c - 1);
                }
            }
        }
    }
    if ((arg1->unk118 & 0x4000) != 0) {
        func_80063194((s32)arg1);
    }
    goto L8003871C;

L800386D8:
    if (arg1->unk16 != 5) {
        return 1;
    }
    if (arg1->unk28->unk10 == 0) {
        return 1;
    }
    return arg1->unk159;

L8003870C:
    return arg1->unk12A;

L80038718:
L8003871C:
L80038720:
    return gEntityBaseHandler(arg0, arg1, arg2, arg3);
}
