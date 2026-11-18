// Categories: Physics > Motion > Ground Follow | Gameplay > Player
#include "game/types.h"

typedef struct { s32 x, y, z; } Vec3;

#define FMUL(d, a, b) \
    __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" \
            : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")
#define FM(a, b) ({ s32 r_, a_ = (a), b_ = (b); __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a_), "r"(b_) : "$8", "$9"); r_; })
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

extern u8 func_800538C8(void *context, u32 *flags, s32 *v, s32 *h);
extern void func_800471C4(s32 anim, u8 *o);
extern void func_80052E28(u8 *o);
extern void gSinCos(s32 angle, s32 *sin, s32 *cos) __asm__("func_8001846C");
extern void func_800537D0(u8 *o, s32 a1, s32 a2, s32 *v, s32 t);
extern void gGteCrossProduct(s32 *a, s32 *b, s32 *c) __asm__("func_80010974");
extern s32 func_8001A7AC(s32 *v);
extern void func_8001ABE0(s32 scale, s32 *v);
extern s32 func_80052550(u8 *o, s32 *v);
extern void func_8006943C(s32 *p);
extern s32 func_80066F78(s32 *a, s32 *b, s32 *c, s32 *d);
extern void func_80053514(u8 *o, s32 a);
extern void func_80053188(u8 *o, s32 a);
extern s32 func_80052F54(u8 *o);
extern void func_8001AD20(s32 *a, s32 *b, s32 *c);
extern s32 func_8005220C(u8 *o);
extern void func_80052708(u8 *o, s32 a);
extern s32 func_8001A314(s32 *v);

/**
 * @brief Runs the hover-form physics tick: drive mode and steering from the
 *        input flags, thrust, gravity, ground snap, world bounds and the
 *        impact bookkeeping, then publishes the movement direction and speed.
 *
 * @param o_ Entity block. +0xAC is the state block s used below, +0x50/+0x54/
 *        +0x58 the world position, +0xB0 the heading (12-bit units) with its
 *        sine at +0xA0 and cosine at +0xA8 (+0xA4 is held at 0), +0xB8 a
 *        one-shot "was just transformed" byte, +0xB9 a status byte (bit 0 =
 *        airborne, bit 1 = ground contact), +0xBB the drive mode, +0xF2 the
 *        body lift, +0xF0/+0x84 scratch, +0x78/+0x84 the published direction
 *        and speed.
 * @param dt Frame delta in the tick unit of the state timers.
 * @param n_ Number of physics sub-steps the loop executes.
 * @param flags Input/state word. Bits 0x80000000/0x40000000 (the two y-axis
 *        directions, set by func_800538C8) and 0x200000/0x100000 (two pad
 *        buttons) select the drive mode, 0x20000000/0x10000000 (the two x-axis
 *        directions) only steer through the bank angle, and 0x2000000 selects
 *        the slower of the two speed classes for the same input.
 * @return Accumulated damage: the sum of func_80052F54 over the sub-steps that
 *         registered a hard landing, 0 when none did.
 *
 * @par Confirmed
 * 1. func_800538C8(s, &flags, &mag[0], &mag[1]) polls the controller while no
 *    direction bit is set and s +0xC0 / s +0x310 are zero: it ORs 0x20000000/
 *    0x10000000 for the x axis and 0x80000000/0x40000000 for the y axis into
 *    flags, writes the two absolute magnitudes and returns the analog status
 *    in r74; +0xC1 is passed as the controller index.
 * 2. The mode table maps the button bits onto mode numbers 1..8 plus 0x13, and
 *    0x20000000/0x10000000 add 0x40/0x20 (the steering terms). mode & 0x1F is
 *    stored at +0xBB; the animation track is 0x28 by default, 0x29 for every
 *    other mode and 0x2A for 0x13, and func_800471C4(anim, o) starts it.
 * 3. s +0xDC is a throttle ramp: it grows by dt while the mode is 0 and is
 *    clamped at 0x800, and is reset to 0 for every other mode.
 * 4. Per sub-step the bank angle s +0xC2 is either stamped from mag[1] (0x40:
 *    mag[1] * 3 >> 2, 0x20: -(mag[1] * 96) >> 7, else 0) when r74 is set, or
 *    ramped by dt towards 0 and clamped to -0x60..0x60. The heading at +0xB0 is
 *    advanced by (s +0xC2 << 16) >> 17, or >> 19 when +0xB8 is set.
 * 5. scale is 0xCCC, grows once the throttle ramp passes 0x700
 *    ((s +0xDC - 0x700) * 21300 / 256 + 0xCCC) and becomes 0x6000 while +0xB8
 *    is set, in which case the mode is also reset through func_80052E28(o).
 *    func_8001846C(angle, o +0xA0, o +0xA8) refreshes sine and cosine.
 * 6. The mode switch picks the thrust (0x6666/0x3333 forward, -0x6666/-0x3333
 *    back, -/+0x1000 and -/+0x2000 for the strafe modes, which also pass
 *    a2 = 1); for modes 1..4 the thrust is pre-scaled by mag[0] while r74 is
 *    set, and t0 = 1 or 2 selects the strafe-button variant. func_800537D0
 *    builds the local delta at v18, func_80010974 is applied twice (s +0x168
 *    and back) to express it in world space.
 * 7. Grounded (bit 0 of +0xB9 clear): a push derived from the horizontal part
 *    of the vector at s +0x168 is added, its length taken by func_8001A7AC and
 *    re-applied by func_8001ABE0 with a quadratic falloff above 0x4000, plus a
 *    lift term from s +0x16C clamped to 0..0x10000. Both cases subtract
 *    velocity * scale and add -0x9800 (gravity).
 * 8. func_80052550(o, &v18) folds in any pending external impulse
 *    (s +0xEC..0xF4) and returns 1 when it did. The velocity s +0x15C/+0x160/
 *    +0x164 is then integrated, its vertical part clamped at -0xC0000 and its
 *    horizontal length at 0x92147 through func_8001A7AC/func_8001ABE0.
 * 9. Mode 0 with no impulse: the velocity is zeroed when the smallest absolute
 *    component is below 0x2000.
 * 10. Position: o +0x50/+0x54/+0x58 += velocity >> 8 and func_8006943C clamps
 *    x and z to the world rectangle D_800A2950 with a 0x800 margin.
 * 11. Ground contact is sampled at (x, y + 0xCCC, z): func_80066F78 returns
 *    the height and writes the normal to s +0x168, the surface attribute to
 *    v60 and the surface record to v64; func_80053514(o, v64) applies the
 *    record, s +0x146/+0x147 are cleared, and func_80053188(o, v60 & ~0xC0)
 *    runs when that byte survives the mask.
 * 12. y below the ground: the body is snapped up, bit 1 of +0xB9 set and the
 *    downward velocity dropped; when the rebound speed exceeds 0x50000 the
 *    mode becomes 0x12 and func_80052F54(o) is added to the return value. A gap
 *    below 0xCC snaps silently, and modes 1/2 with a larger gap either abort
 *    back to the previous position or set bit 0 (airborne).
 * 13. func_80052708(o, func_8005220C(o)) refreshes the animation state at the
 *    end of every sub-step.
 * 14. After the loop o +0x78 receives the velocity, func_8001A314 normalises
 *    it in place and returns its length, +0x84 = length >> 11, and when the
 *    length is 0 the direction falls back to (o +0xA0, o +0xA4, o +0xA8), the
 *    heading vector.
 *
 * @par Likely role
 * The drive model of the player unit's hover/vehicle form (the mode byte, the
 * 16.16 constants 0xCCCC = 0.8, 0x6666 = 0.4, 0x3333 = 0.2, 0x9800 = 0.59
 * gravity, 0x92147 = 9.1 horizontal speed limit) with a bank angle that reads
 * as the lean of a hovercraft. The chassis that spawns in this form is chosen
 * by func_800573A4, and func_80055988 dispatches here whenever state +0xAD is 1.
 *
 * @par Unknown
 * Which physical pad buttons 0x100000/0x200000 are (Likely a lateral/strafing
 * pair, since they select modes 5..8, whose thrust uses the perpendicular axis
 * of func_800537D0), which of the two y directions is "forward", what 0x2000000
 * selects (Likely a precision/brake modifier, since the same button then yields
 * half the thrust, 0x3333 instead of 0x6666), what status
 * bit 1 of +0xB9 records, what s +0xDC accumulates, what the offsets 0xCCC /
 * 0x800 / 0x384 measure, and the semantics of mode 0x13 and of the -0x4000 /
 * 0x1A000 style bias values in the ground push.
 *
 * @par Callers
 * func_80055988, the form dispatcher, when state +0xAD is 1.
 *
 * @par Environment path
 * The hover/vehicle drive tick of the player: it polls the controller with
 * func_800538C8, maps the input flags onto eight drive modes plus 0x13, ramps
 * the throttle at +0xDC and the bank angle at +0xC2, applies thrust through
 * func_800537D0 and expresses it in world space with func_80010974, then
 * integrates velocity, applies gravity and the horizontal speed limit, moves
 * the position, clamps it with func_8006943C and snaps to the ground through
 * func_80066F78, func_80053514 and func_80053188 before publishing direction
 * and speed at +0x78/+0x84. Its only caller is the form dispatcher
 * func_80055988, when state +0xAD is 1. Not on the render path: it is local
 * physics and writes no packet, no GTE matrix and no camera.
 */
s32 func_80053A74(u8 *o_, s32 dt, s32 n_, u32 flags)
{
    Vec3 v18;
    Vec3 v28;
    Vec3 v38;
    s32 mag[2];
    s32 v50[2];
    s32 v58[2];
    s32 v60, v64;
    s32 v68, v6C;
    s32 ret;
    u8 r74;
    u8 *s;
    register s32 i __asm__("$23");
    s32 fp;
    s32 a1, a2, t0;
    s32 scale, vv, res;
    register u8 *o __asm__("$18");
    register s32 count __asm__("$19");
    register u32 mode __asm__("$20");
    register s32 anim __asm__("$16");

    o = o_;
    count = n_;
    __asm__("" : : "r"(o), "r"(count));
    mode = 0;
    ret = 0;
    s = *(u8 **)(o + 0xAC);
    r74 = func_800538C8(s, &flags, &mag[0], &mag[1]);

    anim = 0x28;
    if (flags & 0x2000000) {
        if (flags & 0x8000000) {
            mode = 0x13;
            anim = 0x2A;
        } else if (flags & 0x40000000) {
            mode = 1;
            anim = 0x29;
        } else if ((s32)flags < 0) {
            mode = 2;
            anim = 0x29;
        } else if (flags & 0x200000) {
            mode = 6;
            anim = 0x29;
        } else if (flags & 0x100000) {
            mode = 5;
            anim = 0x29;
        }
        if (flags & 0x20000000) {
            mode |= 0x40;
            anim = 0x29;
        } else if (flags & 0x10000000) {
            mode |= 0x20;
            anim = 0x29;
        }
    } else {
        if (flags & 0x8000000) {
            mode = 0x13;
            anim = 0x2A;
        } else if (flags & 0x40000000) {
            mode = 3;
            anim = 0x29;
        } else if ((s32)flags < 0) {
            mode = 4;
            anim = 0x29;
        } else if (flags & 0x200000) {
            mode = 8;
            anim = 0x29;
        } else if (flags & 0x100000) {
            mode = 7;
            anim = 0x29;
        }
        if (flags & 0x20000000) {
            mode |= 0x40;
            anim = 0x29;
        } else if (flags & 0x10000000) {
            mode |= 0x20;
            anim = 0x29;
        }
    }
    *(u8 *)(o + 0xBB) = mode & 0x1F;
    if (*(u8 *)(o + 0xBB) == 0) {
        *(u16 *)(s + 0xDC) += (u16)dt;
        if (*(s16 *)(s + 0xDC) > 0x800) {
            *(s16 *)(s + 0xDC) = 0x800;
        }
    } else {
        *(s16 *)(s + 0xDC) = 0;
    }
    func_800471C4(anim, o);

    i = count;
    if (i > 0) {
        s32 *s6, *s5;
        s6 = (s32 *)(s + 0x168);
        s5 = &v28.x;
        for (; i > 0; i--) {
            fp = 0;
            if (r74) {
                if (mode & 0x40) {
                    *(s16 *)(s + 0xC2) = (u32)(mag[1] * 3) >> 2;
                } else if (mode & 0x20) {
                    *(s16 *)(s + 0xC2) = (-(mag[1] * 96)) >> 7;
                } else {
                    *(s16 *)(s + 0xC2) = 0;
                }
            } else {
                if (mode & 0x40) {
                    if (*(s16 *)(s + 0xC2) < 0) {
                        *(s16 *)(s + 0xC2) = 0;
                    }
                    *(u16 *)(s + 0xC2) += (u16)dt;
                } else if (mode & 0x20) {
                    if (*(s16 *)(s + 0xC2) > 0) {
                        *(s16 *)(s + 0xC2) = 0;
                    }
                    *(u16 *)(s + 0xC2) -= (u16)dt;
                } else {
                    *(s16 *)(s + 0xC2) = 0;
                }
                {
                    s16 angle;
                    s32 uangle;

                    angle = *(s16 *)(s + 0xC2);
                    uangle = *(u16 *)(s + 0xC2);
                    if (angle >= -0x60) {
                        if (angle >= 0x61) {
                            uangle = 0x60;
                        }
                    } else {
                        uangle = -0x60;
                    }
                    *(u16 *)(s + 0xC2) = uangle;
                }
            }
            *(s16 *)(o + 0xB0) += (s32)((*(u16 *)(s + 0xC2) << 16) >>
                                        (*(u8 *)(o + 0xB8) ? 19 : 17));
            scale = 0xCCC;
            if (*(s16 *)(s + 0xDC) >= 0x701) {
                scale = (*(s16 *)(s + 0xDC) - 0x700) * 21300 / 256 + 0xCCC;
            }
            if (*(u8 *)(o + 0xB8) != 0) {
                scale = 0x6000;
                if (*(u8 *)(o + 0xBB) != (mode & 0x1F) &&
                    (u32)(*(u8 *)(o + 0xBB) - 1) < 8) {
                    *(u8 *)(o + 0xBB) = 0;
                }
                func_80052E28(o);
                *(u8 *)(o + 0xB8) = 0;
            }
            {
                s32 angle = *(u16 *)(o + 0xB0) & 0xFFF;

                *(u16 *)(o + 0xB0) = angle;
                gSinCos(angle, (s32 *)(o + 0xA0), (s32 *)(o + 0xA8));
            }
            a1 = 0;
            *(s32 *)(o + 0xA4) = 0;
            v18.y = 0;
            a2 = 0;
            switch (*(u8 *)(o + 0xBB)) {
            case 3:
                a1 = 0x6666;
                break;
            case 4:
                a1 = -0x6666;
                break;
            case 1:
                a1 = 0x3333;
                break;
            case 2:
                a1 = -0x3333;
                break;
            case 6:
                a1 = 0x1000;
                a2 = 1;
                break;
            case 5:
                a1 = -0x1000;
                a2 = 1;
                break;
            case 8:
                a1 = 0x2000;
                a2 = 1;
                break;
            case 7:
                a1 = -0x2000;
                a2 = 1;
                break;
            case 19:
                if (!(*(u8 *)(o + 0xB9) & 1)) {
                    scale = 0x2666;
                }
                break;
            case 0:
                if (!(*(u8 *)(o + 0xB9) & 1)) {
                    fp = 1;
                }
                break;
            }
            t0 = 0;
            if ((u32)(*(u8 *)(o + 0xBB) - 1) < 4) {
                if (r74) {
                    a1 = (a1 * mag[0]) >> 7;
                }
                if (flags & 0x200000) {
                    t0 = 1;
                } else if (flags & 0x100000) {
                    t0 = 2;
                }
            }
            func_800537D0(o, a1, a2, &v18.x, t0);
            gGteCrossProduct(s6, &v18.x, s5);
            gGteCrossProduct(s5, s6, &v18.x);
            if (!(*(u8 *)(o + 0xB9) & 1)) {
                s32 s0v;
                s32 vx, vz;
                vx = *(s32 *)(s + 0x168);
                s0v = *(s32 *)(s + 0x16C);
                vz = *(s32 *)(s + 0x170);
                v50[0] = vx;
                v50[1] = vz;
                a1 = func_8001A7AC(v50);
                if (a1 < 0x4000) {
                    a1 = 0;
                } else {
                    a1 -= 0x4000;
                    a1 = FM(a1, FM(a1, a1));
                    a1 <<= 5;
                }
                func_8001ABE0(a1, v50);
                {
                    vx = v50[0];
                    vz = v50[1];
                    {
                        s32 t;
                        if (s0v >= 0) {
                            t = (s0v > 0x10000) ? 0x10000 : s0v;
                        } else {
                            t = 0;
                        }
                        s0v = t;
                    }
                    if (s0v > 0xD999) {
                        s0v = 0x10000;
                    } else {
                        s0v = s0v + 0x2666;
                    }
                    a1 = FM(0x9800, s0v);
                    v18.x += FM(vx, a1);
                    v18.y += FM(s0v, a1);
                    v18.z += FM(vz, a1);
                }
                v18.y -= FM(*(s32 *)(s + 0x160), scale);
            }
            v18.x -= FM(*(s32 *)(s + 0x15C), scale);
            v18.z -= FM(*(s32 *)(s + 0x164), scale);
            v18.y += -0x9800;
            res = func_80052550(o, &v18.x);
            *(s32 *)(s + 0x15C) += v18.x;
            *(s32 *)(s + 0x160) += v18.y;
            *(s32 *)(s + 0x164) += v18.z;
            if (*(s32 *)(s + 0x160) < -0xC0000) {
                *(s32 *)(s + 0x160) = -0xC0000;
            }
            v58[0] = *(s32 *)(s + 0x15C);
            v58[1] = *(s32 *)(s + 0x164);
            if (func_8001A7AC(v58) > 0x92147) {
                func_8001ABE0(0x92147, v58);
                *(s32 *)(s + 0x15C) = v58[0];
                *(s32 *)(s + 0x164) = v58[1];
            }
            if (res == 0 && fp != 0) {
                s32 p, q, r;
                p = ABS(*(s32 *)(s + 0x15C));
                q = ABS(*(s32 *)(s + 0x160));
                r = q - p;
                p = q - ((r >> 31) & r);
                q = ABS(*(s32 *)(s + 0x164));
                r = q - p;
                p = q - ((r >> 31) & r);
                if (p < 0x2000) {
                    *(s32 *)(s + 0x164) = 0;
                    *(s32 *)(s + 0x160) = 0;
                    *(s32 *)(s + 0x15C) = 0;
                }
            }
            v38 = *(Vec3 *)(o + 0x50);
            *(s32 *)(o + 0x50) += *(s32 *)(s + 0x15C) >> 8;
            *(s32 *)(o + 0x54) += *(s32 *)(s + 0x160) >> 8;
            *(s32 *)(o + 0x58) += *(s32 *)(s + 0x164) >> 8;
            func_8006943C((s32 *)(o + 0x50));
            v28.x = *(s32 *)(o + 0x50);
            v28.y = *(s32 *)(o + 0x54) + 0xCCC;
            v28.z = *(s32 *)(o + 0x58);
            {
                s32 s0v;
                s0v = func_80066F78(s5, (s32 *)(s + 0x168), &v60, &v64) + *(s16 *)(o + 0xF2);
                func_80053514(o, v64);
                *(u8 *)(*(u8 **)(o + 0xAC) + 0x147) = 0;
                *(u8 *)(*(u8 **)(o + 0xAC) + 0x146) = 0;
                v60 &= ~0xC0;
                if (v60 != 0) {
                    func_80053188(o, v60);
                }
                if (*(s32 *)(o + 0x54) < s0v) {
                    if (*(u8 *)(o + 0xB9) & 1) {
                        *(u8 *)(o + 0xB9) = (*(u8 *)(o + 0xB9) & 0xFE) | 2;
                        vv = *(s32 *)(s + 0x160);
                        a1 = -vv;
                        if (vv < 0) {
                            *(s32 *)(s + 0x160) = 0;
                        }
                        if (a1 > 0x50000) {
                            *(u8 *)(o + 0xBB) = 0x12;
                            ret += func_80052F54(o);
                        }
                    }
                    *(s32 *)(o + 0x54) = s0v;
                } else if (*(s32 *)(o + 0x54) - s0v < 0xCC) {
                    *(s32 *)(o + 0x54) = s0v;
                } else if ((u32)(*(u8 *)(o + 0xBB) - 1) < 2) {
                    *(u8 *)(o + 0xBB) = 0;
                    v28.x = v38.x;
                    v28.y = v38.y + 0xCCC;
                    v28.z = v38.z;
                    vv = func_80066F78(s5, 0, &v68, &v6C);
                    if (*(s32 *)(o + 0x54) < vv || *(s32 *)(o + 0x54) - s0v < 0x333) {
                        *(Vec3 *)(o + 0x50) = v38;
                        v68 &= ~0xC0;
                        if (v68 != 0) {
                            func_80053188(o, v68);
                        }
                        *(s32 *)(s + 0x15C) = 0;
                        *(s32 *)(s + 0x164) = 0;
                    } else {
                        *(u8 *)(o + 0xB9) |= 1;
                    }
                } else {
                    *(u8 *)(o + 0xB9) |= 1;
                }
            }
            if (!(*(u8 *)(o + 0xB9) & 1)) {
                s32 *p = (s32 *)(s + 0x15C);
                func_8001AD20(s6, p, s5);
                func_8001AD20(s5, s6, p);
            }
            func_80052708(o, func_8005220C(o));
        }
    }
    *(Vec3 *)(o + 0x78) = *(Vec3 *)(s + 0x15C);
    vv = func_8001A314((s32 *)(o + 0x78)) >> 11;
    *(s32 *)(o + 0x84) = vv;
    if (vv == 0) {
        *(Vec3 *)(o + 0x78) = *(Vec3 *)(o + 0xA0);
    }
    return ret;
}
