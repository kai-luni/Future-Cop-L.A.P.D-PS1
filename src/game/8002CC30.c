#include "game/types.h"

struct Vec { s32 x, y, z; };
struct Frame { u8 pad[16]; };
struct Frames { u8 pad[8]; struct Frame frame[1]; };
struct Model { u8 pad00[0x10]; s32 anim; u8 pad14[0x20]; struct Frames *frames; };
struct Obj {
    u8 pad00[8]; s32 id; u8 commands[0x1c];
    struct Model *model;
    u8 matrix[0x30];
    struct Vec pos;
    s32 radius;
    u8 pad6c[4]; s16 frame; u8 pad72[6];
    struct Vec old;
    u8 pad84[0x1c];
    void *script;
    u8 pada4[0x14];
    struct Obj *target; s32 targetId;
    u8 padc0[8]; u8 anim[0xc]; s16 animFrame;
    u8 padd6[6]; s16 type; u8 mode, flags;
    struct Obj *found; s32 foundId;
    s16 speed, range, heading, padEE, timer, yaw, pitch;
};
extern void func_8002CB20(struct Obj *);
extern void func_80060238(void *, s32, struct Obj *, s32, s32);
extern void func_800188D0(s32, s32, s32, void *);
extern void func_80010750(void *);
extern void func_80010234(void *, void *, s32);
extern struct Obj *func_8004E80C(struct Obj *, s32, s32, s32, s32, s32, s32);
extern s32 func_8001A100(s32 *);
extern s32 func_80095C30(s32, s32);
extern void func_80044B34(struct Obj *, void *, s32, s32);
extern void func_800175F8(void *, s32);

s32 func_8002CC30(struct Obj *self, s32 dt)
{
    s32 delta[3];
    struct Obj *target;
    register s32 pitch __asm__("$6");
    register s32 c __asm__("$4");
    s32 yaw, diff, step, a, b;

    if (self->commands[0]) func_8002CB20(self);
    if (!self->model) return 0;
    if (self->script) func_80060238(&self->script, dt, self, 0, 1);
    if (self->flags & 4) goto done;
    if (self->flags & 2) {
        self->yaw = (self->yaw + dt * self->speed) & 0xfff;
        func_800188D0(self->timer, 0, self->yaw, self->matrix);
        func_80010750(self->matrix);
        func_80010234(&self->model->frames->frame[self->frame], &self->pos, 1);
        goto done;
    }
    if (!(self->flags & 1)) goto done;
    if (self->flags & 0x10) {
        if (!self->target || self->targetId != self->target->id) goto fallback;
        delta[0] = self->target->pos.x - self->pos.x;
        delta[1] = self->target->pos.y - self->pos.y;
        delta[2] = self->target->pos.z - self->pos.z;
    } else {
        if (!self->mode) {
            yaw = (self->yaw + 0x800) & 0xfff;
            pitch = 0;
            goto rotate;
        }
        self->timer -= dt;
        if (self->timer <= 0) {
            self->found = func_8004E80C(self, self->mode, self->type, 1, self->range << 4, 0xffff0000, 0);
            self->timer = 1500;
            if (self->found) self->foundId = self->found->id;
        }
        target = self->found;
        if (!target || target->id != self->foundId) goto fallback;
        delta[0] = target->pos.x - self->pos.x;
        delta[1] = target->pos.y - self->pos.y;
        delta[2] = target->pos.z - self->pos.z;
        {
            register s32 comparison __asm__("$3");
            comparison = func_8001A100(delta) - (target->radius + self->radius) < (self->range << 4);
            if (!comparison) goto fallback;
        }
    }
    yaw = func_80095C30(delta[0], delta[2]) & 0xfff;
    if (!(self->flags & 8)) {
        pitch = 0;
        goto rotate;
    }
    {
    register s32 a __asm__("$7");
    register s32 b __asm__("$6");
    a = delta[0]; if (a < 0) a = -a;
    b = delta[2]; if (b < 0) b = -b;
    if (a < b) { a ^= b; b ^= a; a ^= b; }
    if ((a >> 1) < b) {
        c = a + b;
        {
            register s32 t __asm__("$3");
            register s32 v __asm__("$2");
            v = c << 6;
            t = c << 3;
            v += t;
            t = c << 1;
            v += t;
            v += a;
            t = b << 5;
            c = v + t;
        }
        b = a - b;
        a = c >> 7;
    }
    {
        register s32 lo __asm__("$5");
        register s32 hi __asm__("$2");
        register s32 first __asm__("$4");
        first = delta[1];
        lo = b >> 2;
        lo = a + lo;
        hi = b >> 6;
        pitch = -((func_80095C30(first, lo - hi) << 12)) >> 12;
    }
    }
    pitch = pitch < -512 ? -512 : pitch > 512 ? 512 : pitch;
    goto rotate;
fallback:
    yaw = self->heading;
    pitch = 0;
rotate:
    if (self->yaw != yaw) {
        if (self->flags & 0x40) {
            diff = (yaw - self->heading) & 0xfff;
            if (diff > 0x800) diff -= 0x1000;
            if (diff < -0x300) yaw = (self->heading - 0x300) & 0xfff;
            else if (diff > 0x300) yaw = (self->heading + 0x300) & 0xfff;
        }
    }
    if (self->yaw != yaw || self->pitch != pitch) {
        if (self->yaw != yaw) {
            diff = (yaw - self->yaw) & 0xfff;
            if (diff > 0x800) diff -= 0x1000;
            step = dt * self->speed;
            a = ((diff >> 31) + diff) ^ (diff >> 31);
            if (a < step) step = a;
            if (diff < 0) step = -step;
            self->yaw = (self->yaw + step) & 0xfff;
        }
        if (self->pitch != pitch) {
            {
                register s32 tmp __asm__("$2");
                tmp = pitch - self->pitch;
                diff = tmp & 0xfff;
            }
            if (diff > 0x800) diff -= 0x1000;
            step = dt * self->speed;
            a = ((diff >> 31) + diff) ^ (diff >> 31);
            if (a < step) step = a;
            if (diff < 0) step = -step;
            self->pitch = (self->pitch + step) & 0xfff;
        }
        func_800188D0(self->yaw, self->pitch, 0, self->matrix);
        func_80010750(self->matrix);
        func_80010234(&self->model->frames->frame[self->frame], &self->pos, 1);
        self->old.x = *(s32 *)(self->matrix + 0x18);
        self->old.y = *(s32 *)(self->matrix + 0x1c);
        self->old.z = *(s32 *)(self->matrix + 0x20);
    }
done:
    if (self->model->anim) {
        func_80044B34(self, self->anim, dt, 0);
        func_800175F8(&self->model, self->animFrame);
    }
    return 0;
}
