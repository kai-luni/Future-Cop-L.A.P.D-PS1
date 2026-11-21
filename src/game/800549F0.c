// Categories: Gameplay > Player > On-Foot Locomotion | Physics > Motion > Ground Follow | Graphics > Animation > Track Playback
#include "game/types.h"

struct Vec549F0 {
    s32 x;
    s32 y;
    s32 z;
};

struct AnimClip549F0 {
    u8 unk00[4];
    s16 start;
    s16 end;
    u8 unk08[8];
};

struct AnimTrack549F0 {
    struct AnimClip549F0 *clips;
    struct AnimClip549F0 *clip;
    s32 elapsed;
    s16 frame;
    s16 target;
    s8 step;
    u8 index;
    u8 count;
    u8 flags;
};

struct Sub549F0 {
    u8 pad00[0xDE];
    u8 unkDE;
    u8 unkDF;
    u8 padE0[0x66];
    s8 unk146;
    s8 unk147;
    u8 pad148[0x14];
    struct Vec549F0 velocity;
    u8 pad168[0x10];
};

struct Obj549F0 {
    u8 pad00[0x50];
    struct Vec549F0 position;
    u8 pad5C[0x1C];
    struct Vec549F0 velocity;
    s32 unk84;
    u8 pad88[0x18];
    struct Vec549F0 axis;
    struct Sub549F0 *sub;
    u16 angle;
    u8 padB2[6];
    u8 unkB8;
    u8 unkB9;
    u8 unkBA;
    u8 state;
    u8 padBC[6];
    u8 timer;
    u8 counter;
    u8 padC4[4];
    struct AnimTrack549F0 track;
};

/* Offsets are relative to the oracle's work area at sp + 0x18. */
struct Work549F0 {
    struct Vec549F0 delta;
    s32 pad0C;
    struct Vec549F0 probe;
    s32 pad1C;
    struct Vec549F0 previous;
    s32 pad2C;
    struct Vec549F0 normal;
    s32 pad3C;
    s32 magV;
    s32 magH;
    s32 surface;
    s32 pad4C;
    struct Vec549F0 from;
    s32 pad5C;
    struct Vec549F0 to;
    s32 pad6C;
};

extern s32 func_800538C8(u8 *, u32 *, s32 *, s32 *);
extern void func_80052E28(struct Obj549F0 *);
extern void func_80051AC8(struct Obj549F0 *, s32);
extern void gSinCos(u32, s32 *, s32 *) __asm__("func_8001846C");
extern s32 gGetGroundHeight(void *, void *, void *, void *) __asm__("func_80066F78");
extern void gComputeOnFootVelocityDelta(struct Obj549F0 *, s32, s32, void *, s32) __asm__("func_80054590");
extern s32 func_80052550(struct Obj549F0 *, void *);
extern void gClampToWorldBounds(s32 *) __asm__("func_8006943C");
extern s32 gTraceMapRay(void *, void *, s32, s32, s32) __asm__("func_800684AC");
extern void func_80053514(struct Obj549F0 *, s32);
extern void func_80053188(struct Obj549F0 *, s32);
extern struct AnimClip549F0 *func_800448D8(struct AnimTrack549F0 *, s32, s32, u8);
extern void gEaseVectorQuarter(void *, void *) __asm__("func_8001A6F8");
extern void gNormalizeVector(void *) __asm__("func_8001A180");
extern s32 func_80019FCC(s32 *);
extern s32 gSqrt12(s32) __asm__("func_80095900");
extern s32 func_8001A7AC(s32);
extern s32 func_80044B34(struct Obj549F0 *, struct AnimTrack549F0 *, s32, s32);
extern void gApplyAnimationSoundEvent(s32, struct Obj549F0 *) __asm__("func_80047088");
extern s32 gNormalizeGetLength(s32 *) __asm__("func_8001A314");
extern void gSetModelFrame(void *, s32) __asm__("func_800175F8");
extern s32 func_8005220C(struct Obj549F0 *);
extern void func_80052708(struct Obj549F0 *, s32);
extern s32 func_80052F54(struct Obj549F0 *, s32);
s32 gTickOnFootPhysics(struct Obj549F0 *selfArg, s32 unused, s32 speedArg, u32 argFlags) __asm__("func_800549F0");


/**
 * @brief Per-substep locomotion tick of a character on foot: maps the
 *        stick to a movement mode, integrates velocity with collision and
 *        ground snapping, and drives the animation track.
 *
 * @param selfArg Entity block. Position +0x50, velocity +0x78, speed
 *        +0x84, facing axis +0xA0, state-block pointer +0xAC, angle
 *        +0xB0, byte +0xB8, flag byte +0xB9, current clip +0xBA, state
 *        +0xBB, timer +0xC2, idle counter +0xC3, animation track +0xC8
 *        (clips +0xC8, clip +0xCC, elapsed +0xD0, frame +0xD4, target
 *        +0xD6, step +0xD8, index +0xD9, count +0xDA).
 * @param unused Frame step handed over by func_80055988; never read (the
 *        prologue casts it to void), so every iteration is one fixed
 *        base step.
 * @param speedArg Sub-step count: the body loops exactly this many times
 *        and returns early when the value is <= 0.
 * @param argFlags Entity flag word. func_800538C8 ORs the stick
 *        direction bits 0x20000000, 0x10000000, 0x80000000 and
 *        0x40000000 into it; bits 0x08000000, 0x02000000, 0x00200000
 *        and 0x00100000 are read from the caller's word.
 * @return The sum of every damage value func_80052F54 reported for a
 *         hard landing, 0 when the vertical speed never exceeded the
 *         limit.
 *
 * @par Confirmed
 * Prologue: sub = +0xAC, then func_800538C8(sub, &argFlags, &magV,
 * &magH) returns the controller status and writes the absolute stick
 * magnitudes. The entry mode is chosen in three branches:
 * 1. (argFlags & 0x0A000000) == 0x08000000, state +0xBB neither 3 nor
 *    0x0D/0x0E ((u32)(state - 0x0D) >= 2) and +0xB9 bit 0x40 clear: a
 *    probe at (position.x + axis.x >> 3, position.y + 0x10000,
 *    position.z + axis.z >> 3) is queried with func_80066F78 and its
 *    height above position.y selects 0x0F, or 0x0A once it reaches
 *    0x0AE2; reaching 0x2334 additionally sets +0xB9 bit 8 when the
 *    state block's +0xDF bit 0 is clear.
 * 2. argFlags & 0x02000000: mode 1/2/9/6/5 from 0x40000000, the sign
 *    bit, 0x08000000, 0x00200000 and 0x00100000.
 * 3. Otherwise: mode 3/4/8/7 from 0x40000000, the sign bit, 0x00200000
 *    and 0x00100000.
 * Both later branches add 0x40 for 0x20000000, otherwise 0x20 for
 * 0x10000000; only the low five bits (0x1F) are used as the target
 * state.
 * Per iteration the head clears +0xB9 bit 0x20 when bit 0x01 or 0x40 is
 * set, clears 0x02/0x10/0x20 while the state and the target state are
 * both 0, and derives action 1, (flag >> 4) & 2, or 2. Clip flag byte
 * (clip + 3): with bit 0x02 set and +0xB9 bit 0x02 set the action is
 * forced to 1, that bit is cleared and a non-zero target state is
 * stored; with bit 0x02 clear a differing target state only takes
 * effect when no clip is active (+0xD9 == 0) or +0xB9 bit 0x02 is set,
 * otherwise the state follows the input directly. +0xB9 bit 0x02 is
 * always cleared afterwards.
 * Grounded turns: when +0xB9 bit 0x00 is clear a turn step of 0x20
 * (bit 0x40), -0x20 (bit 0x20) or 0 is scaled by magH when the stick
 * read succeeded, reduced by (turn * +0x84) >> 9, added to the 12-bit
 * angle +0xB0, and clip 0x0E (turn <= 0) or 0x0F (turn > 0) is
 * requested. A non-zero +0xB8 runs func_80052E28(self) once and is
 * cleared. The angle is then masked to 0xFFF and the facing axis +0xA0
 * is rebuilt by func_8001846C(angle, &axis.x, &axis.z) with axis.y = 0;
 * the movement delta's y is cleared and the timer +0xC2 decrements.
 * Clip 0x80 marks the shrunken shadow: func_80051AC8(self, 0) restores
 * it and clears the bit whenever the state is not 9, while state 9
 * calls func_80051AC8(self, 1) and sets it.
 * State switch, with "speed" and "clip" as the values handed to
 * func_80054590 and to the track:
 * 0 idle: +0xC3 increments while the stick is neutral; a wrap to 0
 *   selects clip 0x10, and any direction resets the counter.
 * 1/2: speed 0xA000 / -0xA000, clips 6 / 7.
 * 3/4: speed 0x18000 / -0x18000, clips 8 / 9.
 * 5/6: speed -0xA000 / 0xA000, clips 1 / 2, turn channel signalled.
 * 7/8: speed -0x10000 / 0x10000, clips 0x0A / 0x0B, turn signalled.
 * 9: func_80051AC8(self, 1), +0xB9 bit 0x80 set, clip 3.
 * 10: clip 4. 11: vertical 0x78000 (0xA0000 with +0xDF bit 0), clip 4,
 *   timer 0x0A, state becomes 0x0C.
 * 13: speed 0x18000, vertical 0x64000 (0x85555), clip 8, state 0x0E.
 * 15/17: clip 0x11. 16: speed 0x40000, vertical 0x58000 (0x75555),
 *   clip 0x11, state 0x11. 18: clip 5. 12 and all other values: none.
 * The "vertical" values above are written into the y of the movement
 * delta. While +0xB9 bit 0x00 is set (airborne) the switch is skipped
 * and the current clip +0xBA is kept, except that timer 1 in state 0x0C
 * gives speed 0x1CCCC (unless +0xB9 bit 8) and clears bit 8.
 * Movement: for states 1..4 without the 0x08000000 flag the speed is
 * scaled by magV when the stick read succeeded, and direction 1
 * (0x00200000) or 2 (0x00100000) is set; then
 * func_80054590(self, speed, turn, &delta, direction) writes the delta
 * (it also adds its own -0x9800 gravity term).
 * Collision: func_80052550(self, &delta) returns a response code, the
 * delta is added into the state block velocity (+0x15C, +0x160, +0x164),
 * which is clamped at -0xC0000; with no collision and a per-case marker
 * (states 0, 9, 0x12) the largest absolute component is measured and
 * the whole velocity is zeroed when it is <= 0x7FFF with +0x147 == 0,
 * or below +0x147 << 9.
 * Integration: the previous position is saved, position += velocity
 * >> 8 for all three axes, func_8006943C(&position.x) clamps x and z
 * into the world rectangle, and a rising velocity (>= 0x11) clips the
 * segment previous + 0x24C0 to position + 0x24C0 with func_800684AC:
 * any of its low 3 bits zeroes velocity.y and halves x and z.
 * Ground: a probe at position.y + 0x0CCC returns the height, the normal
 * and the surface attribute word; with the state block's +0xDE set the
 * height is replaced by position.y - 0xC0 and velocity.y is cleared.
 * func_80053514(self, surface) attaches the actor to the returned
 * ground record, +0x146 and +0x147 are cleared, the attribute word is
 * masked with 0xFFFFFF3F and func_80053188(self, word) runs when
 * anything remains. The ground normal is then low-pass filtered into
 * +0x168 with func_8001A6F8 and normalised with func_8001A180.
 * Landing: below the height, an airborne actor clears +0xB9 bit 0x00,
 * sets bit 0x02, keeps the negated vertical velocity as its speed and,
 * when that exceeds 0x50000 (0x80000 with clip flag 0x10), switches to
 * state 0x12 and adds func_80052F54(self, speed) to the returned value;
 * in state 0x0C a velocity.y below -0x40 is simply zeroed. A height
 * gain of 0x333 or more goes to the fallback path, otherwise the
 * position is snapped to the height.
 * Fallback: states 1, 2 and 5 with +0xB9 bit 0x40 clear and +0xDE == 0
 * reset the state to 0, re-query the ground at the previous position +
 * 0x0CCC and, when the actor is at or within 0x333 of that floor,
 * restore the previous position, apply its attribute word through
 * func_80053188 and zero the horizontal velocity; otherwise +0xB9 bit
 * 0x00 is set (airborne).
 * After the ground decision state 0x0E forces bit 0x00 when bit 0x02 is
 * clear, and func_80052708(self, func_8005220C(self)) refreshes the
 * contact record with the collision mask.
 * Animation: a clip change happens when the requested index differs
 * from +0xBA, or when no clip is active (+0xD9 == 0) and the index is
 * non-zero; func_800448D8(&track, index, 1, 1) performs it and +0xBA
 * caches it. The track step rate is a fixed 0xA0 for states 9, 10, 0x0B,
 * 0x0C, 0x0F, 0x11 and 0x12, otherwise
 * func_80095900(func_80019FCC(&velocity.x)) << 2 >> 8 scaled by
 * (4 - +0x147), with minima of 0x300 in state 0x0E, 0x100 in states
 * 1..4 and 0x1E elsewhere.
 * Normal test: the planar part (normal.x, normal.z) is measured with
 * func_8001A7AC, +0xB9 bit 0x40 is cleared and set again when the
 * magnitude exceeds 0xA000.
 * Action 2 additionally switches the landing clip 0x37 or 0x36 by the
 * current frame +0xD4, sets state 0x0D and clears +0xB9 bits 0x10 and
 * 0x20.
 * func_80044B34(self, &track, speed, action) returns the packed event
 * word: bit 0 clear of 0x10 means a clip end (it clears +0xB9 bit
 * 0x10), a non-zero action sets +0xB9 bit 8 << action, and the 6-bit
 * fields above bit 0 are dispatched one by one through
 * func_80047088(event, self).
 * Clip flag 0x04 promotes state 0x0A to 0x0B and 0x0F to 0x10; an
 * exhausted track (+0xD9 == 0 with event bit 0) returns the state to 0
 * and emits event 0x27.
 * Epilogue: velocity = state block velocity, +0x84 =
 * func_8001A314(&velocity.x) >> 11 with a zero length falling back to
 * the axis vector, and func_800175F8(self + 0x28, +0xD4) applies the
 * frame to the model instance.
 *
 * @par Likely role
 * The on-foot player character controller, selected by func_80055988
 * over its twin func_80053A74 when the mode byte +0xAD is not 1: the
 * twin shares this signature and the same stick decoding but runs a
 * different set of speeds. States 1..8 are the eight ground
 * locomotion directions (two walking, two running, two side-step and
 * two strafe variants), 9..0x12 the jump, fall, landing and hit
 * sequence, and the returned damage is what the caller would forward to
 * the HUD; +0xB9 bit 0x00 is the airborne flag, bit 0x02 a "landed this
 * step" latch, bit 0x40 "steep ground", bit 0x80 "shrunken shadow".
 *
 * @par Unknown
 * The names of the individual clip indices, the meaning of the state
 * block bytes +0xDE, +0xDF, +0x146 and +0x147 (only used as a flag, a
 * slowdown multiplier and a velocity threshold), the 0x0AE2 and 0x2334
 * probe thresholds, and why the frame step argument is ignored while
 * the sub-step count drives the loop.
 *
 * @par Callers
 * func_80055988 only (call site 0x80055DC0, analysis/edges.json kind
 * "direct"); it discards the returned damage and runs this function
 * whenever the mode byte +0xAD is not 1.
 */
s32 gTickOnFootPhysics(struct Obj549F0 *selfArg, s32 unused, s32 speedArg, u32 argFlags)
{
    register struct Obj549F0 *self __asm__("$17") = selfArg;
    s32 speed = speedArg;
    struct Work549F0 w;
    s32 wflags;
    s32 fallbackFlags;
    s32 fallbackSurface;
    register struct Sub549F0 *sub __asm__("$20");
    register s32 move __asm__("$21");
    register s32 anim __asm__("$19");
    s32 action;
    register s32 spare __asm__("$23");
    register s32 input __asm__("$30");
    s32 impactTotal;
    s32 remaining;
    u8 status;
    struct Vec549F0 *normalPtr;
    s32 lane;
    s32 st;

    (void)unused;
    impactTotal = 0;
    sub = self->sub;
    move = 0;
    status = (u8)func_800538C8((u8 *)sub, &argFlags, &w.magV, &w.magH);

    if ((argFlags & 0x0A000000) == 0x08000000 &&
        (st = self->state) != 3 &&
        (u32)(st - 0x0D) >= 2 &&
        (self->unkB9 & 0x40) == 0) {
        w.probe.x = self->position.x + (self->axis.x >> 3);
        w.probe.y = self->position.y + 0x10000;
        w.probe.z = self->position.z + (self->axis.z >> 3);
        lane = gGetGroundHeight(&w.probe, (void *)move, (void *)move,
                            &w.surface) - self->position.y;
        move = 0x0F;
        if (lane >= 0x0AE2) {
            move = 0x0A;
            if ((sub->unkDF & 1) == 0) {
                if (lane >= 0x2334) {
                    self->unkB9 |= 8;
                }
            }
        }
    } else if (argFlags & 0x02000000) {
        if (argFlags & 0x40000000) {
            move = 1;
        } else if ((s32)argFlags < 0) {
            move = 2;
        } else if (argFlags & 0x08000000) {
            move = 9;
        } else if (argFlags & 0x00200000) {
            move = 6;
        } else if (argFlags & 0x00100000) {
            move = 5;
        }
        if (argFlags & 0x20000000) {
            move |= 0x40;
        } else if (argFlags & 0x10000000) {
            move |= 0x20;
        }
    } else {
        if (argFlags & 0x40000000) {
            move = 3;
        } else if ((s32)argFlags < 0) {
            move = 4;
        } else if (argFlags & 0x00200000) {
            move = 8;
        } else if (argFlags & 0x00100000) {
            move = 7;
        }
        if (argFlags & 0x20000000) {
            move |= 0x40;
        } else if (argFlags & 0x10000000) {
            move |= 0x20;
        }
    }

    remaining = speed;
    if (speed <= 0) {
        goto end;
    }
    input = move & 0x1F;
    normalPtr = &w.normal;

loop:
    anim = 0;
    if ((self->unkB9 & 1) == 0) {
        if ((u32)(self->state - 1) < 8 || self->state == 0x0E) {
            if (move == 0) {
                self->state = 0;
            }
        }
    }

    spare = 0;
    if (self->unkB9 & 0x41) {
        self->unkB9 &= 0xDF;
    }
    if (self->state == 0 && input == 0) {
        self->unkB9 &= 0xCD;
    }

    action = 1;
    if ((self->unkB9 & 0x10) == 0) {
        action = (self->unkB9 >> 4) & 2;
    }
    if ((argFlags & 0x08000000) && self->state == 3 && action == 0) {
        action = 2;
        goto after_action;
    }

    if (*(u8 *)((u8 *)self->track.clip + 3) & 2) {
        st = self->unkB9;
        if (st & 2) {
            action = 1;
            self->unkB9 = st & 0xFD;
            if (input != 0) {
                self->state = input;
            }
        }
    } else {
        if (input != self->state) {
            if (self->track.index != 0 || (self->unkB9 & 2) != 0) {
                action = 1;
                self->unkB9 &= 0xFD;
                goto after_action;
            }
        }
        self->state = input;
    }

after_action:
    {
        s32 bits;

        st = self->unkB9;
        bits = st & 0xFD;
        st &= 1;
        self->unkB9 = bits;
    }
    if (st == 0) {
        register s32 turn __asm__("$4");

        turn = 0x20;
        if ((move & 0x40) == 0) {
            turn = 0;
            if (move & 0x20) {
                turn = -0x20;
            }
        }
        if (status != 0) {
            turn = (turn * w.magH) >> 7;
        }
        if (turn != 0) {
            turn -= (turn * self->unk84) >> 9;
            self->angle = self->angle + turn;
            anim = 0x0E;
            if (turn > 0) {
                anim = 0x0F;
            }
        }
    }

    if (self->unkB8 != 0) {
        u8 st;

        st = self->state;
        if (st != input) {
            if ((u32)(self->state - 1) < 8 || self->state == 0x0E) {
                self->state = 0;
            }
        }
        func_80052E28(self);
        self->unkB8 = 0;
    }
    {
        u32 angle;

        angle = self->angle & 0x0FFF;
        self->angle = angle;
        gSinCos(angle, &self->axis.x, &self->axis.z);
    }
    self->axis.y = 0;
    w.delta.y = 0;
    if (self->timer != 0) {
        self->timer--;
    }

    speed = 0;
    lane = speed;
    if (self->unkB9 & 0x80) {
        if (self->state != 9) {
            func_80051AC8(self, speed);
            self->unkB9 &= 0x7F;
        }
    }

    {
        s32 flags;

        flags = self->unkB9;
        if (flags & 1) {
            goto timer_handler;
        }
        switch (self->state) {
        case 9:
            spare = 1;
            anim = 3;
            if ((self->unkB9 & 0x80) == 0) {
                func_80051AC8(self, spare);
            }
            self->unkB9 |= 0x80;
            break;
        case 10:
            anim = 4;
            break;
        case 11:
            {
                s32 vertical;

                vertical = 0x78000;
                if (sub->unkDF & 1) {
                    vertical = 0xA0000;
                }
                anim = 4;
                w.delta.y = vertical;
                self->timer = 0x0A;
                self->state = 0x0C;
            }
            break;
        case 16:
            {
                s32 vertical;

                vertical = 0x58000;
                if (sub->unkDF & 1) {
                    vertical = 0x75555;
                }
                speed = 0x40000;
                anim = 0x11;
                w.delta.y = vertical;
                self->state = 0x11;
            }
            break;
        case 15:
        case 17:
            anim = 0x11;
            break;
        case 13:
            {
                s32 vertical;

                speed = 0x18000;
                vertical = 0x64000;
                if (sub->unkDF & 1) {
                    vertical = 0x85555;
                }
                anim = 8;
                w.delta.y = vertical;
                self->state = 0x0E;
            }
            break;
        case 18:
            spare = 1;
            anim = 5;
            break;
        case 3:
            speed = 0x18000;
            /* Retail case 14 enters after this assignment. */
        case 14:
            anim = 8;
            break;
        case 4:
            speed = -0x18000;
            anim = 9;
            break;
        case 1:
            speed = 0xA000;
            anim = 6;
            break;
        case 2:
            speed = -0xA000;
            anim = 7;
            break;
        case 6:
            anim = 2;
            speed = 0xA000;
            lane = 1;
            break;
        case 5:
            anim = 1;
            speed = -0xA000;
            lane = 1;
            break;
        case 8:
            anim = 0x0B;
            speed = 0x10000;
            lane = 1;
            break;
        case 7:
            anim = 0x0A;
            speed = -0x10000;
            lane = 1;
            break;
        case 0:
            spare = 1;
            if (move == 0) {
                self->counter++;
                if (self->counter == 0) {
                    anim = 0x10;
                    break;
                }
                if (self->track.index == 0) {
                    break;
                }
                if (self->unkBA != 0x10) {
                    break;
                }
                anim = self->unkBA;
            }
            self->counter = 0;
            break;
        case 12:
        default:
            break;
        }
        goto dispatch_done;

timer_handler:
        anim = self->unkBA;
        if (self->timer == 1 && self->state == 0x0C) {
            if ((flags & 8) == 0) {
                speed = 0x1CCCC;
            }
            self->unkB9 = flags & 0xF7;
        }
    }

dispatch_done:
    {
        s32 direction;

        direction = 0;
        if ((u32)(self->state - 1) < 4) {
            if ((argFlags & 0x08000000) == 0) {
                if (status != 0) {
                    speed = (speed * w.magV) >> 7;
                }
                if (argFlags & 0x00200000) {
                    direction = 1;
                } else if (argFlags & 0x00100000) {
                    direction = 2;
                }
            }
        }
        gComputeOnFootVelocityDelta(self, speed, lane, &w.delta, direction);
    }

    {
        s32 collision;

        collision = func_80052550(self, &w.delta);
        sub->velocity.x += w.delta.x;
        sub->velocity.y += w.delta.y;
        sub->velocity.z += w.delta.z;
        if (sub->velocity.y < -0xC0000) {
            sub->velocity.y = -0xC0000;
        }
        if (collision == 0 && spare != 0) {
            s32 value;
            s32 sign;
            s32 component;
            s32 difference;
            s32 response;

            s32 vx, vy, vz;

            vx = sub->velocity.x;
            vy = sub->velocity.y;
            speed = ((vx >> 31) + vx ^ (vx >> 31));
            component = ((vy >> 31) + vy ^ (vy >> 31));
            difference = component - speed;
            speed = component - ((difference >> 31) & difference);
            vz = sub->velocity.z;
            component = ((vz >> 31) + vz ^ (vz >> 31));
            difference = component - speed;
            speed = component - ((difference >> 31) & difference);
            response = self->sub->unk147;
            if ((speed <= 0x7FFF && response == 0) ||
                speed < (response << 9)) {
                sub->velocity.x = sub->velocity.y = sub->velocity.z = 0;
            }
        }
    }

    w.previous = self->position;
    self->position.x += sub->velocity.x >> 8;
    self->position.y += sub->velocity.y >> 8;
    self->position.z += sub->velocity.z >> 8;
    gClampToWorldBounds(&self->position.x);

    if (sub->velocity.y >= 0x11) {
        w.from.x = w.previous.x;
        w.from.y = w.previous.y + 0x24C0;
        w.from.z = w.previous.z;
        w.to.x = self->position.x;
        w.to.y = self->position.y + 0x24C0;
        w.to.z = self->position.z;
        if (gTraceMapRay(&w.from, &w.to, 0, 0, 1) & 7) {
            s32 vx;
            s32 vz;

            vx = sub->velocity.x;
            vz = sub->velocity.z;
            sub->velocity.y = 0;
            sub->velocity.x = vx / 2;
            sub->velocity.z = vz / 2;
        }
    }

    w.probe.x = self->position.x;
    w.probe.y = self->position.y + 0x0CCC;
    w.probe.z = self->position.z;
    lane = gGetGroundHeight(&w.probe, normalPtr, &wflags, &w.surface);
    if (self->sub->unkDE != 0) {
        lane = self->position.y - 0xC0;
        self->sub->velocity.y = 0;
    }
    func_80053514(self, w.surface);
    self->sub->unk147 = 0;
    self->sub->unk146 = 0;
    spare = (s32)0xFFFFFF3F;
    wflags &= spare;
    if (wflags != 0) {
        func_80053188(self, wflags);
    }

    speed = (s32)((u8 *)sub + 0x168);
    gEaseVectorQuarter(normalPtr, (void *)speed);
    gNormalizeVector((void *)speed);

    if (self->position.y < lane) {
        if (self->unkB9 & 1) {
            s32 vertical;
            s32 limit;

            self->unkB9 = (self->unkB9 & 0xFE) | 2;
            vertical = sub->velocity.y;
            sub->velocity.y = 0;
            limit = 0x50000;
            speed = -vertical;
            if (*(u8 *)((u8 *)self->track.clip + 3) & 0x10) {
                limit = 0x80000;
            }
            if (limit < speed) {
                s32 damage;

                self->state = 0x12;
                damage = func_80052F54(self, speed);
                impactTotal += damage;
            }
        } else if (self->state == 0x0C && sub->velocity.y < -0x40) {
            sub->velocity.y = 0;
        }
    } else if (self->position.y - lane >= 0x333) {
        goto above_ground;
    }
    self->position.y = lane;
    goto after_ground;

above_ground:
    if ((u32)(self->state - 1) < 2 || self->state == 5) {
        if ((self->unkB9 & 0x40) == 0 && self->sub->unkDE == 0) {
            s32 floor;

            anim = 0;
            self->state = 0;
            w.probe.x = w.previous.x;
            w.probe.y = w.previous.y + 0x0CCC;
            w.probe.z = w.previous.z;
            floor = gGetGroundHeight(&w.probe, (void *)anim,
                                 &fallbackFlags, &fallbackSurface);
            if (self->position.y < floor ||
                self->position.y - floor < 0x333) {
            {
                s32 fb;
                s32 masked;

                fb = fallbackFlags;
                self->position = w.previous;
                masked = fb & spare;
                fallbackFlags = masked;
                if (masked != 0) {
                    func_80053188(self, masked);
                }
                sub->velocity.x = 0;
                sub->velocity.z = 0;
                goto after_ground;
            }
            }
        }
    }
    self->unkB9 |= 1;

after_ground:
    if (self->state == 0x0E) {
        s32 bits;

        bits = self->unkB9;
        if ((bits & 2) == 0) {
            self->unkB9 = bits | 1;
        }
    }
    func_80052708(self, func_8005220C(self));

    if (anim != self->unkBA) {
        goto change_animation;
    }
    if (self->track.index != 0) {
        goto animation_done;
    }
    if (anim == 0) {
        goto animation_done;
    }
change_animation:
    func_800448D8(&self->track, anim, 1, 1);
    self->unkBA = anim;

animation_done:
    if (self->state == 0x0A) goto fixed_speed;
    if (self->state == 0x0F) goto fixed_speed;
    if (self->state == 0x0B) goto fixed_speed;
    if (self->state == 0x0C) goto fixed_speed;
    if (self->state == 0x12) goto fixed_speed;
    if (self->state == 9) goto fixed_speed;
    if (self->state == 0x11) goto fixed_speed;
    goto measured_speed;

fixed_speed:
    speed = 0xA0;
    goto speed_done;

measured_speed:
    speed = gSqrt12(func_80019FCC(&sub->velocity.x)) << 2;
    speed >>= 8;
    if (self->sub->unk147 != 0) {
        speed *= 4 - self->sub->unk147;
    }
    st = self->state;
    if (st == 0x0E) {
        if (speed < 0x300) {
            speed = 0x300;
        }
    } else if ((u32)(st - 1) < 4) {
        if (speed < 0x100) {
            speed = 0x100;
        }
    } else if (speed < 0x1E) {
        speed = 0x1E;
    }

speed_done:
    w.from.x = w.normal.x;
    w.from.y = w.normal.z;
    {
        s32 magnitude;

        magnitude = func_8001A7AC((s32)&w.from);
        self->unkB9 &= 0xBF;
        if (0xA000 < magnitude) {
            self->unkB9 |= 0x40;
        }
    }

    if (action == 2) {
        if ((self->unkB9 & 1) == 0) {
            if ((self->unkB9 & 0x40) == 0) {
                s32 frame;
                s32 index;

                frame = self->track.frame;
                index = 0x37;
                if ((u32)(frame - 2) >= 4 &&
                    (u32)(frame - 0x12) >= 0x0E) {
                    index = 0x36;
                }
                func_800448D8(&self->track, index, 1, 1);
                frame = self->unkB9;
                self->state = 0x0D;
                frame &= 0xCF;
                self->unkB9 = frame;
            }
            action = 0;
        }
    }

    lane = func_80044B34(self, &self->track, speed, action);
    if (lane & 1) {
        self->unkB9 &= 0xEF;
    } else if (action != 0) {
        self->unkB9 |= 8 << action;
    }

    speed = (s32)((u32)lane >> 1);
    while (speed != 0) {
        s32 event;

        event = speed & 0x3F;
        speed >>= 6;
        gApplyAnimationSoundEvent(event, self);
    }

    if (*(u8 *)((u8 *)self->track.clip + 3) & 4) {
        if (self->state == 0x0A) {
            self->state = 0x0B;
        } else if (self->state == 0x0F) {
            self->state = 0x10;
        }
    }
    if (self->track.index == 0 && (lane & 1)) {
        self->state = 0;
        gApplyAnimationSoundEvent(0x27, self);
    }

    if (--remaining > 0) {
        goto loop;
    }

end:
    self->velocity = sub->velocity;
    self->unk84 = gNormalizeGetLength(&self->velocity.x) >> 11;
    if (self->unk84 == 0) {
        self->velocity = self->axis;
    }
    gSetModelFrame((u8 *)self + 0x28, self->track.frame);
    return impactTotal;
}
