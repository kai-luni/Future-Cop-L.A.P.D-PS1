#include "game/types.h"

struct ObjInner { u8 pad[0x34]; u8 *base; };
struct Obj {
    u8 pad00[8];
    s32 id;
    u8 command;
    u8 pad0d[0x14];
    u8 sound_state;
    s8 sound;
    u8 pad23;
    u32 flags;
    struct ObjInner *inner;
    s32 matrix[9];
    s32 x, y, z;
    u8 transform[0x14];
    s16 part;
    u8 pad72[6];
    s32 savedx, savedy, savedz;
    u8 pad84[0x1c];
    u8 mode, direction, options, start_sound, end_sound;
    u8 pada5[3];
    s32 speed, angle0, angle, angle1, target0, targety, targetz;
};

#define FIXMUL(out, a, b) __asm__ volatile("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(out) : "r"(a), "r"(b) : "$8", "$9")
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

extern void func_8002F280(struct Obj *);
extern void func_800188D0(s32, s32, s32, s32 *);
extern void func_80010750(s32 *);
extern void func_80010234(void *, void *, s32);
extern void func_800457B8(s32, s32);
extern void func_80047284(s32, s32, s32, s32);

s32 func_8002F30C(struct Obj *obj, s32 dt)
{
    s32 speed;
    s32 target;
    s32 old;
    s32 destz;
    s32 delta;
    s32 newv;
    s32 amount;
    s32 absdelta;
    s32 xstep, zstep, zm, c;
    s32 rx, ry, rz;
    u8 changed;

    if (obj->command) func_8002F280(obj);
    speed = obj->speed;
    if (!obj->direction) speed = -speed;
    switch (obj->mode) {
    case 0:
    case 4:
    case 5:
        old = obj->angle;
        if (!obj->direction) target = obj->angle0;
        else target = obj->target0;
        changed = old != target;
        if (!changed) goto stopped;
        delta = (target - old) & 0xfff;
        if (delta > 0x800) delta -= 0x1000;
        amount = (dt * speed) >> 16;
        amount = ABS(amount);
        absdelta = ABS(delta);
        if (absdelta < amount) amount = absdelta;
        if (delta < 0) amount = -amount;
        newv = (old + amount) & 0xfff;
        if (newv == old) break;
        obj->angle = newv;
        if ((obj->options & 2) && newv == target)
            obj->direction = (obj->direction + 1) & 1;
        if (obj->mode == 4) {
            rx = obj->angle1; ry = newv; rz = 0;
        } else if (obj->mode == 5) {
            rx = obj->angle1; ry = 0; rz = newv;
        } else {
            rx = newv; rz = 0; ry = 0;
        }
        func_800188D0(rx, ry, rz, obj->matrix);
        func_80010750(obj->matrix);
        func_80010234(obj->inner->base + (obj->part * 16 + 8), obj->transform, 1);
        break;
    case 1:
    case 2:
        if (!obj->direction) {
            target = obj->angle0;
            destz = obj->angle1;
        } else {
            target = obj->target0;
            destz = obj->targetz;
        }
        changed = 0;
        if (target != obj->x || destz != obj->z) changed = 1;
        if (!changed) goto stopped;
        amount = speed * dt;
        if (obj->mode == 2) {
            zm = obj->matrix[6];
            FIXMUL(xstep, zm, amount);
            zm = obj->matrix[8];
        } else {
            zm = obj->matrix[0];
            FIXMUL(xstep, zm, amount);
            zm = obj->matrix[2];
        }
        FIXMUL(zstep, zm, amount);
        obj->x += xstep;
        if (xstep > 0) c = target < obj->x;
        else c = obj->x < target;
        if (c) obj->x = target;
        obj->z += zstep;
        if (zstep > 0) c = destz < obj->z;
        else c = obj->z < destz;
        if (c) obj->z = destz;
        if ((obj->options & 2) && obj->x == target && obj->z == destz)
            goto toggle;
        goto update;
    case 3:
        newv = obj->y;
        old = newv;
        if (!obj->direction) target = obj->angle;
        else target = obj->targety;
        changed = old != target;
        if (!changed) goto stopped;
        newv += dt * speed;
        if ((newv - target > 0 && speed > 0) || (newv - target < 0 && speed < 0)) newv = target;
        if (newv == old) break;
        obj->y = newv;
        if ((obj->options & 2) && newv == target) {
        toggle:
            obj->direction = (obj->direction + 1) & 1;
        }
    update:
        func_80010750(obj->matrix);
        func_80010234(obj->inner->base + (obj->part * 16 + 8), obj->transform, 1);
    }
    if (changed) {
        if (!(obj->flags & 0x400000) && !obj->sound_state && obj->start_sound)
            obj->sound_state = obj->start_sound;
        obj->savedx = obj->matrix[6];
        obj->savedy = obj->matrix[7];
        obj->savedz = obj->matrix[8];
        __asm__ volatile("");
        return 0;
    }
stopped:
    if (obj->sound_state) obj->sound_state = 0;
    if (obj->flags & 0x400000) {
        func_800457B8(obj->id, obj->sound);
        obj->flags &= ~0x400000;
        if (obj->end_sound) func_80047284(obj->end_sound, obj->x, obj->z, -1);
    }
    return 0;
}
