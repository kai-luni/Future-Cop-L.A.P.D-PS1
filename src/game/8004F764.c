// Categories: Gameplay > Path Movement | Physics > Motion | Utilities > Mathematics > Trigonometry
#include "game/types.h"

struct CellF764 {
    u16 pad0, pad2, pad4;
    u16 x, y, z;
};
struct StateF764 {
    u8 *map;
    u16 x, y, z;
    u16 dx, dy, dz;
    u16 tx, ty, tz;
    s16 vx, vy, vz;
    s32 progress;
    u16 speed, distance;
    s16 step;
    u16 pad26;
    s16 yawDelta, inverseStep, pitchDelta;
    u16 pad2E, turnSpeed;
    u8 pad32[0x3C - 0x32];
    u32 flags;
    u16 pad40, pad42;
    s16 index, nextIndex, oldIndex, pad4A;
    u16 yaw, oldYaw, targetYaw, pitch, oldPitch, targetPitch;
    u16 lean, pad5A;
    u8 pad5C[0x64 - 0x5C];
    s32 sineYaw, sinePitch, cosineYaw;
    s32 vector[3];
};
struct ObjectF764 {
    u8 pad00[0x84];
    s32 turn;
};
extern s32 gSelectNextPathNode(struct StateF764 *, struct ObjectF764 *, s32) __asm__("func_8004F330");
extern s32 gVectorLength3D(s32 *) __asm__("func_8001A100");
extern void gVectorScaleInPlace(s32, s32 *) __asm__("func_8001AB78");
extern s32 gAtan2(s32, s32) __asm__("func_80095C30");
extern void gSinCos(u32, s32 *, s32 *) __asm__("func_8001846C");

#define MULU16(out, a, b) \
    __asm__("multu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")
#define MUL16(out, a, b) \
    __asm__("mult\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

#define RATIO(out, a, b) do { \
    s32 divisor; \
    s32 half; \
    s32 product; \
    if ((a) > 0) { \
        divisor = (b); \
        if (divisor > 0) { \
            half = divisor >> 1; \
            MULU16(out, a, 0x80000000U / (u32)half); \
        } else { \
            half = -divisor; \
            half >>= 1; \
            MULU16(product, a, 0x80000000U / (u32)half); \
            (out) = -product; \
        } \
    } else { \
        divisor = (b); \
        (a) = -(a); \
        if (divisor > 0) { \
            half = divisor >> 1; \
            MULU16(product, a, 0x80000000U / (u32)half); \
            (out) = -product; \
        } else { \
            half = -divisor; \
            half >>= 1; \
            MULU16(out, a, 0x80000000U / (u32)half); \
        } \
    } \
} while (0)
void gUpdatePathMoverSegment(struct StateF764 *state, struct ObjectF764 *obj, u8 update) __asm__("func_8004F764");


/**
 * @brief Segment hand-off of the rail mover: selects the next path node, snaps or
 *        turns the heading toward it, and produces the step clamp, the bank angle
 *        and the per-frame velocity.
 *
 * @param state Path-follower state block, 16-bit fixed point with eight
 *        fractional bits. Offsets: map pointer +0x00, position +0x04/+0x06/+0x08,
 *        current velocity +0x0A/+0x0C/+0x0E, direction to the next node
 *        +0x10/+0x12/+0x14, velocity written here +0x16/+0x18/+0x1A, progress
 *        inside the segment +0x1C, current segment length +0x20, length of the
 *        segment just measured +0x22, step clamp +0x24, head offset +0x26, yaw
 *        delta +0x28, reciprocal step +0x2A, pitch delta +0x2C, rate word +0x30,
 *        flag word +0x3C, scene indices +0x44/+0x46/+0x48, yaw +0x4C, old yaw
 *        +0x4E, target yaw +0x50, pitch +0x52, old pitch +0x54, target pitch
 *        +0x56, bank +0x58, sine/cosine cache +0x64..+0x6C and the scratch
 *        vector +0x70.
 * @param obj Render object; only the signed turn amount at +0x84 is read, as the
 *        gate of the bank calculation.
 * @param update 0 commits the newly selected segment and snaps the orientation to
 *        it; 1 computes the incremental turn, the step and the velocity.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8004F330(state, obj, state->index) returns the next node index; a negative
 * value returns immediately. The node sits at map + 4 + index * 12 and its three
 * u16 coordinates at +6, +8 and +0xA. The direction stored in +0x10/+0x12/+0x14
 * is that coordinate minus (position + velocity).
 * The direction is copied to the vector at +0x70 scaled by 256, its length comes
 * from func_8001A100, length >> 8 is stored at +0x22, and
 * func_8001AB78(0x80000000 / (length >> 1), vector) normalises the copy to 16.16
 * in place.
 * Yaw: target yaw = func_80095C30(x, z) & 0xFFF, recomputed when the node index
 * changed or flag 0x400 is clear; before that, old yaw takes the previous target
 * value when the two differ and flag 0x1000 is clear. Pitch: the horizontal
 * distance is approximated by abs, swap and shift-add expressions, then target
 * pitch = (0x1000 - func_80095C30(y, horizontal)) & 0xFFF.
 * The flag word loses bits 28-29 (mask 0xCFFFFFFF) and, with flag 0x20 set, the
 * bank and its spare half-word are cleared.
 * update == 0: pitch and yaw snap to their targets, each suppressed by flags
 * 0x100 and 0x1000, and func_8001846C refreshes the cache at +0x68 (pitch sine)
 * and +0x64/+0x6C (yaw sine and cosine); the function then returns.
 * update == 1: flag 0x800 returns at once, and so does an unchanged node index
 * with flag 0x400 after both deltas have been zeroed. Otherwise each delta is
 * (target - current) & 0xFFF, zeroed when they are equal and otherwise written as
 * the wrapped shortest difference (values above 0x800 lose 0x1000); when both
 * deltas are zero the function returns. The step at +0x24 is
 * min(+0x20 / 2, +0x20 - (progress >> 8), +0x22 / 2) clamped at 0, and +0x2A
 * becomes 0x4000 / step, or 0x7FFF for a zero step.
 * Bank: only with flag 0x20 and a non-zero yaw delta. The delta is clamped to
 * +-0x800 and scaled by -768/2048, then multiplied by the ratio
 * obj->+0x84 * 65536 / +0x30 clamped to 1.0, and the product is stored at +0x58.
 * Velocity: +0x16/+0x18/+0x1A become ((+0x10 << 8) * (step / +0x22) -
 * (+0x0A << 8) * (step / +0x20)) >> 10 per axis, and the flag word ends as
 * (flags & 0xCFFFFFFF) | 0x10000000.
 *
 * @par Likely role
 * The hand-off half of the spline follower func_8005027C: that function calls it
 * with 0 at a segment transition and with 1 whenever no next node has been chosen
 * yet, so 0 is the "commit the new segment" path and 1 the "steer into the turn"
 * path. The step never exceeds half of either adjoining segment, which is the
 * usual corner slowdown, and +0x2A converts it into a reciprocal so the turn can
 * be interpolated per frame. The bank at +0x58 is body roll into the turn, scaled
 * inversely by the rate word so that slow movers lean further.
 *
 * @par Unknown
 * - Whether the rate at +0x30 is a speed (the reading func_8005027C gives it) or a
 *   turn rate (the reading the bank ratio gives it); the same word serves both.
 * - Why the node table looks 12 bytes wide here while func_8005027C steps it from
 *   map + 0 and reads occupancy nibbles at entry +4 and +8, which only lines up if
 *   a record really spans 16 bytes.
 * - The units of +0x24 and why the pitch is stored as 0x1000 minus the angle.
 *
 * @par Callers
 * func_8005027C, two sites: the segment transition (update 0) and the branch taken
 * while no next node is known (update 1).
 */
void gUpdatePathMoverSegment(struct StateF764 *state, struct ObjectF764 *obj, u8 update)
{
    register struct StateF764 *s __asm__("$17");
    u8 *map;
    struct CellF764 *cell;
    register s32 *vec __asm__("$16");
    s32 length;
    register s32 a0 __asm__("$4");
    register s32 a1 __asm__("$5");
    register s32 a2 __asm__("$6");
    register s32 a3 __asm__("$7");
    register s32 v0 __asm__("$2");
    register s32 v1 __asm__("$3");
    s32 delta;
    s32 bound;
    s32 product;
    s32 localSine;

    s = state;
    map = s->map;
    s->nextIndex = gSelectNextPathNode(s, obj, s->index);
    vec = s->vector;
    if (s->nextIndex < 0) {
        return;
    }
    cell = (struct CellF764 *)(map + 4) + s->nextIndex;
    s->tx = cell->x - (s->x + s->dx);
    s->ty = cell->y - (s->y + s->dy);
    s->tz = cell->z - (s->z + s->dz);
    s->vector[0] = (s16)s->tx << 8;
    s->vector[1] = (s16)s->ty << 8;
    s->vector[2] = (s16)s->tz << 8;
    length = gVectorLength3D(vec);
    s->distance = length >> 8;
    gVectorScaleInPlace(0x80000000U / (u32)(length >> 1), vec);

    if (s->nextIndex != s->oldIndex || !(s->flags & 0x400)) {
        if ((s16)s->oldYaw != (s16)s->targetYaw && !(s->flags & 0x1000)) {
            s->oldYaw = s->targetYaw;
        }
        s->targetYaw = gAtan2(s->vector[0], s->vector[2]) & 0xFFF;
    }
    a3 = s->vector[0];
    if (a3 < 0) {
        a3 = -a3;
    }
    a2 = s->vector[2];
    if (a2 < 0) {
        a2 = -a2;
    }
    if (a3 < a2) {
        a3 ^= a2;
        a2 ^= a3;
        a3 ^= a2;
    }
    if ((a3 >> 1) < a2) {
        a0 = a3 + a2;
        v0 = a0 << 6;
        v1 = a0 << 3;
        v0 += v1;
        v1 = a0 << 1;
        v0 += v1;
        v0 += a3;
        v1 = a2 << 5;
        a0 = v0 + v1;
        a2 = a3 - a2;
        a3 = a0 >> 7;
    }
    a1 = a2 >> 2;
    a1 = a3 + a1;
    v0 = a2 >> 6;
    a1 -= v0;
    v0 = gAtan2(s->vector[1], a1);
    a0 = 0xCFFFFFFF;
    v0 = (v0 << 12) >> 12;
    s->targetPitch = (0x1000 - v0) & 0xFFF;
    s->flags &= a0;
    if (s->flags & 0x20) {
        s->lean = s->pad5A = 0;
    }
    if (!update) {
        if (!(s->flags & 0x100)) {
            v0 = s->targetPitch;
            s->pitch = v0;
            s->oldPitch = v0;
            gSinCos((s16)v0, &localSine, &s->sinePitch);
        }
        if (!(s->flags & 0x1000)) {
            v0 = s->targetYaw;
            s->yaw = v0;
            s->oldYaw = v0;
            gSinCos((s16)v0, &s->sineYaw, &s->cosineYaw);
        }
        return;
    }
    if (s->flags & 0x800) {
        return;
    }
    if (s->oldIndex == s->nextIndex && (s->flags & 0x400)) {
        s->yawDelta = 0;
        s->pitchDelta = 0;
        return;
    }
    v0 = (s16)s->targetYaw;
    v1 = (s16)s->yaw;
    a2 = (v0 - v1) & 0xFFF;
    if (a2 == 0) {
        s->yawDelta = 0;
    }
    v1 = 0;
    if (!(s->flags & 0x100)) {
        v0 = (s16)s->targetPitch;
        v1 = (s16)s->pitch;
        v0 = v0 - v1;
        v1 = v0 & 0xFFF;
    }
    if (v1 == 0) {
        s->pitchDelta = 0;
    }
    if (a2 == 0 && v1 == 0) {
        return;
    }
    if (v1 != 0) {
        if (v1 > 0x800) {
            v1 -= 0x1000;
        }
        s->pitchDelta = v1;
    }
    v0 = s->speed << 16;
    a0 = v0 >> 16;
    v0 = (u32)v0 >> 31;
    v0 = a0 + v0;
    __asm__("" : "=r"(v0) : "0"(v0));
    a1 = v0 >> 1;
    v1 = a0 - (s->progress >> 8);
    if (v1 < a1) {
        a1 = v1;
    }
    v1 = (s16)s->distance / 2;
    if (v1 < a1) {
        a1 = v1;
    }
    if (a1 < 0) {
        a1 = 0;
    }
    s->step = a1;
    if (a1 > 0) {
        v1 = (a1 << 8) >> 1;
        v0 = (0x80000000U / (u32)v1) >> 10;
    } else {
        v0 = 0x7FFF;
    }
    s->inverseStep = v0;
    if (a2 != 0) {
        if (a2 > 0x800) {
            a2 -= 0x1000;
        }
        s->yawDelta = a2;
        if (s->flags & 0x20) {
            v0 = (s16)s->targetYaw;
            v1 = (s16)s->yaw;
            a0 = v0 - v1;
            if (a0 > 0x800) {
                a0 -= 0x1000;
            } else if (a0 < -0x800) {
                a0 += 0x1000;
            } else {
                goto clamp_upper;
            }
            if (a0 < -0x800) {
                v1 = -0x800;
            } else {
clamp_upper:
                v1 = 0x800;
                if (a0 <= 0x800) {
                    v1 = a0;
                }
            }
            v0 = -(v1 * 768);
            a1 = v0 / 2048;
            a0 = obj->turn;
            if (a0 > 0) {
                v0 = s->turnSpeed << 16;
                v1 = v0 >> 16;
                if (v1 > 0) {
                    v1 = v0 >> 17;
                    goto turn_positive;
                }
                v1 = -v1;
                v1 >>= 1;
                goto turn_negative;
            }
            v0 = s->turnSpeed << 16;
            v1 = v0 >> 16;
            if (v1 <= 0) {
                goto turn_both_negative;
            }
            a0 = -a0;
            v1 = v0 >> 17;
turn_negative:
            v0 = 0x80000000U / (u32)v1;
            MULU16(v0, a0, v0);
            a0 = -v0;
            goto turn_done;
turn_both_negative:
            a0 = -a0;
            v1 = -v1;
            v1 >>= 1;
turn_positive:
            v0 = 0x80000000U / (u32)v1;
            MULU16(a0, a0, v0);
turn_done:
            if (a0 > 0x10000) {
                a0 = 0x10000;
            }
            v0 = a1 << 16;
            MUL16(v0, v0, a0);
            a1 = v0 >> 16;
            s->lean = a1;
        }
    }
    v0 = s->step;
    a0 = v0 << 8;
    if (a0 > 0) {
        v0 = (s16)s->speed << 8;
        if (v0 > 0) {
            v1 = v0 >> 1;
            goto speed_positive;
        }
        v1 = -v0;
        v1 >>= 1;
        goto speed_negative;
    }
    v0 = (s16)s->speed << 8;
    if (v0 <= 0) {
        goto speed_both_negative;
    }
    v1 = v0 >> 1;
    a0 = -a0;
speed_negative:
    v0 = 0x80000000U / (u32)v1;
    MULU16(v0, a0, v0);
    a3 = -v0;
    goto speed_done;
speed_both_negative:
    a0 = -a0;
    v1 = -v0;
    v1 >>= 1;
speed_positive:
    v0 = 0x80000000U / (u32)v1;
    MULU16(a3, a0, v0);
speed_done:;
    v0 = s->step;
    a0 = v0 << 8;
    if (a0 > 0) {
        v0 = (s16)s->distance << 8;
        if (v0 > 0) {
            v1 = v0 >> 1;
            goto distance_positive;
        }
        v1 = -v0;
        v1 >>= 1;
        goto distance_negative;
    }
    v0 = (s16)s->distance << 8;
    if (v0 <= 0) {
        goto distance_both_negative;
    }
    v1 = v0 >> 1;
    a0 = -a0;
distance_negative:
    v0 = 0x80000000U / (u32)v1;
    MULU16(v0, a0, v0);
    a2 = -v0;
    goto distance_done;
distance_both_negative:
    a0 = -a0;
    v1 = -v0;
    v1 >>= 1;
distance_positive:
    v0 = 0x80000000U / (u32)v1;
    MULU16(a2, a0, v0);
distance_done:;

    a1 = 0xCFFFFFFF;
    MUL16(v1, (s16)s->tx << 8, a2);
    MUL16(v0, (s16)s->dx << 8, a3);
    v1 -= v0;
    v1 >>= 10;
    s->vx = v1;
    MUL16(v1, (s16)s->ty << 8, a2);
    MUL16(v0, (s16)s->dy << 8, a3);
    v1 -= v0;
    v1 >>= 10;
    s->vy = v1;
    MUL16(v1, (s16)s->tz << 8, a2);
    MUL16(v0, (s16)s->dz << 8, a3);
    v1 -= v0;
    v1 >>= 10;
    s->vz = v1;
    s->flags = (s->flags & a1) | 0x10000000;
}
