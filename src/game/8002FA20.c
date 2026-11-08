#include "game/types.h"

struct Sub {
    u8 pad[0x24];
    s8 type;
    u8 kind;
    u16 value;
    s16 height, x, y, z;
    s32 speed, field;
    s16 angle, rate;
};
struct Arg { u8 pad[8]; struct Sub *sub; };
struct Self {
    u8 pad00[0x24]; u32 flags; s32 model;
    u8 matrix[0x24]; s32 pos[3];
    u8 pad5c[0xc]; s32 size; u8 pad6c[2]; s16 max;
    u8 pad70[0x44]; s32 x,y,z,kind; s16 value, fieldc6;
    s32 fieldc8; volatile s32 speed; s32 fieldd0, fieldd4;
};
extern s32 func_8002AD58(s32, struct Self *, struct Arg *, s32);
extern s32 func_80067684(s32 *, s32);
extern void func_800188D0(s32, s32, s32, void *);
#define FMULU(a,b) ({ s32 r_; __asm__("nop\n\tnop\n\tmultu %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
void func_8002FA20(struct Self *self, struct Arg *arg, s32 arg2)
{
    struct Sub *sub;
    s32 max, speed, mode, type;
    register s32 rate __asm__("$3");
    func_8002AD58(2, self, arg, arg2);
    sub = arg->sub;
    max = sub->x;
    if (max < sub->y) max = sub->y;
    if (max < sub->z) max = sub->z;
    if (!self->model) { self->max = max; self->size = max << 4; }
    self->kind = sub->kind;
    self->value = sub->value;
    self->fieldc8 = sub->field;
    self->x = sub->x << 4;
    self->y = sub->y << 4;
    self->z = sub->z << 4;
    speed = sub->speed;
    if (speed > 0) {
        speed >>= 1;
        self->speed = FMULU(0x12c0000, 0x80000000u / (u32)speed) >> 16;
    } else {
        speed = -speed;
        speed >>= 1;
        self->speed = -FMULU(0x12c0000, 0x80000000u / (u32)speed) >> 16;
    }
    if (self->speed <= 0) self->speed = 1;
    self->fieldd0 = 0;
    type = sub->type;
    if (type != 0) {
        if (type == 1) mode = -2;
        else mode = type - 2;
    } else mode = -1;
    self->pos[1] = func_80067684(self->pos, mode) + (sub->height << 4);
    self->flags |= 0x800001;
    if (sub->angle >= 0) {
        func_800188D0(sub->angle, 0, 0, self->matrix);
        self->fieldd4 = 0;
    } else {
        rate = sub->rate << 4;
        self->fieldc6 = 0;
        self->fieldd4 = (rate / 218) >> 4;
    }
    if (self->kind == 1 && self->value == 1) self->kind = 8;
}
