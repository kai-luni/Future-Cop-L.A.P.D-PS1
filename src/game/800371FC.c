// Categories: Gameplay > Entity Update > Mover | Physics > Motion > Gravity | Gameplay > Collision > Area Blast
#include "game/types.h"

#define ABS(x, r) ((((x) >> 31) + (x)) ^ ((x) >> 31))

struct Vec371 { s32 x, y, z; };
struct Unk4E80C {
    u8 pad00[8];
    s32 unk08;
    u16 unk0C;
    u8 pad0E[6];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 unk19;
    u8 pad1A[0xA];
    u32 unk24;
    u8 pad28[0x34];
    s32 unk5C;
    s32 unk60;
    s32 unk64;
    u8 pad68[6];
    s16 unk6E;
};
struct Unk4E80CList {
    s32 count;
    s32 pad04;
    struct Unk4E80C *items[1];
};
struct Unk371FC {
    u8 pad00[0x1C];
    s16 unk1C;
    u8 pad1E[0x0E];
    s32 unk2C;
    u8 pad30[0x20];
    struct Vec371 pos;
    u8 pad5C[0x0C];
    s32 unk68;
    u8 pad6C[0x0C];
    struct Vec371 direction;
    s32 unk84;
    u8 pad88[0x18];
    struct Vec371 velocity;
    struct Vec371 old;
    s32 unkB8;
    u8 padBC[8];
    s32 unkC4;
    struct Unk4E80C *unkC8;
    s32 unkCC;
    u8 unkD0;
    u8 unkD1;
    s16 unkD2;
};

extern struct Unk4E80CList gCommandListHead __asm__("D_800A8F90");
extern void func_8003BC0C(struct Unk371FC *object);
extern void gBuildRotationMatrix(s32 a0, s32 a1, s32 a2, s32 *a3) __asm__("func_800188D0");
extern s32 gGetGroundHeight(struct Vec371 *pos, s32 a1, s32 a2, s32 a3) __asm__("func_80066F78");
extern s32 gTraceMapRay(struct Vec371 *a, struct Vec371 *b, struct Vec371 *out, s32 d, s32 e) __asm__("func_800684AC");
extern void gVectorAddScaled(struct Vec371 *a, struct Vec371 *b, s32 scale, struct Vec371 *out) __asm__("func_8001ACA4");
extern s32 gVectorLength3D(struct Vec371 *vector) __asm__("func_8001A100");
extern void gNormalizeVector(struct Vec371 *vector) __asm__("func_8001A180");

/**
 * @brief Per-frame mover tick of a propelled entity: proximity blast test
 *        against the entity table, spin, gravity, ground snap, segment
 *        collision, speed derivation and a lifetime countdown.
 *
 * @param arg0 Entity block. Position +0x50, floor offset +0x68,
 *        direction +0x78, speed +0x84, velocity +0xA0, previous position
 *        +0xAC, gravity word +0xB8, expire marker +0x1C, lifetime +0xC4,
 *        cached owner +0xC8 with the id to match at +0xCC, control bits
 *        +0xD0, spin angle +0xD1, blast radius +0xD2, rotation matrix
 *        +0x2C.
 * @param arg1 Frame time step: multiplies the gravity word +0xB8, adds
 *        to the spin angle +0xD1, scales the position integrator and is
 *        subtracted from the lifetime +0xC4.
 * @return Nothing.
 *
 * @par Confirmed
 * Owner check: a cached pointer +0xC8 whose +0x08 no longer equals
 * +0xCC is dropped.
 * Blast test, only with a non-zero radius +0xD2: the entity table
 * D_800A8F90 is walked (count at +0x00, entries at +0x08). Candidates
 * are skipped when their +0x08 is <= 0, their +0x16 equals 0x0B, their
 * team byte +0x18 equals the own team (0xFF when there is no cached
 * owner) or their +0x24 bit 0 is set. The delta to a candidate
 * (+0x5C/+0x60/+0x64) is rejected if any component exceeds 0xFFFF,
 * then the octagonal sum of the squared components, each shifted 4,
 * squared and shifted back by 12, is compared with
 * (h * h) >> 12 plus ((r + 2*h) * r) >> 12, where h is +0xD2 and r is
 * the candidate's +0x6E, i.e. the two radii combined. A hit writes
 * +0xC4 = 0x12C, velocity.y = -0x8000, +0xD2 = 0, velocity.x = 0,
 * velocity.z = 0, +0xB8 = 0x8000, +0xD1 = 0, +0xD0 = (+0xD0 & 0xFE) |
 * 2 and calls func_8003BC0C(self), which turns the entity into its
 * impact record through the object spawn func_800402DC.
 * Motion, only while +0xD0 bit 0 is clear: with bit 1 set, +0xD1 += arg1
 * and func_800188D0(+0xD1 << 4, 0, 0, &+0x2C) rebuilds the matrix as a
 * rotation about x. func_80066F78(&position, 0, 0, 0) gives the ground
 * height; +0x68 added makes the floor level. At or below it the actor
 * lands: with all three velocity components below 0x1800 the velocity
 * is cleared, position.y is snapped and both +0x1C = -1 and +0xD0 bit 0
 * are written; otherwise position.y is snapped, velocity.y becomes
 * -(velocity.y * 5 / 8) and velocity.x/z are scaled by 7 / 8. Above the
 * floor, velocity.y loses +0xB8 * arg1 / 300 per step, clamped at 0
 * when the result would overshoot.
 * Then the previous position is copied to +0xAC, the position is
 * advanced with gVectorAddScaled(position, velocity, arg1 * 218,
 * position), and func_800684AC(+0xAC, position, out, 0, 0) & 0xFF is
 * the grid clip: any hit writes +0x1C = -1, stores the corrected
 * position and returns immediately.
 * Shared tail: direction = velocity, +0x84 =
 * func_8001A100(&direction) / 300, func_8001A180 normalises direction.
 * Finally +0xC4 -= arg1 and a non-positive lifetime sets +0x1C = -1.
 *
 * @par Likely role
 * The flight of a thrown or hovering prop such as an arcing debris piece
 * or a plasma bolt: it hangs +0x68 above the ground, spins while +0xD0
 * bit 1 is set, bounces with damping on impact with the floor, and once
 * it carries a radius +0xD2 it detonates on the first entity of another
 * team inside the combined radii, spawning the impact object. +0x1C =
 * -1 is the engine's "remove this entity" marker, so both the grid hit
 * and the lifetime expiry retire the entity.
 *
 * @par Unknown
 * The units of +0x68 and +0xC4, the meaning of +0xD0 bit 1 (a knockback
 * state rather than free spin), whether +0xB8 is gravity or a blast
 * impulse, and why the landing branch keeps the negated vertical
 * velocity instead of simply stopping.
 *
 * @par Callers
 * func_80037708, message 6 only (call site 0x80037780,
 * analysis/edges.json kind "direct"). The same handler loads the matrix
 * at +0x2C and transforms a model vertex right afterwards. func_80037708
 * itself has no direct callers; it is dispatched through a message
 * table.
 */
void func_800371FC(struct Unk371FC *arg0, s32 arg1)
{
    register struct Unk371FC *self __asm__("$18");
    register s32 step __asm__("$22");
    struct Unk4E80C *o;
    s32 i;
    s32 dx;
    s32 dy;
    s32 dz;
    struct Vec371 out;
    s32 vec[3];
    register s32 tmp __asm__("$5");
    s32 sgn;
    s32 scale;
    register s32 h __asm__("$2");
    struct Unk4E80C *owner;
    register s32 s4 __asm__("$20");
    register s32 s5 __asm__("$21");
    u8 s1;
    register struct Vec371 *pos __asm__("$17");
    register struct Vec371 *old __asm__("$16");

    self = arg0;
    step = arg1;
    if (self->unkC8 != 0 && self->unkC8->unk08 != self->unkCC) {
        self->unkC8 = 0;
    }
    h = self->unkD2;
    if (h != 0) {
        s1 = 0xFF;
        owner = self->unkC8;
        s4 = h << 1;
        s5 = (h * h) >> 12;
        if (owner != 0) {
            s1 = owner->unk18;
        }
        for (i = 0; i < gCommandListHead.count; i++) {
            register s32 shifted __asm__("$3");
            o = gCommandListHead.items[i];
            if (o->unk08 <= 0) continue;
            if (o->unk16 == 0xB) continue;
            if (o->unk18 == s1) continue;
            if (o->unk24 & 1) continue;
            vec[0] = dx = o->unk5C - self->pos.x;
            vec[1] = dy = o->unk60 - self->pos.y;
            vec[2] = dz = o->unk64 - self->pos.z;
            if (0xFFFF < ABS(dx, sgn)) continue;
            if (0xFFFF < ABS(dy, sgn)) continue;
            if (0xFFFF < ABS(dz, sgn)) continue;
            shifted = dx >> 4; dx = shifted * shifted;
            shifted = dy >> 4; dy = shifted * shifted;
            shifted = dz >> 4; dz = shifted * shifted;
            h = o->unk6E;
            if ((dx >> 12) + (dy >> 12) + (dz >> 12) <
                s5 + (((h + s4) * h) >> 12)) {
                self->unkC4 = 0x12C;
                self->velocity.y = -0x8000;
                self->unkD2 = 0;
                self->velocity.x = 0;
                self->velocity.z = 0;
                self->unkB8 = 0x8000;
                self->unkD1 = 0;
                self->unkD0 = (self->unkD0 & 0xFE) | 2;
                func_8003BC0C(self);
            }
        }
    }
    if (!(self->unkD0 & 1)) {
        register struct Vec371 *call0 __asm__("$4");
        register struct Vec371 *call1 __asm__("$5");
        register s32 call2 __asm__("$6");
        register struct Vec371 *call3 __asm__("$7");
        register s32 ground __asm__("$3");
        register s32 currentY __asm__("$4");
        if (self->unkD0 & 2) {
            self->unkD1 += step;
            gBuildRotationMatrix(self->unkD1 << 4, 0, 0, &self->unk2C);
        }
        h = gGetGroundHeight(&self->pos, 0, 0, 0);
        ground = self->unk68;
        currentY = self->pos.y;
        tmp = ground + h;
        h = tmp < currentY;
        if (!h) {
            register s32 damp __asm__("$3");
            if (ABS(self->velocity.y, sgn) < 0x1800 &&
                ABS(self->velocity.x, sgn) < 0x1800 &&
                ABS(self->velocity.z, sgn) < 0x1800) {
                self->velocity.x = 0;
                self->velocity.y = 0;
                self->velocity.z = 0;
                self->pos.y = tmp;
                self->unk1C = -1;
                self->unkD0 |= 1;
                goto UPDATE_SPEED;
            }
            self->pos.y = tmp;
            if (self->velocity.y != 0) { damp = self->velocity.y * 5 / 8; h = -damp; self->velocity.y = h; }
            if (self->velocity.x != 0) { damp = self->velocity.x * 7 / 8; self->velocity.x = damp; }
            if (self->velocity.z != 0) { damp = self->velocity.z * 7 / 8; self->velocity.z = damp; }
        } else {
            s32 a0 = self->velocity.y;
            s32 v1 = self->unkB8 * step / 300;
            if (a0 > 0 && a0 < v1) self->velocity.y = 0;
            else self->velocity.y = a0 - v1;
        }
        self->old = self->pos;
        pos = &self->pos;
        call0 = pos;
        call1 = &self->velocity;
        call2 = step * 218;
        call3 = pos;
        __asm__("" : "+r"(call0), "+r"(call1), "+r"(call2), "+r"(call3));
        gVectorAddScaled(call0, call1, call2, call3);
        old = &self->old;
        __asm__("" : "+r"(old));
        if (gTraceMapRay(old, pos, &out, 0, 0) & 0xFF) {
            self->unk1C = -1;
            self->pos = out;
            return;
        }
UPDATE_SPEED:
        self->direction = self->velocity;
        self->unk84 = gVectorLength3D(&self->direction) / 300;
        gNormalizeVector(&self->direction);
    }
    self->unkC4 -= step;
    if (self->unkC4 <= 0) self->unk1C = -1;
}
