// Categories: Physics > Motion | Gameplay > AI > Target Selection | Gameplay > Entity Update
#include "game/types.h"

extern s32 func_8001AB00(s32 *a, s32 *b);
extern s32 func_8001A100(s32 *vector);

#define MULU16(out, a, b) \
    __asm__ volatile("multu\t%1,%2\n\tmflo\t$8\n\tmfhi\t$9\n\tsll\t$9,$9,16\n\tsrl\t$8,$8,16\n\tor\t%0,$9,$8" \
                     : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")

typedef struct SubObject {
    s32 unk00;      // 0x00 (0xDC)
    u8 pad04[0x18]; // 0x04 to 0x1C
    s32 unk1C;      // 0x1C (0xF8)
    s16 unk20;      // 0x20 (0xFC)
    u8 pad22[0x0E]; // 0x22 to 0x30
    s16 unk30;      // 0x30 (0x10C)
    s16 unk32;      // 0x32 (0x10E)
    u8 pad34[0x08]; // 0x34 to 0x3C
    u32 unk3C;      // 0x3C (0x118)
    s16 unk40;      // 0x40 (0x11C)
    u8 pad42[0x02]; // 0x42 to 0x44
    s16 unk44;      // 0x44 (0x120)
    s16 unk46;      // 0x46 (0x122)
    s16 unk48;      // 0x48 (0x124)
} SubObject;

typedef struct Object {
    u8 pad00[0x16];
    u8 unk16;              // 0x16
    u8 pad17[0x0D];        // 0x17 to 0x24
    s32 unk24;             // 0x24
    u8 pad28[0x34];        // 0x28 to 0x5C
    s32 x;                 // 0x5C
    s32 y;                 // 0x60
    s32 z;                 // 0x64
    u8 pad68[0x04];        // 0x68 to 0x6C
    s16 unk6C;             // 0x6C
    u8 pad6E[0x0A];        // 0x6E to 0x78
    s32 unk78[3];          // 0x78
    s32 unk84;             // 0x84
    struct Object *target; // 0x88
    u8 pad8C[0x04];        // 0x8C to 0x90
    s16 unk90;             // 0x90
    u8 pad92[0x4A];        // 0x92 to 0xDC
    SubObject subDC;       // 0xDC
} Object;

/**
 * @brief Approach step of an entity that tracks a target: gates on vertical
 *        distance and heading, then publishes a target speed and a 300-tick hold
 *        into its motion block.
 *
 * @param a0 Message id of the caller. The body never reads it; the type-26
 *        handler and the class-5 handler both forward their own message.
 * @param self Entity: +0x5C/+0x60/+0x64 position, +0x6C height bias, +0x16 type,
 *        +0x78/+0x7C/+0x80 heading vector, +0x84 speed, +0x88 target pointer,
 *        +0x90 s16 proximity gate, +0xDC the motion block.
 * @param unused_arg2 Partner argument; not read.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * flags = self->+0xDC+0x3C. The function returns immediately when
 * self->unk90 >= 0x4000, when flags & 0x48 is set, or when flags >> 30 == 1.
 * diff = target position - self position, and the vertical term is folded to an
 * absolute value and reduced by (target->unk6C - self->unk6C) << 4; a remainder
 * of 0x1401 or more returns, so the two entities must share a narrow vertical
 * band.
 * s0 = func_8001AB00(self->unk78, diff), the 16.16 dot product of the heading
 * with the offset, and a2 = func_8001A100(diff), its length. While the length is
 * at least 16 the dot product is rescaled by 0x80000000 / (length >> 1) with
 * MULU16 in all four sign combinations, i.e. it becomes a cosine-scaled value
 * where 0x10000 means "dead ahead"; a shorter distance short-circuits to
 * 0x10000.
 * Gates after that: with s0 <= 0xF600 the function returns when self->unk90 is
 * at least 0x2000, and always when s0 <= 0x8000 (more than 60 degrees off).
 * When target->unk24 is negative the two motion blocks are matched through
 * their +0x44/+0x46/+0x48 link words and the (unk20 << 8) - unk1C pair: one
 * branch sets self's +0x40 to 1, another returns without a change, and the
 * latch branch sets +0x32 = target->unk84 >> 1 (capped by +0x30), +0x40 = 300
 * and forces a positive target +0x40 to 1.
 * Otherwise +0x32 = (self->unk84 >> 1) + (+0x30 >> 2); when s0 > 0xF5FF and the
 * distance is below 0x4000 the speed drops to +0x30 >> 2 for type 26 and to 0
 * for every other type. All paths that reach the end set +0x40 = 300.
 *
 * @par Likely role
 * The approach-and-hold decision of chase-capable objects. The block at +0xDC
 * matches the mover layout func_8005027C documents word for word (+0x30 base
 * speed, +0x32 target speed, +0x40 hold timer, +0x3C flags), so this function
 * chooses the speed the integrator then ramps toward: fast while the target is
 * ahead but far, slow or zero when it is close, and a 300-tick hold plus a
 * speed taken from the target when the target reports itself as active at
 * +0x24. The +0x44/+0x46/+0x48 triple looks like a "who is holding whom" link,
 * which would make this a harness/tow/pin behaviour.
 *
 * @par Unknown
 * What the +0x44/+0x46/+0x48 link triple names, whether +0x24 is a movement or
 * ownership flag, the units of the 0x1400/0x4000/0x1401 bands, and the meaning
 * of the +0x6C height bias that is subtracted from the vertical distance.
 *
 * @par Callers
 * func_8002C3F0 (case 3, the type-26 tick/draw case) and func_800382B4 (case 8,
 * the class-5 animated-piece handler); both call sites are direct.
 */
void func_80037B28(s32 a0, Object *self, s32 unused_arg2) {
    s32 diff[3];
    Object *target;
    register SubObject *sub2 __asm__("$5");
    register SubObject *sub1 __asm__("$7");
    register s32 a2 __asm__("$6");
    register s32 a1 __asm__("$5");
    register s32 v1 __asm__("$3");
    register s32 v0 __asm__("$2");
    u32 flags;
    s32 s0;

    flags = self->subDC.unk3C;
    if (self->unk90 >= 0x4000) {
        return;
    }
    if (flags & 0x48) {
        return;
    }
    if ((flags >> 30) == 1) {
        return;
    }

    target = self->target;
    diff[0] = target->x - self->x;
    v1 = target->y;
    v1 -= self->y;
    a1 = v1 >> 31;
    diff[1] = v1;
    diff[2] = target->z - self->z;
    v1 = a1 + v1;

    a2 = v1 ^ a1;
    a2 -= (target->unk6C - self->unk6C) << 4;
    v0 = a2 < 0x1401;
    if (!v0) {
        return;
    }

    s0 = func_8001AB00(self->unk78, diff);
    a2 = func_8001A100(diff);

    if (a2 >= 16) {
        if (s0 > 0) {
            if (a2 > 0) {
                v1 = a2 >> 1;
                MULU16(v0, s0, 0x80000000u / (u32)v1);
                goto set_s0;
            }
            v1 = -a2;
            v1 >>= 1;
            MULU16(v0, s0, 0x80000000u / (u32)v1);
            v0 = -v0;
            goto set_s0;
        }
        a0 = -s0;
        if (a2 > 0) {
            v1 = a2 >> 1;
            MULU16(v0, a0, 0x80000000u / (u32)v1);
            v0 = -v0;
            goto set_s0;
        }
        v1 = -a2;
        v1 >>= 1;
        MULU16(v0, a0, 0x80000000u / (u32)v1);
set_s0:
        s0 = v0;
    } else {
        s0 = 0x10000;
    }

    sub2 = &self->subDC;

    if (s0 <= 0xF600) {
        if (self->unk90 >= 0x2000) {
            return;
        }
        if (s0 <= 0x8000) {
            return;
        }
    }

    sub1 = &target->subDC;
    if (target->unk24 < 0) {
        if (target->subDC.unk00 != self->subDC.unk00) {
            goto match_DE0;
        }
        if (sub1->unk44 != sub2->unk44) {
            goto label_DC0;
        }
        if ((sub1->unk20 << 8) - sub1->unk1C < (sub2->unk20 << 8) - sub2->unk1C) {
            goto match_DE0;
        }
        return;

label_DC0:
        if (sub2->unk44 == sub1->unk48) {
            goto match_DE0;
        }
        if (sub2->unk46 != sub1->unk44) {
            sub2->unk40 = 1;
            return;
        }
match_DE0:
        sub2->unk32 = target->unk84 >> 1;
        if (sub2->unk30 < (s16)sub2->unk32) {
            sub2->unk32 = (u16)sub2->unk30;
        }
        sub2->unk40 = 300;
        if (sub1->unk40 > 0) {
            sub1->unk40 = 1;
        }
        return;
    }

    if (s0 > 0xF5FF) {
        v0 = a2 < 0x4000;
        if (v0) {
            goto label_E64;
        }
    }
    sub2->unk32 = (self->unk84 >> 1) + ((s16)sub2->unk30 >> 2);
    goto set_40;

label_E64:
    if (self->unk16 == 26) {
        sub2->unk32 = (s16)sub2->unk30 >> 2;
    } else {
        sub2->unk32 = 0;
    }
set_40:
    sub2->unk40 = 300;
}
