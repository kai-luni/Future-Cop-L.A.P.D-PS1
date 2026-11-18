// Categories: Game Engine > Effects > Impact Placement | Gameplay > Damage | Utilities > Mathematics > Geometry
#include "game/types.h"

typedef struct Vec70730 {
    s32 x;
    s32 y;
    s32 z;
} Vec70730;

typedef struct Base70730 {
    u8 pad00[0x28];
    s32 offX;
    s32 offY;
    s32 offZ;
    u8 pad34[0x12];
    s16 unk46;
    u8 pad48[0x14];
    Vec70730 pos;
    u8 pad68[6];
    s16 unk6E;
} Base70730;

typedef struct World70730 {
    u8 pad00[4];
    Base70730 **unk04;
    u8 pad08[6];
    u16 flagsE;
} World70730;

typedef union Flags70730 {
    u8 b;
    u16 h;
} Flags70730;

typedef struct Obj70730 {
    Base70730 *base;
    u8 pad04[8];
    s8 type0C;
    u8 pad0D[0xB];
    Flags70730 flags;
    u16 range;
} Obj70730;

extern u32 gPlayerCount __asm__("D_8009FA0C");
extern Base70730 *gPlayerEntities[] __asm__("D_8009FA58");

extern void gGteTransformWords(Vec70730 *dst, Vec70730 *src, s32 count) __asm__("func_8001006C");
extern void gGteLoadRotation(void *arg) __asm__("func_80010820");
extern void gSinCos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
extern s32 func_80019668(void);
extern s32 func_80019F54(s32 *vector);
extern void func_8003D0F0(Base70730 *base, s32 type, s32 flags, Vec70730 *out);
extern s32 func_80067684(Vec70730 *vec, s32 adjust);
s16 gComputeEffectEndPoint(World70730 *world, Obj70730 *obj, Vec70730 *out, Vec70730 *arg) __asm__("func_80070730");


/**
 * @brief Computes one end point of an effect segment - position, random scatter
 *        and ground height - and reports which actor that point landed on.
 *
 * Confirmed role
 * --------------
 * func_80070E48, the effect walker, calls this once per segment end: for
 * node->a into e->verts and for node->b into e->verts + e->count, using the
 * first half's output as the scatter reference of the second. A non-negative
 * return value is used as an index into D_8009FA58 and the actor found there
 * receives the effect message 14, so the return value is the damage target and
 * the written vector is the visual end point.
 *
 * @param world World block. Only +0xE flags and +0x4 (origin offset pointer)
 *        are read.
 * @param obj   Effect object: a base pointer at +0x0, a type byte at +0xC, a
 *        flag word at +0x18 and the range at +0x1A. When +0x0 is null the same
 *        +0xC is an inline position vector instead of type and flags.
 * @param out   Receives the computed point (three 32-bit words).
 * @param arg   Reference point for the flat branch (flag 8): its x and z are
 *        taken over and its ground height becomes the y value.
 *
 * @return Index into D_8009FA58 of the actor that was hit, or -1. Confirmed:
 *         selected is initialised to -1 and only overwritten inside the
 *         proximity test.
 *
 * @par Confirmed base point
 * With obj->base == 0 the target is the inline vector at obj+0xC and the radius
 * is the absolute range (+0x1A sign-stripped with the shift/mask trick). With a
 * base, func_8003D0F0(base, type(+0xC), flags & 0x10, out) resolves the point;
 * when bit 3 of world->flagsE is clear the base matrix at base+4 is loaded
 * (func_80010820) and the point is transformed (func_8001006C) and moved by the
 * base offsets at +0x28/+0x2C/+0x30. A negative range is turned into
 * (base->unk46 * -range) >> 12, a positive one is used as is.
 *
 * @par Confirmed proximity test (flag 2)
 * One random actor: r = random % D_8009FA0C, and only when the next random
 * value is odd. The vector from that actor's position (+0x5C) to the point is
 * measured with the cheap metric func_80019F54; if (length >> shift) is smaller
 * than the radius plus the actor's +0x6E, the point snaps onto the actor, the
 * radius becomes 0 and the actor index is returned. shift is 5 normally and 4
 * when bit 5 of world->flagsE is set - i.e. the tolerance is doubled.
 *
 * @par Confirmed scatter (flag 9)
 * The radius is scaled by a random 12-bit value (Q12) and a random angle is
 * turned into (sin, 0, cos) with func_8001846C. With flag 8 and a non-null arg
 * the flat branch runs instead: out takes arg's x and z, y becomes
 * func_80067684(arg, -1 or -2) and, when a base exists and bit 3 of
 * world->flagsE is set, the world origin offsets are subtracted. Otherwise the
 * scaled vector is added to the target and y is again replaced by
 * func_80067684 - Confirmed by the identical call pattern in func_8002CC30 and
 * func_80083A10, where the same helper supplies a ground height.
 *
 * @par Likely
 * The third branch (neither flag 2 nor flag 9) builds the point from two
 * random angles and a sine/cosine product instead of a flat circle, which fits
 * a scatter volume rather than a disc. The flag bit 0x10 of obj->flags and bits
 * 8 and 9 of world->flagsE control instance offsetting, height snapping and
 * scatter; their names are not recoverable.
 *
 * @par Unknown
 * The exact meaning of base+0x46 (reciprocal of a ground-follow radius, scaled
 * by 4096) and of the actor field +0x6E (a personal radius, added to the
 * tolerance) is not proven - both are only ever used additively here.
 *
 * @par Globals
 * D_8009FA0C at 0x8009FA0C - actor count, used as the random modulus.
 * D_8009FA58 at 0x8009FA58 - array of actor bases; position at +0x5C and the
 * tolerance at +0x6E are read.
 *
 * @par Object offsets used (Confirmed)
 * obj: +0x0 base, +0xC type or inline position, +0x18 flags, +0x1A range.
 * base: +0x4 matrix, +0x28/+0x2C/+0x30 offsets, +0x46 s16 radius factor,
 * +0x5C position, +0x6E s16 radius.
 * world: +0x4 origin pointer (its +0x28/+0x2C/+0x30 offsets), +0xE flags.
 *
 * @par Control flow
 * 1. selected = -1. mode = bit 3 of world->flagsE, but only when a base exists.
 * 2. obj == 0 -> out = (0,0,0), return -1.
 * 3. Base point and radius as above.
 * 4. Optional one-actor proximity test, which may retarget and set selected.
 * 5. Radius truncated to s16; 0 -> out = target, return.
 * 6. Scatter (circle or tilted) and Y replacement by ground height.
 * 7. If mode is still set, subtract the world origin offsets.
 * 8. Return selected.
 *
 * @par Calls
 * func_8003D0F0 (base point), func_80010820 (matrix to GTE), func_8001006C
 * (transform), func_80019668 (random, five sites), func_8001846C (sin/cos, two
 * sites), func_80019F54 (cheap length), func_80067684 (ground height).
 *
 * @par Side effects
 * Writes out and arg is only read. Replaces the GTE rotation matrix with the
 * base matrix, so every later GTE call in the same frame inherits it until
 * another function reloads it. Consumes the global random generator. No global
 * variable is written.
 *
 * @par Callers
 * func_80070E48, two call sites (analysis/edges.json, kind "direct"); the same
 * address also has an "address_taken" edge.
 *
 * @par Environment path
 * Computes one end point of a beam or ribbon strip and returns the index of
 * the player entity it locked onto, or -1; func_80070E48 calls it for both
 * ends and uses the index to send the damage event. The base point is the
 * vector at obj + 0xC, or func_8003D0F0's point rotated and offset into the
 * base object's frame, and the scatter radius comes from the range field (a
 * negative range scales the base object's +0x46 value). With flag bit 1 set,
 * a random player in gPlayerEntities is tested half of the time and the point
 * snaps to that player's position when it lies within the radius plus the
 * player's +0x6E radius. Otherwise the point is scattered: with flag mask 9
 * by a random horizontal offset and then snapped to the ground with
 * func_80067684, or without it in a random 3D direction; in mode 1 the base
 * object's offset is subtracted from the result.
 */
s16 gComputeEffectEndPoint(World70730 *world, Obj70730 *obj, Vec70730 *out, Vec70730 *arg)
{
    Vec70730 vec;
    s32 trig[4];
    s16 selected;
    register s32 speed __asm__("$19");
    s32 speed16;
    Vec70730 *target;
    u8 mode;
    s32 adjust;
    s16 rs;
    register u32 range __asm__("$2");

    selected = -1;
    mode = 0;

    if (obj->base != 0) {
        mode = (world->flagsE >> 3) & 1;
    }

    if (obj == 0) {
        goto null_obj;
    }

    if (obj->base == 0) {
        register u32 x __asm__("$3");
        register s32 m __asm__("$2");

        target = (Vec70730 *)((u8 *)obj + 0xC);
        x = obj->range;
        m = ((s32)(s16)x) >> 31;
        speed = (x + m) ^ m;
    } else {
        func_8003D0F0(obj->base, obj->type0C, obj->flags.b & 0x10, out);
        target = out;

        if (mode == 0) {
            gGteLoadRotation((u8 *)obj->base + 4);
            gGteTransformWords(out, out, 1);
            out->x += obj->base->offX;
            out->y += obj->base->offY;
            out->z += obj->base->offZ;
        }

        rs = obj->range;
        range = obj->range;
        mode = 0;
        if (rs < 0) {
            speed = (u32)(obj->base->unk46 * -rs) >> 12;
        } else {
            speed = range;
        }
    }

    if ((obj->flags.h & 2) != 0) {
        s32 r;
        s32 shift;
        s32 dist;

        r = func_80019668() % gPlayerCount;

        if ((func_80019668() & 1) != 0) {
            shift = (world->flagsE & 0x20) ? 4 : 5;
            vec.x = target->x - gPlayerEntities[r]->pos.x;
            vec.y = target->y - gPlayerEntities[r]->pos.y;
            vec.z = target->z - gPlayerEntities[r]->pos.z;
            dist = func_80019F54((s32 *)&vec);

            if ((dist >> shift) < ((s16)speed + gPlayerEntities[r]->unk6E)) {
                target = &gPlayerEntities[r]->pos;
                speed = 0;
                selected = r;
            }
        }
    }

    range = speed << 16;
    speed16 = (s32)range >> 16;

    if (speed16 == 0) {
        goto copy_target;
    }

    if ((obj->flags.h & 9) != 0) {
        speed16 = (u32)((func_80019668() & 0xFFF) * speed16) >> 12;
        gSinCos(func_80019668() & 0xFFF, &vec.x, &vec.z);
        vec.y = 0;

        if (((obj->flags.h & 8) != 0) && (arg != 0)) {


            out->x = arg->x;
            out->z = arg->z;

            if ((func_80019668() & 1) != 0) {
                adjust = -1;
            } else {
                adjust = -2;
            }

            out->y = func_80067684(arg, adjust);

            if ((obj->base != 0) && ((world->flagsE & 8) != 0)) {
                out->x -= (*world->unk04)->offX;
                out->y -= (*world->unk04)->offY;
                out->z -= (*world->unk04)->offZ;
                mode = 0;
            }

            goto done;
        }

        vec.x = ((vec.x >> 4) * (s16)speed16) >> 8;
        vec.y = ((vec.y >> 4) * (s16)speed16) >> 8;
        vec.z = ((vec.z >> 4) * (s16)speed16) >> 8;

        if ((obj->base != 0) && ((world->flagsE & 8) != 0)) {
            vec.x += (*world->unk04)->offX;
            vec.y += (*world->unk04)->offY;
            vec.z += (*world->unk04)->offZ;
            vec.x += target->x;
            vec.y += target->y;
            vec.z += target->z;

            if ((func_80019668() & 1) != 0) {
                adjust = -1;
            } else {
                adjust = -2;
            }

            vec.y = func_80067684(&vec, adjust);

            out->x = vec.x - (*world->unk04)->offX;
            out->y = vec.y - (*world->unk04)->offY;
            out->z = vec.z - (*world->unk04)->offZ;
            mode = 0;
            goto done;
        }

        out->x = vec.x + target->x;
        out->y = vec.y + target->y;
        out->z = vec.z + target->z;

        if ((func_80019668() & 1) != 0) {
            adjust = -1;
        } else {
            adjust = -2;
        }

        out->y = func_80067684(out, adjust);
        goto done;
    }

    {
        s32 cos1;
        s32 sin2;
        s32 cos2;
        s32 p1;
        s32 p2;
        register s32 product __asm__("$3");

        gSinCos(func_80019668() & 0xFFF, &trig[0], &trig[1]);
        gSinCos(func_80019668() & 0xFFF, &trig[2], &trig[3]);

        cos1 = trig[1] >> 2;
        sin2 = trig[2] >> 2;
        cos2 = trig[3] >> 2;
        p1 = cos1 * sin2;
        p2 = cos1 * cos2;

        trig[1] = cos1;
        trig[2] = sin2;
        trig[3] = cos2;

        vec.x = p1 >> 12;
        vec.y = -trig[0];
        vec.z = p2 >> 12;

        out->x = target->x + (((p1 >> 16) * speed16) >> 8);
        range = vec.y >> 4;
        product = (s32)range * speed16;
        out->y = target->y + (product >> 8);
        range = vec.z >> 4;
        product = (s32)range * speed16;
        out->z = target->z + (product >> 8);
        goto done;
    }

copy_target:
    *out = *target;
    goto done;

null_obj:
    out->x = 0;
    out->y = 0;
    out->z = 0;

done:
    if (mode != 0) {
        out->x -= (*world->unk04)->offX;
        out->y -= (*world->unk04)->offY;
        out->z -= (*world->unk04)->offZ;
    }

    return selected;
}
