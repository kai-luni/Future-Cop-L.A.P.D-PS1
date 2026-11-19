// Categories: Gameplay > Entity Update | Gameplay > AI > Aiming | Graphics > Animation
#include "game/types.h"

#define B(o,n) (*(u8 *)((u8 *)(o) + (n)))
#define H(o,n) (*(s16 *)((u8 *)(o) + (n)))
#define U(o,n) (*(u16 *)((u8 *)(o) + (n)))
#define W(o,n) (*(s32 *)((u8 *)(o) + (n)))
#define F(o,n) (*(u32 *)((u8 *)(o) + (n)))
#define P(o,n) (*(u8 **)((u8 *)(o) + (n)))

extern void func_8002B4B4(u8 *, s32);
extern s32 func_80044B34(u8 *, void *, s32, s32);
extern u8 func_80060238(void *, s32, u8 *, s32, s32);
extern void func_800448D8(void *, s32, s32, u8);
extern void func_8005F6E8(void *, u8 *, u8 *, s32);
extern void gRetireEntityBase(u8 *, s32, s32) __asm__("func_8002A810");
extern void gSetModelFrame(void *, s32) __asm__("func_800175F8");
extern void gUpdateFallMotion(s32, u8 *, s32) __asm__("func_8003802C");
extern u8 func_8002B1DC(u8 *, s32, u8 *);
extern s32 gVectorLength3D(s32 *) __asm__("func_8001A100");
extern void func_8002B0EC(u8 *, s32);
extern void gTurnEntityToAngle(u8 *, s32, s32) __asm__("func_8002B588");
extern void gOrientEntityBasis(u8 *) __asm__("func_80037EB4");
extern void func_8002B1D0(u8 *);
extern void func_8002B6C8(u8 *, u8 *, s32);
extern void func_8002B1AC(u8 *);

/**
 * @brief Per-frame update of the class-26 actor: runs its command script,
 *        advances the animation, services the attack scheduler and drives the
 *        state machine at +0x15F.
 *
 * @param obj Actor block: +0x1C counter, +0x28 model instance, +0x84 move
 *            speed, +0xA0 weapon block, +0xB8 target pointer, +0xBC target id,
 *            +0xC0 timer, +0xC8 animation track, +0xD4 frame,
 *            +0x10C/+0x10E reload and countdown, +0x118 flag word,
 *            +0x122 facing term, +0x124/+0x11E frame compare, +0x12A aim angle,
 *            +0x138 pointer, +0x158 speed limit, +0x15A lock range, +0x15C
 *            current angle, +0x15E animation id, +0x15F state, +0x160 status
 *            flags, +0x162 attack byte, +0x164/+0x165/+0x166/+0x167 animation
 *            variants.
 * @param step Time step of the frame.
 * @return Nothing.
 *
 * @par Confirmed
 * The body runs in this order:
 * 1. func_8002B4B4(obj, step) executes the four-slot command script at +0x0C
 *    (opcodes 0x5A/0x5B set the state and the +0x160 pause bit). The +0x162
 *    attack byte is then cleared unless +0x15E is +0x164 or +0x164 + 1.
 * 2. Speed selection: step * 21 when +0x15E == +0x167, step when +0x162 is
 *    non-zero or +0x15F is 7 or 8, otherwise (+0x84) * step.
 * 3. event = func_80044B34(obj, obj + 0xC8, speed, 0) >> 1: the animation
 *    events are packed 6 bits at a time and are consumed with event >>= 6.
 *    Event 1 (the attack frame) calls func_80060238(obj + 0xA0, step, obj, 0, 0)
 *    and on success sets +0x162 = 2 and jumps to animation +0x164 + 1 through
 *    func_800448D8. Event 2 sets +0x162 = 1 and fires
 *    func_8005F6E8(obj + 0xA0, obj, obj, 1). Event 3 calls
 *    func_8002A810(obj, 1, 1) and returns at once.
 * 4. func_800175F8(obj + 0x28, +0xD4) refreshes the model frame. When idle and
 *    outside states 7 and 8 the scheduler gets the release call
 *    func_80060238(obj + 0xA0, step, obj, 0, 1); state 7 returns here.
 * 5. func_8003802C(6, obj, step) runs the motion step and, for any state but
 *    6 with a zero +0xC0 timer, func_8002B1DC(obj, 6, 0) is called.
 * 6. Target lock: target = +0xB8 and it is valid only when target->+0x08 equals
 *    +0xBC. distance = func_8001A100(delta) - (target->+0x68 + obj->+0x68),
 *    with delta built from +0x5C/+0x60/+0x64 on both sides.
 * 7. With bit 0x40 of +0x118 set (an attack or aim is in progress) the body
 *    takes one of three paths: a turn away when +0x122 < -1
 *    (func_8002B1DC(obj, 5, target), then func_8002B0EC or func_8002B6C8), a
 *    turn towards +0x12A with func_8002B588 that snaps +0x15C when the
 *    difference is <= 64 and afterwards clears 0x40, zeroes +0x84, re-orients
 *    with func_80037EB4 and returns the animation to +0x165, or a range check
 *    against (+0xAA << 4) that aborts the attack or keeps chasing.
 * 8. Without 0x40, states 0, 1 and 4 lock a target when it is valid, inside
 *    (+0x15A << 4), and neither 0x40 nor 0x80 of +0x160 is set. The first lock
 *    sets +0x84 = 0, +0x10E = 0, sets 0x40, copies +0x12A to +0x15C and chases.
 * 9. If +0x160 bit 1 is clear, the +0x1C counter is below 5 and the state is
 *    not 7, 4 or 6, func_8002B1DC(obj, 6, 0) is tried; on failure the animation
 *    variant in +0x118 bits 24..27 is raised by 8 when it is 1..6, the state is
 *    forced to 2 and func_8002B1AC re-arms +0x10C/+0x10E.
 * 10. A final switch over +0x15F handles 0 (re-acquire and re-arm), 1 (drop an
 *    invalid target), 2 (leave when +0x138 is null, re-arm while +0x84 <
 *    +0x158), 3 (clear the variant bits), 5 (re-lock or force 0x40) and 6 (when
 *    +0x124 == +0x11E, copy the u16 at obj + 0xA0 + 0x04 into +0xC0, reload
 *    +0x1C from +0x1E and return to state 0).
 * 11. The tail keeps the animation id in sync: with 0x40 set it uses +0x164
 *    (or 0), otherwise the slow variant +0x165 or the fast one +0x166, so the
 *    walk/run animation follows +0x84 <= (+0x158 >> 2) + 2.
 * The class-26 dispatcher func_8002C3F0 passes a third argument (its data
 * pointer) that this body never reads.
 *
 * @par Likely role
 * The think tick of an armed actor: +0x15F is the behaviour state, +0x118 bit 6
 * the "attack in progress" flag, +0x10E the pause before the next
 * re-acquisition and +0x15A the lock range. With 0x40 set the actor rotates
 * onto +0x12A before firing, which is what the aim helpers it calls do, and
 * func_8002A810 reads as the kill/removal path of the entity handlers.
 *
 * @par Unknown
 * What states 3 and 4 mean (4 only appears as an early-out), the units of +0xAA
 * and +0x15A beyond the << 4 scaling, and what the first argument 6 of
 * func_8003802C selects.
 *
 * @par Callers
 * func_8002C3F0 only, the class-26 handler installed by func_8002AE54, which
 * reaches this body for message 8 - the attack case.
 */
void func_8002B784(u8 *obj, s32 step)
{
    s32 event;
    u8 *target;
    u8 valid;
    u8 active;
    s32 distance;
    s32 vector[3];
    s32 speed;
    register s32 difference __asm__("$2");
    s32 sign;
    u32 mode;

    func_8002B4B4(obj, step);
    if (B(obj,0x162)) {
        if (B(obj,0x15E) != B(obj,0x164) &&
            B(obj,0x15E) != B(obj,0x164) + 1) {
            B(obj,0x162) = 0;
        }
    }
    if (B(obj,0x15E) == B(obj,0x167)) {
        speed = step * 21;
    } else if (B(obj,0x162) || B(obj,0x15F) == 7 || B(obj,0x15F) == 8) {
        speed = step;
    } else {
        speed = W(obj,0x84) * step;
    }
    event = func_80044B34(obj, obj + 0xC8, speed, 0) >> 1;
    while (event) {
        switch (event) {
        case 1:
            if (func_80060238(obj + 0xA0, step, obj, 0, 0)) {
                B(obj,0x162) = 2;
                func_800448D8(obj + 0xC8, B(obj,0x164) + 1, 1, 1);
            }
            break;
        case 2:
            B(obj,0x162) = 1;
            func_8005F6E8(obj + 0xA0, obj, obj, 1);
            break;
        case 3:
            gRetireEntityBase(obj, 1, 1);
            return;
        }
        event >>= 6;
    }
    gSetModelFrame(obj + 0x28, H(obj,0xD4));
    if (!B(obj,0x162) && (u32)(B(obj,0x15F) - 7) >= 2) {
        func_80060238(obj + 0xA0, step, obj, 0, 1);
    }
    if ((u32)(B(obj,0x15F) - 7) < 2) {
        if (B(obj,0x15F) == 7) {
            return;
        }
    }
    gUpdateFallMotion(6, obj, step);
    if (B(obj,0x15F) != 6 && H(obj,0xC0) == 0) {
        func_8002B1DC(obj, 6, 0);
    }
    target = P(obj,0xB8);
    valid = 0;
    if (target) {
        valid = W(target,8) == W(obj,0xBC);
    }
    if (valid) {
        vector[0] = W(target,0x5C) - W(obj,0x5C);
        vector[1] = W(target,0x60) - W(obj,0x60);
        vector[2] = W(target,0x64) - W(obj,0x64);
        distance = gVectorLength3D(vector) - (W(target,0x68) + W(obj,0x68));
    }
    if (F(obj,0x118) & 0x40) {
        if (H(obj,0x122) < -1) {
            if (B(obj,0x15F) != 5) {
                func_8002B1DC(obj, 5, target);
            }
            if (!valid) {
                func_8002B0EC(obj, 0);
            } else {
                func_8002B6C8(obj, target, step);
            }
        } else if (H(obj,0x10E) > 0) {
            if (H(obj,0x15C) != H(obj,0x12A)) {
                gTurnEntityToAngle(obj, H(obj,0x12A), step);
                difference = H(obj,0x15C);
                __asm__ volatile ("" : "+r" (difference));
                difference -= H(obj,0x12A);
                sign = difference >> 31;
                difference = (sign + difference) ^ sign;
                if (difference <= 64) {
                    U(obj,0x15C) = U(obj,0x12A);
                }
            } else {
                F(obj,0x118) &= ~0x40;
                W(obj,0x84) = 0;
                gOrientEntityBasis(obj);
                func_8002B0EC(obj, B(obj,0x165));
            }
        } else if (B(obj,0x15F) != 5) {
            if (!valid || distance > (H(obj,0xAA) << 4)) {
                if (B(obj,0x15F) == 1) {
                    func_8002B1DC(obj, 0, target);
                }
                func_8002B1D0(obj);
            } else if (B(obj,0x162) != 2) {
                func_8002B6C8(obj, target, step);
            }
        }
    } else {
        active = 0;
        if ((B(obj,0x15F) == 1 || B(obj,0x15F) == 4 || B(obj,0x15F) == 0) &&
            valid && distance <= (H(obj,0x15A) << 4) &&
            !(F(obj,0x118) & 0x40) && !(B(obj,0x160) & 0x80)) {
            if (W(obj,0x84) == 0) {
                if (B(obj,0x15F) == 0) {
                    func_8002B1DC(obj, 1, target);
                }
                W(obj,0x84) = 0;
                H(obj,0x10E) = 0;
                F(obj,0x118) |= 0x40;
                U(obj,0x15C) = U(obj,0x12A);
                func_8002B6C8(obj, target, step);
            } else {
                H(obj,0x10E) = 0;
            }
            active = 1;
        }
        if (!active && H(obj,0x10E) == 0) {
            U(obj,0x10E) = U(obj,0x10C);
        }
    }
    if (!(B(obj,0x160) & 2) && H(obj,0x1C) < 5 &&
        B(obj,0x15F) != 7 && B(obj,0x15F) != 4 && B(obj,0x15F) != 6) {
        if (!func_8002B1DC(obj, 6, 0)) {
            mode = (F(obj,0x118) >> 24) & 15;
            if (mode >= 1 && mode <= 6) {
                F(obj,0x118) = (F(obj,0x118) & 0xF0FFFFFF) | ((mode + 8) << 24);
            }
            B(obj,0x15F) = 2;
            func_8002B1AC(obj);
        }
    }
    switch (B(obj,0x15F)) {
    case 0:
        if (valid) {
            func_8002B1DC(obj, 1, target);
        }
        if (H(obj,0x10E) == 0) {
            U(obj,0x10E) = U(obj,0x10C);
        }
        break;
    case 1:
        if (!valid) {
            func_8002B1DC(obj, 0, 0);
        }
        break;
    case 2:
        if (!P(obj,0x138)) {
            func_8002B1DC(obj, 0, 0);
        } else if (W(obj,0x84) < H(obj,0x158)) {
            func_8002B1AC(obj);
        }
        break;
    case 3:
        if ((F(obj,0x118) >> 24) & 15) {
            F(obj,0x118) &= 0xF0FFFFFF;
        }
        break;
    case 4:
        break;
    case 5:
        if (valid) {
            func_8002B1DC(obj, 1, target);
        } else if (!(F(obj,0x118) & 0x40)) {
            if (W(obj,0x84) == 0) {
                F(obj,0x118) |= 0x40;
            }
            if (H(obj,0x10E)) {
                H(obj,0x10E) = 0;
            }
        }
        break;
    case 6:
        if (H(obj,0x124) == H(obj,0x11E)) {
            if (H(obj,0xC0) >= 0) {
                U(obj,0xC0) = U(P(obj,0xA0),4);
            }
            U(obj,0x1C) = U(obj,0x1E);
            func_8002B1DC(obj, 0, 0);
        }
        break;
    }
    if (F(obj,0x118) & 0x40) {
        if (B(obj,0x15E) != B(obj,0x167)) {
            if (valid) {
                if (B(obj,0x15E) != B(obj,0x164)) {
                    func_8002B0EC(obj, B(obj,0x164));
                }
            } else if (B(obj,0x15E) != 0) {
                func_8002B0EC(obj, 0);
            }
        }
    } else {
        if (!(B(obj,0x160) & 1) && W(obj,0x84) <= (H(obj,0x158) >> 2) + 2) {
            if (B(obj,0x15E) != B(obj,0x165)) {
                func_8002B0EC(obj, B(obj,0x165));
            }
        } else if (B(obj,0x15E) != B(obj,0x166)) {
            func_8002B0EC(obj, B(obj,0x166));
        }
    }
}
