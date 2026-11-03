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

extern u32 D_8009FA0C;
extern Base70730 *D_8009FA58[];

extern void func_8001006C(Vec70730 *dst, Vec70730 *src, s32 count);
extern void func_80010820(void *arg);
extern void func_8001846C(u32 angle, s32 *sine, s32 *cosine);
extern s32 func_80019668(void);
extern s32 func_80019F54(s32 *vector);
extern void func_8003D0F0(Base70730 *base, s32 type, s32 flags, Vec70730 *out);
extern s32 func_80067684(Vec70730 *vec, s32 adjust);

s16 func_80070730(World70730 *world, Obj70730 *obj, Vec70730 *out, Vec70730 *arg)
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
            func_80010820((u8 *)obj->base + 4);
            func_8001006C(out, out, 1);
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

        r = func_80019668() % D_8009FA0C;

        if ((func_80019668() & 1) != 0) {
            shift = (world->flagsE & 0x20) ? 4 : 5;
            vec.x = target->x - D_8009FA58[r]->pos.x;
            vec.y = target->y - D_8009FA58[r]->pos.y;
            vec.z = target->z - D_8009FA58[r]->pos.z;
            dist = func_80019F54((s32 *)&vec);

            if ((dist >> shift) < ((s16)speed + D_8009FA58[r]->unk6E)) {
                target = &D_8009FA58[r]->pos;
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
        func_8001846C(func_80019668() & 0xFFF, &vec.x, &vec.z);
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

        func_8001846C(func_80019668() & 0xFFF, &trig[0], &trig[1]);
        func_8001846C(func_80019668() & 0xFFF, &trig[2], &trig[3]);

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
