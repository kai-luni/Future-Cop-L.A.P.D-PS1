// Categories: Gameplay > Entity Update > Mover | Physics > Motion | Graphics > GTE > Matrix Build
#include "game/types.h"

struct Vec3_35B7C { s32 x, y, z; };

struct ModelFrame {
    u16 unk00;
    s16 unk02;
    s16 radius;
    u16 unk06;
    u8 unk08[8];
};

struct ModelFrames {
    u8 header[0x10];
    struct ModelFrame frames[1];
};

struct Model {
    u8 unk00[0x10];
    void *unk10;
    u8 unk14[0x20];
    struct ModelFrames *frames;
};

struct ModelInstance {
    struct Model *model;
    u8 coord[0x3C];
    s32 radius;
    u16 unk44;
    u16 unk46;
    s16 frame;
    u8 flags;
    s8 unk4B;
};

struct Obj35B7C {
    u8 pad00[0x8];
    void *unk08;
    u16 unk0C;
    u16 pad0E;
    u8 pad10[4];
    u16 unk14;
    u8 unk16;
    u8 pad17;
    u8 unk18;
    u8 unk19;
    s16 unk1A;
    s16 unk1C;
    u8 pad1E[2];
    u8 unk20;
    u8 pad21[3];
    s32 unk24;
    struct ModelInstance mi;
    u8 pad74[4];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
    u8 pad88[0x18];
    s16 unkA0;
    s16 unkA2;
    s16 unkA4;
    s16 unkA6;
    s32 unkA8;
    s32 unkAC;
    s32 unkB0;
    s32 unkB4;
    s32 unkB8;
    s32 unkBC;
    s32 unkC0;
    struct Obj35B7C *unkC4;
    struct Obj35B7C *unkC8;
    s32 unkCC;
    s16 unkD0;
    s16 unkD2;
    s16 unkD4;
    u16 unkD6;
    s16 unkD8;
    u16 unkDA;
    u8 unkDC;
    u8 unkDD;
    u8 padDE[2];
    s32 unkE0;
    u32 unkE4;
    u8 unkE8[4];
    void *unkEC;
    u8 padF0[4];
    s16 unkF4;
};

struct SpawnInfo35B7C {
    u16 unk00;
    s16 unk02;
    s16 unk04;
    u16 unk06;
    u16 unk08;
    s16 unk0A;
    u16 unk0C;
    u8 unk0E;
    u8 pad0F;
    u8 pad10;
    u8 pad11;
    u8 unk12;
};

extern void gSetModelFrame(struct ModelInstance *instance, s32 frame) __asm__("func_800175F8");
extern s32 func_8001A13C(s32 *a, s32 *b);
extern s32 func_8001A4E8(s32 *a, s32 *b);
extern void gNormalizeVector(s32 *vector) __asm__("func_8001A180");
extern s32 gNormalizeGetLength(s32 *vector) __asm__("func_8001A314");
extern s32 gEaseVectorQuarter(s32 *a, s32 *b) __asm__("func_8001A6F8");
extern s32 gVectorDot(s32 *a, s32 *b) __asm__("func_8001AB00");
extern void gVectorAddScaled(s32 *a, s32 *b, s32 delta, s32 *out) __asm__("func_8001ACA4");
extern s32 func_800684AC(s32 *a, s32 *b, s32 *out, s32 arg3, s32 arg4);
extern s32 gGetGroundHeight(s32 *a, s32 b, s32 c, s32 d) __asm__("func_80066F78");
extern s32 gSampleHeightGridBilinear(s32 *a) __asm__("func_80069A38");
extern void gBuildRotationMatrix(s32 a, s32 b, s32 angle, s32 *out) __asm__("func_800188D0");
extern void gBuildBasisZFromPair(s32 *first, s32 *second, s32 *basis) __asm__("func_80019310");
extern void gGteLoadRotation(s32 *basis) __asm__("func_80010820");
extern void gGteTransformWords(s32 *a, s32 *b, s32 c) __asm__("func_8001006C");
extern void gGteLoadMatrix(s32 *a) __asm__("func_80010750");
extern void gGteTransformVerticesWide(s32 *a, s32 *b, s32 c) __asm__("func_80010234");
extern void gGteCrossProduct(s32 *a, s32 *b, s32 *out) __asm__("func_80010974");
extern s32 func_80019FCC(s32 *vector);
extern s32 gSqrt12(s32 angle) __asm__("func_80095900");
extern void func_800410BC(s32 *pos, s32 speed, s32 angle, struct Obj35B7C *target);
extern s32 gEntityBaseHandler(s32 state, struct Obj35B7C *self, struct Obj35B7C *other, s32 arg3) __asm__("func_8002AD58");
extern void gProjectileInit(struct Obj35B7C *self, struct Obj35B7C *other, s32 arg3) __asm__("func_80035904");
extern s32 gSpawnTypeAtOwner(void *owner, u8 column, s32 arg2, s32 row, s32 arg4) __asm__("func_800402DC");
extern void gSpawnEntitySlotTypes(u8 arg0, struct Obj35B7C *self, s32 arg2, s32 arg3, s32 arg4) __asm__("func_8003B898");
extern void *func_8004E80C(struct Obj35B7C *target, u8 arg1, s16 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6);
extern void *gSpawnDirectShot(struct SpawnInfo35B7C *info, struct Model *model, s32 *vec80, s32 *vec70, struct Obj35B7C *unkC8, struct Obj35B7C *s1p, s32 unk24, s32 arg7, s32 arg8, s32 arg9, s32 arg10, u32 unkE4) __asm__("func_8005E47C");
extern void *func_8006C224(struct Obj35B7C *target, struct Obj35B7C *arg1, s32 arg2, s32 arg3);
extern void func_80044B34(struct Obj35B7C *self, void *unkE8, s32 delta, s32 arg3);

/**
 * @brief Per-frame update of a mobile entity: advances its animation, acquires
 *        or validates a leader/target, flies toward it with lead pursuit,
 *        resolves collisions, bursts child objects on arrival and finally
 *        rebuilds its own basis matrix and model vertex.
 *
 * @param self  The entity record (layout below). It is updated in place.
 * @param delta Frame time step. Used as a multiplier for speed/acceleration
 *              and as a countdown decrement, so it is a tick count, not a
 *              seconds value.
 *
 * @return Nothing.
 *
 * @par Globals (confirmed field use) (offsets into self)
 * +0x08 serial id (compared with a target's own +0x08 to detect reuse),
 * +0x16 type (0x12 unwraps a second object through +0xB4), +0x1A s16 type id,
 * +0x1C s16 "movement enabled" flag (negative disables the movement block),
 * +0x20 team/owner byte, +0x24 model/arg word, +0x2C 3x3 matrix (written at the
 * end); +0x44 is the third 3-word row of the matrix at +0x2C (matrix words
 * 6..8) and serves as the direction of the dot-product test; +0x50/+0x54/+0x58
 * position; +0x5C/+0x60/+0x64 the transformed model vertex, written at the end
 * by func_80010234 and read when a child's offset is built;
 * +0x78/+0x7C/+0x80 velocity, +0x84 speed, +0xA0/+0xA2/+0xA4
 * home point (shifted left by 8), +0xA6 leash radius (also shifted left by 8),
 * +0xA8 previous position, +0xB4/+0xB8/+0xBC heading vector, +0xC0 cached target
 * id, +0xC4 current target, +0xC8 leader, +0xCC cached leader id, +0xD0 s16,
 * +0xD2 s16 acceleration, +0xD4 s16 target speed, +0xD6 u16 spin angle,
 * +0xD8 s16 spawn argument, +0xDA u16 countdown, +0xDC state byte
 * (0x12 / 0x15 / 3), +0xDD spawn type (0xFF selects the alternate path),
 * +0xE0 state word (0..4), +0xE4 flag word, +0xE8 animation track,
 * +0xEC pointer whose byte 1 selects the animation rate mode, +0xF4 s16 frame.
 * Model instance block at +0x28: model +0x28, radius +0x68, frame +0x70,
 * unk4B +0x73.
 *
 * @par Control flow (confirmed)
 * 1. State 4 (launch): the entity asks for a spawn source - either
 *    func_8004E80C(target, unkDD, unkD8, 1, unkA6 << 8, 0xFFFF0000, 0) where
 *    target is unkC8 or self, or func_8006C224(target, unkC4, unkA6 << 8, -1)
 *    when unkDD == 0xFF. On success it stores unkDA = 0x5DC, unkE0 = 1,
 *    unkC4 = source, unkC0 = source->+0x08 and unk84 = unkD4; on failure it
 *    drops back to state 0.
 * 2. Animation: with a model and a non-null model->+0x10, the rate is delta for
 *    mode 1, unk84 * delta for mode 2 and 0 otherwise; func_80044B34 advances
 *    the track and func_800175F8 installs the new frame radius.
 * 3. A stale leader (unkCC != leader->+0x08) is dropped to null.
 * 4. s4 = func_8001A4E8(home << 8, pos): the octagonal distance between the
 *    home point and the current position.
 * 5. Burst: when unkDC == 0x12 and s4 >= 0x3001, a spawn info block is filled
 *    (unk06 = unkD4, unk08 = unkD2, unk00 = unk1A, unk0A = unkA6 << 4,
 *    unk12 = 0, unk0E = unk20, unk02 = unkD0 >> 4) and up to three children are
 *    created with gSpawnDirectShot(&info, model, vec80, vec70, unkC8, p2, unk24,
 *    3, 0, 0, 0, unkE4). The per-child direction gets +0x2000 in x and z for
 *    child 0, -0x2000 for child 1 and +0x2000 in y for child 2, is normalised
 *    with func_8001A180, and is skipped when the dot product with self +0x44
 *    (func_8001AB00) is <= -0xEC83. Each created child with mi.unk4B != 0 is
 *    post-processed with func_800402DC(child, mi.unk4B, 0, 1, 0). unkDC is then
 *    set to 3.
 * 6. Leash: when (unkA6 << 8) < s4 the movement block is disabled
 *    (unk1C = -1).
 * 7. Movement block, entered only while unk1C >= 0: the home position is copied
 *    to unkA8, unk84 accelerates toward unkD4 << 4 by (unkD2 * delta) / 16, and
 *    s3 = delta * unk84 is the step length.
 *    - State 2: leaves for state 1 once y - (unkA2 << 8) >= 0x4001; damps the y
 *      velocity with unk7C = (3 * unk7C + 0x18000) / 4, normalises it, and
 *      cross-products it with the matrix at +0x2C into unkB4.
 *    - States 1 and 3: the countdown unkDA is decremented; when it goes
 *      negative the speed halves, unkD4 = unk84 >> 4, unkDA += 0x12C, and the
 *      movement block is disabled once unk84 < 16. The target is revalidated
 *      against unkC0; the aim point is the target position, extrapolated by the
 *      target velocity times its own speed and delta when the target's serial
 *      is newer. The distance is measured with func_8001A314, which normalises
 *      the vector in place and returns length << 2. Within 0x7FFF the state
 *      drops to 0 if the target is "complete" (type 1 with a positive
 *      half-word at target->+0xAC + 0x324, or type 0x12 with the same test one
 *      indirection deeper); in state 3 the y component is lifted by 0xA000
 *      before the test. If the step covers the whole distance the entity snaps
 *      onto the direction (or stops when closer than 8), otherwise the velocity
 *      is smoothed toward it with func_8001A6F8, normalised, lifted by 0x2000
 *      in y and cross-producted into unkB4.
 * 8. gVectorAddScaled(position, velocity, s3, position) integrates the position.
 *    For unkDC == 0x15 the state 3 -> state 1 transition happens and, when the
 *    target is at least 0x301 away while the height above the floor is below
 *    0x800, the position is raised by 0x200, state 3 is re-entered and the
 *    speed halves.
 * 9. Collision: func_800684AC(unkA8, position, out, 0, 0) != 0 disables
 *    movement and snaps the position to the corrected point; otherwise
 *    func_80066F78(position, 0, 0, 0) gives the floor height and a position
 *    below it disables movement and pulls the position halfway back toward
 *    unkA8.
 * 10. Basis: the point (unkB4, unkB8 + 0x20000, unkBC) is built and, when
 *     unkE4 bit 1 is set, the spin angle unkD6 is advanced by
 *     (delta << 12) >> 6 modulo 0x1000, func_800188D0 builds a rotation matrix
 *     at +0x2C, func_80019310 builds a second basis into spB8 and the rotation
 *     is post-multiplied (func_80010820, func_8001006C with count 3). Otherwise
 *     func_80019310 writes the basis directly to +0x2C. func_80010750 loads it
 *     into the GTE and func_80010234 transforms the frame vertex at
 *     model->frames + (frame << 4) + 8 into +0x5C.
 *
 * @par Calls
 * func_8003B898 (effect attach), func_8004E80C / func_8006C224 (spawn source
 * lookup), gSpawnDirectShot (child object creation), func_800402DC (post-create
 * parameter), func_80044B34 / func_800175F8 (animation), func_8001A4E8 and
 * func_8001A314 (octagonal / exact distance), func_8001A180 (normalise),
 * func_8001A6F8 (3/4 low pass), func_8001AB00 (dot product), gVectorAddScaled
 * (add scaled vector), func_80010974 (cross product), func_800684AC
 * (collision), func_80066F78 (floor height), func_80069A38, func_8001A13C,
 * func_800188D0 / func_80019310 / func_80010820 / func_8001006C / func_80010750 /
 * func_80010234 (matrix and transform pipeline).
 *
 * @par Globals
 * No global variable is written. The function writes only self: +0x1C, +0x78,
 * +0x7C, +0x80, +0xB4, +0xB8, +0xBC, +0xC0, +0xC4, +0xC8, +0xD6, +0xDA,
 * +0xDC, +0xE0, +0x2C, +0x84, +0xD4 and the model instance radius/frame.
 *
 * @par Side effects
 * Moves the entity, may create up to three new objects in the global object
 * lists, may disable further movement, and leaves the GTE loaded with this
 * entity's matrix.
 *
 * @par Callers
 * func_80036624 (direct), message 6 of its dispatch table - i.e. the entity's
 * per-frame tick handler, with the message argument being the time step.
 *
 * @par Likely
 * The entity is an aircraft or hover vehicle (the state 2 climb, the constant
 * y lift of 0x2000/0xA000 and the floor test all fit a flyer), and the burst at
 * s4 >= 0x3001 is a deployment or drop of three units once the entity has left
 * its home area. The collision call is a wall/terrain pushback and the floor
 * test a "sink into the ground" recovery.
 *
 * @par Unknown
 * - The semantics of +0xD0, +0xD2, +0xD8 and +0xDD beyond their role as spawn
 *   and speed parameters.
 * - What byte 1 of the block at +0xEC encodes (only the values 1 and 2 are
 *   distinguished).
 * - Whether the +0xB4/+0xB8/+0xBC vector is a heading, an up vector or a
 *   velocity for the second body; it is fed to func_80019310 as basis input.
 */
void gGuidedProjectileUpdate(struct Obj35B7C *self, s32 delta) __asm__("func_80035B7C");

void gGuidedProjectileUpdate(struct Obj35B7C *self, s32 delta)
{
    struct Obj35B7C *p1;
    struct Obj35B7C *p2;
    struct Obj35B7C *target;
    s32 s3;
    s32 s4;
    s32 i;
    s32 temp;
    s32 vec30[3];
    s32 vec40[3];
    s32 sp50[3];
    s32 sp60[3];
    s32 vec70[3];
    s32 vec80[3];
    struct SpawnInfo35B7C info;
    s32 vecA8[3];
    s32 spB8[9];
    s32 spE0[3];

    if (self->unkE0 == 4) {
        gSpawnEntitySlotTypes(self->unk20, self, 0, (self->unk19 >> 3) & 0x1E, 2);
        if (self->unkDD != 0xFF) {
            target = self->unkC8;
            if (target == 0) {
                target = self;
            }
            p1 = (struct Obj35B7C *)func_8004E80C(target, self->unkDD, self->unkD8, 1,
                                                   self->unkA6 << 8, 0xFFFF0000, 0);
        } else {
            if (self->unkC8 != 0) {
                target = self->unkC8;
                if (target->unk16 == 0x12) {
                    target = (struct Obj35B7C *)target->unkB4;
                }
                p1 = (struct Obj35B7C *)func_8006C224(target, self->unkC4, self->unkA6 << 8, -1);
            } else {
                p1 = 0;
            }
        }
        if (p1 != 0) {
            register s32 val __asm__("$2");
            register s32 speed __asm__("$3");
            self->unkDA = 0x5DC;
            self->unkE0 = 1;
            val = *(s32 *)((u8 *)p1 + 8);
            speed = self->unkD4;
            self->unkC4 = p1;
            self->unkC0 = val;
            self->unk84 = speed;
        } else {
            self->unkE0 = 0;
        }
    }

    if (self->mi.model != 0 && self->mi.model->unk10 != 0) {
        switch (((u8 *)self->unkEC)[1]) {
        case 1:
            temp = delta;
            break;
        case 2:
            temp = self->unk84 * delta;
            break;
        default:
            temp = 0;
            break;
        }
        func_80044B34(self, self->unkE8, temp, 0);
        gSetModelFrame(&self->mi, self->unkF4);
    }

    if (self->unkC8 != 0) {
        if (self->unkCC != (s32)self->unkC8->unk08) {
            self->unkC8 = 0;
        }
    }

    vec30[0] = self->unkA0 << 8;
    vec30[1] = self->unkA2 << 8;
    vec30[2] = self->unkA4 << 8;
    s4 = func_8001A4E8(vec30, (s32 *)((u8 *)self + 0x50));

    if (self->unkDC == 0x12 && s4 >= 0x3001) {
        info.unk06 = self->unkD4;
        info.unk08 = self->unkD2;
        info.unk00 = self->unk1A;
        info.unk0A = self->unkA6 << 4;
        info.unk12 = 0;
        info.unk0E = self->unk20;
        info.unk02 = self->unkD0 >> 4;
        for (i = 0; i < 3; i++) {
            if (self->unkDD != 0xFF) {
                target = self->unkC8;
                if (target == 0) target = self;
                p2 = (struct Obj35B7C *)func_8004E80C(target, self->unkDD, self->unkD8, 1,
                                                       self->unkA6 << 8, 0xFFFF0000, 0);
            } else {
                target = self->unkC8;
                if (target != 0) {
                    register struct Obj35B7C *arg __asm__("$5");
                    if (target->unk16 == 0x12) target = (struct Obj35B7C *)target->unkB4;
                    if (i > 0) {
                        arg = ((struct Obj35B7C *)vecA8[i - 1])->unkC4;
                    } else {
                        arg = self->unkC4;
                    }
                    p2 = (struct Obj35B7C *)func_8006C224(target, arg, self->unkA6 << 8, 0x9C40);
                } else {
                    p2 = 0;
                }
            }
            if (p2 == 0) {
                if ((i & 1) == 0) {
                    p2 = self->unkC4;
                } else {
                    p2 = ((struct Obj35B7C *)vecA8[i - 1])->unkC4;
                }
                if (p2 == 0) {
                    break;
                }
            }
            *(struct Vec3_35B7C *)vec80 = *(struct Vec3_35B7C *)((u8 *)self + 0x50);
            vec70[0] = *(s32 *)((u8 *)p2 + 0x5C) - *(s32 *)((u8 *)self + 0x5C);
            vec70[1] = *(s32 *)((u8 *)p2 + 0x60) - *(s32 *)((u8 *)self + 0x60);
            vec70[2] = *(s32 *)((u8 *)p2 + 0x64) - *(s32 *)((u8 *)self + 0x64);
            switch (i) {
            case 0:
                vec70[0] += 0x2000;
                vec70[2] += 0x2000;
                break;
            case 1:
                vec70[0] -= 0x2000;
                vec70[2] -= 0x2000;
                break;
            case 2:
                vec70[1] += 0x2000;
                break;
            }
            gNormalizeVector(vec70);
            temp = gVectorDot((s32 *)((u8 *)self + 0x44), vec70);
            if (temp > -0xEC83) {
                void *created = gSpawnDirectShot(&info, self->mi.model, vec80, vec70,
                                              self->unkC8, p2, self->unk24, 3, 0, 0, 0,
                                              self->unkE4);
                vecA8[i] = (s32)created;
                if (created != 0) {
                    if (self->mi.unk4B != 0) {
                        gSpawnTypeAtOwner((void *)vecA8[i], (u8)self->mi.unk4B, 0, 1, 0);
                    }
                }
            } else {
                break;
            }
        }
        self->unkDC = 3;
    }

    if ((self->unkA6 << 8) < s4) self->unk1C = -1;

    if (self->unk1C >= 0) {
        *(struct Vec3_35B7C *)&self->unkA8 = *(struct Vec3_35B7C *)((u8 *)self + 0x50);
        {
            s32 accel;
            s32 lim;
            lim = self->unkD4 << 4;
            accel = self->unkD2;
            if (accel != 0) {
                if (self->unk84 < lim) {
                    self->unk84 += (accel * delta) / 16;
                    if (self->unk84 > lim) self->unk84 = lim;
                }
            }
        }
        s3 = delta * self->unk84;
        p1 = 0;
        switch ((u32)self->unkE0) {
        case 2:
            if (*(s32 *)((u8 *)self + 0x54) - (self->unkA2 << 8) >= 0x4001) self->unkE0 = 1;
            { register s32 y __asm__("$3"); s32 c; c = 0x18000; y = self->unk7C; y *= 3; self->unk7C = (y + c) / 4; }
            gNormalizeVector(&self->unk78);
            gGteCrossProduct(&self->unk78, (s32 *)((u8 *)self + 0x2C), &self->unkB4);
            break;
        case 1:
        case 3:
            self->unkDA = (u16)(self->unkDA - delta);
            if ((s16)self->unkDA < 0) {
                self->unk84 /= 2;
                self->unkD4 = (s16)(self->unk84 >> 4);
                self->unkDA += 0x12C;
                if (self->unk84 < 16) {
                    self->unk1C = -1;
                }
            }
            p1 = self->unkC4;
            if (p1 != 0 && p1->unk08 != (void *)self->unkC0) p1 = 0;
            if (p1 != 0) {
                *(struct Vec3_35B7C *)sp60 = *(struct Vec3_35B7C *)((u8 *)p1 + 0x5C);
                if ((s32)p1->unk08 > (s32)self->unk08) {
                    gVectorAddScaled(sp60, (s32 *)((u8 *)p1 + 0x78), p1->unk84 * delta, sp60);
                }
                vec30[0] = sp60[0] - *(s32 *)((u8 *)self + 0x50);
                vec30[1] = sp60[1] - *(s32 *)((u8 *)self + 0x54);
                vec30[2] = sp60[2] - *(s32 *)((u8 *)self + 0x58);
                s4 = gNormalizeGetLength(vec30);
                if (s4 <= 0x7FFF) {
                    if ((p1->unk16 == 1 &&
                         *(s16 *)(*(u8 **)((u8 *)p1 + 0xAC) + 0x324) > 0) ||
                        (p1->unk16 == 0x12 &&
                         *(s16 *)(*(u8 **)(*(u8 **)((u8 *)p1 + 0xB4) + 0xAC) + 0x324) > 0)) {
                        self->unkE0 = 0;
                    }
                }
                if (self->unkE0 == 3) {
                    vec30[1] += 0xA000;
                    s4 = gNormalizeGetLength(vec30);
                }
                if (s3 >= s4) {
                    s3 = s4;
                    if (s3 < 8) {
                    zero:
                        self->unkE0 = 0;
                    } else {
                        *(struct Vec3_35B7C *)&self->unk78 = *(struct Vec3_35B7C *)vec30;
                    }
                } else {
                    vec40[0] = vec30[0] - self->unk78;
                    vec40[1] = vec30[1] - self->unk7C;
                    vec40[2] = vec30[2] - self->unk80;
                    gEaseVectorQuarter(vec30, &self->unk78);
                    gNormalizeVector(&self->unk78);
                    vec40[1] += 0x2000;
                    gNormalizeVector(vec40);
                    gEaseVectorQuarter(vec40, &self->unkB4);
                }
            } else {
                goto zero;
            }
            break;
        case 0:
            break;
        default:
            goto skip;
        }

        {
            gVectorAddScaled((s32 *)((u8 *)self + 0x50), &self->unk78, s3,
                          (s32 *)((u8 *)self + 0x50));
            if (self->unkDC == 0x15) {
                if (self->unkE0 == 3) self->unkE0 = 1;
                if (p1 != 0) {
                    if (func_8001A13C((s32 *)((u8 *)self + 0x50), sp60) >= 0x301) {
                        if (*(s32 *)((u8 *)self + 0x54) - gSampleHeightGridBilinear((s32 *)((u8 *)self + 0x50)) < 0x800) {
                            *(s32 *)((u8 *)self + 0x54) += 0x200;
                            self->unkE0 = 3;
                            self->unk84 /= 2;
                        }
                    }
                }
            }
            if (func_800684AC((s32 *)((u8 *)self + 0xA8), (s32 *)((u8 *)self + 0x50),
                              sp50, 0, 0) != 0) {
                self->unk1C = -1;
                *(struct Vec3_35B7C *)((u8 *)self + 0x50) = *(struct Vec3_35B7C *)sp50;
            } else {
                temp = gGetGroundHeight((s32 *)((u8 *)self + 0x50), 0, 0, 0);
                if (*(s32 *)((u8 *)self + 0x54) < temp) {
                    self->unk1C = -1;
                    *(s32 *)((u8 *)self + 0x50) =
                        (*(s32 *)((u8 *)self + 0x50) + self->unkA8) >> 1;
                    *(s32 *)((u8 *)self + 0x54) =
                        (*(s32 *)((u8 *)self + 0x54) + self->unkAC) >> 1;
                    *(s32 *)((u8 *)self + 0x58) =
                        (*(s32 *)((u8 *)self + 0x58) + self->unkB0) >> 1;
                }
            }
        }
    }

skip:
    spE0[0] = self->unkB4;
    spE0[1] = self->unkB8 + 0x20000;
    spE0[2] = self->unkBC;
    if (self->unkE4 & 2) {
        s32 ang;
        ang = self->unkD6;
        ang += (delta << 12) >> 6;
        ang &= 0xFFF;
        self->unkD6 = ang;
        gBuildRotationMatrix(0, 0, ang, (s32 *)((u8 *)self + 0x2C));
        gBuildBasisZFromPair((s32 *)((u8 *)self + 0x78), spE0, spB8);
        gGteLoadRotation(spB8);
        gGteTransformWords((s32 *)((u8 *)self + 0x2C), (s32 *)((u8 *)self + 0x2C), 3);
    } else {
        gBuildBasisZFromPair((s32 *)((u8 *)self + 0x78), spE0, (s32 *)((u8 *)self + 0x2C));
    }
    gGteLoadMatrix((s32 *)((u8 *)self + 0x2C));
    gGteTransformVerticesWide((s32 *)((u8 *)self->mi.model->frames + ((self->mi.frame << 4) + 8)),
                  (s32 *)((u8 *)self + 0x5C), 1);
}

