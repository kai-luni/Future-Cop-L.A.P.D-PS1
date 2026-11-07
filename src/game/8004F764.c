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
extern s32 func_8004F330(struct StateF764 *, struct ObjectF764 *, s32);
extern s32 func_8001A100(s32 *);
extern void func_8001AB78(s32, s32 *);
extern s32 func_80095C30(s32, s32);
extern void func_8001846C(u32, s32 *, s32 *);

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

void func_8004F764(struct StateF764 *state, struct ObjectF764 *obj, u8 update)
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
    s->nextIndex = func_8004F330(s, obj, s->index);
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
    length = func_8001A100(vec);
    s->distance = length >> 8;
    func_8001AB78(0x80000000U / (u32)(length >> 1), vec);

    if (s->nextIndex != s->oldIndex || !(s->flags & 0x400)) {
        if ((s16)s->oldYaw != (s16)s->targetYaw && !(s->flags & 0x1000)) {
            s->oldYaw = s->targetYaw;
        }
        s->targetYaw = func_80095C30(s->vector[0], s->vector[2]) & 0xFFF;
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
    v0 = func_80095C30(s->vector[1], a1);
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
            func_8001846C((s16)v0, &localSine, &s->sinePitch);
        }
        if (!(s->flags & 0x1000)) {
            v0 = s->targetYaw;
            s->yaw = v0;
            s->oldYaw = v0;
            func_8001846C((s16)v0, &s->sineYaw, &s->cosineYaw);
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
