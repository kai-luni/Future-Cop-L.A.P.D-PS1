// Categories: Game Engine > Object Update > Tick Handler | Gameplay > Path Movement | Graphics > 3D > Actor Transform | Audio > Trigger
#include "game/types.h"

#define B(o,n) (*(u8 *)((u8 *)(o) + (n)))
#define H(o,n) (*(s16 *)((u8 *)(o) + (n)))
#define U(o,n) (*(u16 *)((u8 *)(o) + (n)))
#define W(o,n) (*(s32 *)((u8 *)(o) + (n)))
#define F(o,n) (*(u32 *)((u8 *)(o) + (n)))
#define P(o,n) (*(u8 **)((u8 *)(o) + (n)))
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

typedef struct { s32 x, y, z; } Vec;

extern s32 func_80044B34(u8 *, void *, s32, s32);
extern void gSetModelFrame(void *, s32) __asm__("func_800175F8");
extern void func_8002E528(u8 *);
extern void func_8002E2BC(u8 *, s32);
extern u8 gStepAttackScheduler(void *, s32, u8 *, s32, s32) __asm__("func_80060238");
extern void func_800448D8(void *, s32, s32, u8);
extern void gFireEntityWeapon(void *, u8 *, u8 *, s32) __asm__("func_8005F6E8");
extern s32 gAtan2(s32, s32) __asm__("func_80095C30");
extern void gBuildRotationMatrix(s32, s32, s32, void *) __asm__("func_800188D0");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
extern void gVectorMulAdd(void *, void *, s32, void *) __asm__("func_8001AC28");
extern void func_800457B8(s32, s32);

#define FMULU(a,b) ({ s32 r_; __asm__("nop\n\tnop\n\tmultu %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
s32 gTickActor(u8 *obj, s32 dt) __asm__("func_8002E630");


/**
 * @brief Per-frame tick of an animated actor: state machine, travel along a
 *        start-to-end segment, heading steering, animation clip selection,
 *        model part transform and a one-shot audio trigger.
 *
 * Confirmed role
 * --------------
 * func_8002ED7C is the message table of this object class and forwards message
 * 6 - the tick message - unchanged to this function, returning its result. All
 * of the actor's motion and animation therefore advance here and nowhere else.
 *
 * @param obj Actor block. The file does not declare a struct; every field is
 *        addressed through the macros B/H/U/W/F/P. The state byte at +0xDC
 *        selects the branch, the flag byte at +0xDD enables the optional
 *        parts.
 * @param dt  Frame delta. It is subtracted from the timer at +0xE2, multiplied
 *        by the speed at +0xE8 and by the animation rate at +0xE6, and scaled
 *        by 8 for the heading step.
 *
 * @return Always 0. Confirmed: the only return statement is "return 0" in the
 *         done label and every path jumps there.
 *
 * @par Confirmed state machine
 * State 4 is a special case and is handled before everything else: the
 * animation track at +0xC8 is stepped, the model frame at +0xD4 is applied,
 * and when the event code (result shifted right by 1) equals 3 the counter at
 * +0x1C is set to -1 and the state becomes 5 - the clip ends the sequence.
 *
 * The command queue at +0x0C is then served by func_8002E528 (opcodes 0x55 and
 * 0x56, see that function). The timer at +0xE2 is decremented by dt unless
 * flag bit 0 of +0xDD is set and the timer is still positive; if it stays
 * positive the transitions below are skipped and only the movement part runs.
 *
 * Transitions (switch on +0xDC, only while the timer has run out):
 * 0 -> advance the position by the velocity at +0x100/+0x104/+0x108, run the
 * script at +0xA0 with the second argument 0; on a non-zero result switch to 1
 * (unless flag 0x80 is set and the frame at +0xD4 differs from the clip index
 * stored at model_clip+6, which aborts), on zero clear the timer. The position
 * is restored afterwards, so state 0 does not drift.
 * 1 -> state 2, 2 -> state 3 when +0xD9 differs from +0x110, 3 -> state 0.
 *
 * @par Confirmed movement (states 1 and 3, shared case)
 * distance = speed(+0xE8) * dt, negated for state 3 and clamped to the travel
 * limits: below zero for state 3 and above +0xF0 for state 1, both of which
 * also zero the timer. The travelled distance at +0xEC is then converted into
 * a Q16 ratio with the constant pair 0x80000000 and (limit shifted right by 1):
 * at travel == limit the ratio is exactly 0x10000, so func_8001AC28 computes
 * start(+0xF4) + delta(+0x100) * ratio, i.e. the current point on the segment.
 * The animation is stepped by the absolute distance.
 *
 * @par Confirmed steering (state 2)
 * Only when flag 0x10 is set, or flag 4 is set and +0xB8 is non-null: the goal
 * angle is the current yaw at +0xE4 plus 0x800 (the object faces backwards) or
 * atan2(dx, dz) of the target at +0xB8 from func_80095C30. The yaw is advanced
 * by at most dt * 8 per frame along the short arc (the difference is wrapped
 * into -0x800..0x800 and clamped with the ABS macro), stored back at +0xE4,
 * turned into a matrix with func_800188D0, loaded with func_80010750 and used
 * to transform the model part selected by +0x70. The three words at
 * +0x44/+0x48/+0x4C (matrix row 2, which for the pure yaw rotation used here is
 * (sin, 0, cos)) are copied to +0x78 - Likely a cached forward vector.
 *
 * @par Confirmed animation events (state 2)
 * When the model instance at +0x28 reports a non-zero value at its +0x10 (the
 * same test func_8002E2BC makes before touching a model, Likely "model data
 * attached"), the animation rate is scaled by (rate(+0xE6) * dt) / 16 and the
 * event code (result shifted right by 1, masked with 0x3F) is dispatched: 2
 * calls func_8005F6E8 on the script block at +0xA0, 1 calls func_80060238 and
 * then switches the clip with func_800448D8(track +0xC8, +0x110, 1, 1) when the
 * timer is still positive and the clip at +0xD9 differs.
 *
 * @par Confirmed audio trigger
 * At the tail, when flag 0x40 is set: states 1 and 3 copy +0x113 into +0x21,
 * every other state that has +0x113 non-zero calls func_800457B8 with the id at
 * +0x8 and the index at +0x22, clears +0x21 and clears bit 0x400000 of the
 * flags at +0x24. Likely a sound start that is acknowledged through that bit.
 *
 * @par Globals
 * None. Confirmed: the file declares no D_ symbol and reads no memory besides
 * the object block and the pointers it contains.
 *
 * @par Object offsets used (Confirmed, from the code itself)
 * +0x0C command opcode, +0x1C s16, +0x21/+0x22 bytes, +0x24 u32 flags,
 * +0x28 model instance (+0x10 tested, +0x34 part table), +0x2C matrix
 * (9 words, translation at +0x50), +0x50 position, +0x5C copy destination,
 * +0x70 s16 part index, +0x78 cached vector, +0xA0 script, +0xA4 animation
 * flags, +0xB8 target, +0xC8 animation track, +0xCC clip record (+6 compared),
 * +0xD4 frame, +0xD9 current clip, +0xDC state, +0xDD flags, +0xDE/+0xE0
 * unused here, +0xE2 timer, +0xE4 yaw, +0xE6 rate, +0xE8 speed, +0xEC travel,
 * +0xF0 length, +0xF4 segment start, +0x100 segment delta, +0x10C..+0x10F
 * clip indices (used by func_8002E2BC), +0x110 target clip, +0x113 flag byte.
 *
 * @par Control flow
 * 1. State 4 short path: animation step, model frame, possible end of the
 *    sequence (state 5) and return.
 * 2. Command queue at +0x0C, then the timer gate.
 * 3. Transitions on the state byte while the timer has run out.
 * 4. Second dispatch: movement for states 1 and 3, script and steering for
 *    state 2, animation only for state 0, nothing for states 4 and 5.
 * 5. Tail: model frame, matrix and part transform, audio latch.
 *
 * @par Likely
 * The byte +0x21, latched from +0x113 in the two moving states and cleared
 * after the audio call, is a "sound already triggered" marker for the id at
 * +0x8 and the index at +0x22. States 4 and 5 form an exit sequence: state 4
 * advances the animation and waits for event 3, state 5 is not handled by
 * either switch and only reaches the tail. The pair +0xF4/+0x100 is a segment
 * (start plus delta), because the Q16 ratio is 0 at travel 0 and exactly
 * 0x10000 at the travel limit.
 *
 * @par Unknown
 * The meanings of flag bits 0x10, 0x40 and 0x80 of +0xDD, of the byte +0x113
 * and of the counter +0x1C (set to -1 on the state 4 path) are not provable
 * from this file; they are only seen as branch conditions or plain stores.
 *
 * @par Calls
 * func_8002E528 (command queue), func_8002E2BC (state change, four sites),
 * func_80044B34 (animation step, three sites), func_800448D8 (clip switch),
 * func_80060238 (script step, three sites), func_8005F6E8 (script event),
 * func_80095C30 (atan2), func_800188D0 (matrix from angle), func_80010750
 * (matrix to GTE), func_80010234 (part transform), func_8001AC28 (segment
 * point), func_800457B8 (audio trigger).
 *
 * @par Side effects
 * Writes the whole object block: state, timer, position, yaw, matrix, cached
 * vector, clip latch and the audio latch. Advances the animation track at
 * +0xC8 and the model frame; loads the GTE matrix. No global is written.
 *
 * @par Callers
 * func_8002ED7C, message 6 only (analysis/edges.json, kind "direct"). The same
 * address appears once as an "address_taken" edge, i.e. it is installed in a
 * message table as well.
 *
 * @par Environment path
 * Message-6 (per-frame) body of the type-27 handler func_8002ED7C. It
 * services the command queue with func_8002E528 and runs a state machine on
 * +0xDC: 0 waits while the attack descriptor (func_80060238) looks for a
 * target, 1 and 3 move the object out and back along the segment at
 * +0xF4/+0x100 with func_8001AC28, 2 animates, fires through func_8005F6E8
 * and turns the yaw towards its target (func_80095C30, func_800188D0), and 4
 * plays a final animation before switching to 5. Every path refreshes the
 * frame vertex with func_80010750 and func_80010234, and with flag 0x40 at
 * +0xDD it raises a sound through func_800457B8. The file uses no D_ symbol.
 */
s32 gTickActor(u8 *obj, s32 dt)
{
    s32 s0;
    s32 a2;
    s32 ang;
    u8 *s2;
    u8 *t;
    u16 timer;
    u8 odd;
    register u32 sw __asm__("$3");

    if (B(obj,0xDC) == 4) {
        s0 = func_80044B34(obj, obj + 0xC8, dt, 0);
        gSetModelFrame(obj + 0x28, H(obj,0xD4));
        s0 = (u32)s0 >> 1;
        if (s0 != 3) {
            goto done;
        }
        H(obj,0x1C) = -1;
        B(obj,0xDC) = 5;
        goto done;
    }
    if (B(obj,0xC)) {
        func_8002E528(obj);
    }
    if ((B(obj,0xDD) & 1) && H(obj,0xE2) > 0) {
        goto after_timer;
    }
    timer = U(obj,0xE2) - dt;
    U(obj,0xE2) = timer;
    if ((s16)timer > 0) {
        goto after_timer;
    }
    switch (B(obj,0xDC)) {
    case 0: {
        Vec save;
        u16 t0;
        save = *(Vec *)(obj + 0x50);
        W(obj,0x50) += W(obj,0x100);
        t0 = U(obj,0xA4);
        s0 = t0 & 7;
        W(obj,0x54) += W(obj,0x104);
        W(obj,0x58) += W(obj,0x108);
        U(obj,0xA4) = t0 & 0xFFF8;
        if (gStepAttackScheduler(obj + 0xA0, dt, obj, 0, 0)) {
            if ((B(obj,0xDD) & 0x80) && H(obj,0xD4) != H(P(obj,0xCC),6)) {
                goto after_timer;
            }
            func_8002E2BC(obj, 1);
        } else {
            H(obj,0xE2) = 0;
        }
        U(obj,0xA4) |= s0;
        *(Vec *)(obj + 0x50) = save;
        break;
    }
    case 1:
        func_8002E2BC(obj, 2);
        break;
    case 2:
        if (B(obj,0xD9) != B(obj,0x110)) {
            func_8002E2BC(obj, 3);
        }
        break;
    case 3:
        func_8002E2BC(obj, 0);
        break;
    }
after_timer:
    switch (B(obj,0xDC)) {
    case 1:
    case 3:
        goto case_13;
    case 0:
        goto case_0;
    case 2:
        goto case_2;
    default:
        goto tail;
    }
case_2:
    if (W(P(obj,0x28),0x10)) {
        a2 = dt;
        if (B(obj,0xE6)) {
            a2 = (B(obj,0xE6) * dt) / 16;
        }
        s2 = obj + 0xC8;
        s0 = func_80044B34(obj, s2, a2, 0);
        if (W(obj,0xA0)) {
            s0 = (u32)s0 >> 1;
            if (s0) {
                sw = s0 & 0x3F;
                switch (sw) {
                case 2:
                    gFireEntityWeapon(obj + 0xA0, obj, obj, 1);
                    break;
                case 1:
                    if (H(obj,0xE2) > 0 && B(obj,0xD9) != B(obj,0x110)) {
                        if (gStepAttackScheduler(obj + 0xA0, dt, obj, 0, 0)) {
                            func_800448D8(s2, B(obj,0x110), 1, 1);
                        }
                    }
                    break;
                }
            }
        }
    } else {
        gStepAttackScheduler(obj + 0xA0, dt, obj, 0, 1);
    }
    if ((B(obj,0xDD) & 0x10) || ((B(obj,0xDD) & 4) && P(obj,0xB8))) {
        if (B(obj,0xDD) & 0x10) {
            ang = H(obj,0xE4) + 0x800;
        } else {
            s32 vec[3];
            vec[0] = W(P(obj,0xB8),0x5C) - W(obj,0x5C);
            vec[1] = W(P(obj,0xB8),0x60) - W(obj,0x60);
            vec[2] = W(P(obj,0xB8),0x64) - W(obj,0x64);
            ang = gAtan2(vec[0], vec[2]);
        }
    {
        s32 cur;
        s32 base;
        s32 d;
        s32 step;
        s32 ad;

        ang &= 0xFFF;
        cur = H(obj,0xE4);
        base = U(obj,0xE4);
        if (cur == ang) {
            goto tail;
        }
        d = (ang - cur) & 0xFFF;
        if (d > 0x800) {
            d -= 0x1000;
        }
        step = dt << 3;
        ad = ABS(d);
        if (ad < step) {
            step = ad;
        }
        if (d < 0) {
            step = -step;
        }
        base += step;
        base &= 0xFFF;
        U(obj,0xE4) = base;
        gBuildRotationMatrix(base, 0, 0, obj + 0x2C);
        gGteLoadMatrix(obj + 0x2C);
        gGteTransformVerticesWide(P(P(obj,0x28),0x34) + ((H(obj,0x70) << 4) + 8), obj + 0x5C, 1);
        *(Vec *)(obj + 0x78) = *(Vec *)(obj + 0x44);
    }
    }
    goto tail;
case_0:
    if (W(P(obj,0x28),0x10)) {
        a2 = dt;
        if (B(obj,0xE6)) {
            a2 = (B(obj,0xE6) * dt) / 16;
        }
        func_80044B34(obj, obj + 0xC8, a2, 0);
        goto tail;
    }
    goto tail2;
case_13:
    if (W(obj,0xE8) > 0) {
        s0 = W(obj,0xE8) * dt;
        if (B(obj,0xDC) == 3) {
            s0 = -s0;
            if (W(obj,0xEC) + s0 < 0) {
                s0 = -W(obj,0xEC);
                H(obj,0xE2) = 0;
            }
        } else {
            if (W(obj,0xF0) < W(obj,0xEC) + s0) {
                s0 = W(obj,0xF0) - W(obj,0xEC);
                H(obj,0xE2) = 0;
            }
        }
        a2 = W(obj,0xEC) + s0;
        W(obj,0xEC) = a2;
        if (W(obj,0xF0) > 0) {
            a2 = FMULU(a2, (u32)0x80000000 / (u32)(W(obj,0xF0) >> 1));
        } else {
            a2 = 0x10000;
        }
        gVectorMulAdd(obj + 0xF4, obj + 0x100, a2, obj + 0x50);
        if (W(P(obj,0x28),0x10)) {
            func_80044B34(obj, obj + 0xC8, ABS(s0), 0);
        }
    } else {
        odd = func_80044B34(obj, obj + 0xC8, dt, 0) & 1;
        if (B(obj,0xDC) == 1 || B(obj,0xDC) == 3) {
            if (odd) {
                H(obj,0xE2) = 0;
            }
        }
    }
tail:
    if (W(P(obj,0x28),0x10)) {
        gSetModelFrame(obj + 0x28, H(obj,0xD4));
    }
tail2:
    gGteLoadMatrix(obj + 0x2C);
    gGteTransformVerticesWide(P(P(obj,0x28),0x34) + ((H(obj,0x70) << 4) + 8), obj + 0x5C, 1);
    if (B(obj,0xDD) & 0x40) {
        if (B(obj,0xDC) == 1 || B(obj,0xDC) == 3) {
            B(obj,0x21) = B(obj,0x113);
        } else if (B(obj,0x113)) {
            if (!(F(obj,0x24) & 0x400000)) {
                goto done;
            }
            func_800457B8(W(obj,8), (s8)B(obj,0x22));
            B(obj,0x21) = 0;
            F(obj,0x24) &= ~0x400000;
        }
    }
done:
    return 0;
}
