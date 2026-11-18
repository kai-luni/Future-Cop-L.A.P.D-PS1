// Categories: Gameplay > Object Spawning | Game Engine > Object Setup | Physics > Collision > Ground Query
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
extern void gBuildRotationMatrix(s32, s32, s32, void *) __asm__("func_800188D0");
#define FMULU(a,b) ({ s32 r_; __asm__("nop\n\tnop\n\tmultu %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(r_) : "r"(a), "r"(b) : "$8", "$9"); r_; })
/**
 * @brief Message-2 spawn setup of the second script family: applies the placement
 *        descriptor's extents, position, ground snap, speed and heading to the
 *        entity.
 *
 * @param self Entity block: flag word +0x24, model record +0x28, matrix +0x2C,
 *        position +0x50, size +0x68, extent +0x6E, extent words +0xB4/+0xB8/+0xBC,
 *        kind +0xC0, value +0xC4, spin angle +0xC6, word +0xC8, speed +0xCC,
 *        +0xD0 and rate +0xD4.
 * @param arg Argument block; the descriptor it points at is at +0x08, with type
 *        +0x24, kind +0x25, value +0x26, height +0x28, x/y/z +0x2A/+0x2C/+0x2E,
 *        speed +0x30, word +0x34, angle +0x38 and rate +0x3A.
 * @param extra Forwarded to the base message-2 handler as its payload.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8002AD58(2, self, arg, extra) runs first, i.e. the base spawn hook.
 * The extent +0x6E is the largest of descriptor +0x2A, +0x2C and +0x2E and, only
 * while the model record at +0x28 is null, +0x68 takes extent << 4.
 * Copies: kind +0xC0 = +0x25, value +0xC4 = +0x26, +0xC8 = +0x34, and
 * +0xB4/+0xB8/+0xBC = +0x2A/+0x2C/+0x2E shifted left by 4.
 * Speed +0xCC: for a positive descriptor value it is the middle-bits product of
 * 0x12C0000 with the reciprocal of half that value, divided back by 16; a negative
 * descriptor value produces the same magnitude with the opposite sign; anything that
 * ends up at or below zero is raised to 1. +0xD0 becomes 0.
 * Ground snap: the selector is -1 for type 0, -2 for type 1 and type - 2 otherwise,
 * and position +0x54 becomes func_80067684(pos, selector) plus
 * (descriptor +0x28 << 4).
 * The flag word gains 0x800001. A non-negative descriptor angle at +0x38 builds the
 * matrix with func_800188D0(angle, 0, 0, obj + 0x2C) and clears +0xD4; a negative
 * angle instead clears the spin angle at +0xC6 and sets
 * +0xD4 = ((+0x3A << 4) / 218) >> 4.
 * Finally kind 1 combined with value 1 is rewritten to kind 8.
 *
 * @par Likely role
 * The placement half of the second script family whose handler func_8003015C answers
 * 0xD8 for message 1; the case-6 tick of that handler advances +0xC6 by
 * (delta * +0xD4) & 0xFFF, which fits the rate written here, and the reciprocal
 * speed at +0xCC fits a mover that stores frames per unit rather than units per
 * frame. The kind 1 / value 1 remap looks like "variant A becomes variant B" of the
 * same prop class.
 *
 * @par Unknown
 * - Whether +0xC6 and +0xD4 really are a spin angle and its per-frame rate; only the
 *   caller's arithmetic supports it.
 * - What kind 8 means and why it replaces kind 1 with value 1.
 * - Why the extent is only stored when no model is attached.
 *
 * @par Callers
 * func_8003015C, case 2 of its message table.
 */
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
        gBuildRotationMatrix(sub->angle, 0, 0, self->matrix);
        self->fieldd4 = 0;
    } else {
        rate = sub->rate << 4;
        self->fieldc6 = 0;
        self->fieldd4 = (rate / 218) >> 4;
    }
    if (self->kind == 1 && self->value == 1) self->kind = 8;
}
