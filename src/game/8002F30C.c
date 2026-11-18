// Categories: Gameplay > Entity Update > Mover | Physics > Motion | Graphics > GTE > Matrix Build | Platform > Sound
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
extern void gBuildRotationMatrix(s32, s32, s32, s32 *) __asm__("func_800188D0");
extern void gGteLoadMatrix(s32 *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
extern void func_800457B8(s32, s32);
extern void func_80047284(s32, s32, s32, s32);
s32 gStepMoverObject(struct Obj *obj, s32 dt) __asm__("func_8002F30C");


/**
 * @brief Message-6 tick of a scripted mover: sweeps one angle or one axis of
 *        translation toward a target at a fixed rate, with optional ping-pong,
 *        and rebuilds the matrix and the model part afterwards.
 *
 * @param obj Object block.
 * @param dt Frame delta. The rotation modes take the step as (dt * speed) >> 16,
 *        the translation modes as dt * speed directly.
 * @return Always 0.
 *
 * @par Confirmed layout
 * +0x0C command byte, +0x21 sound state, +0x22 sound id, +0x24 flag word,
 * +0x28 model (its +0x34 frame table), +0x2C 3x3 matrix with its row at words
 * 6..8, +0x50 x/y/z, +0x5C transform, +0x70 model part, +0x78 saved matrix row,
 * +0xA0 mode, +0xA1 direction, +0xA2 options, +0xA3 start sound, +0xA4 end sound,
 * +0xA8 speed, +0xAC angle 0, +0xB0 angle, +0xB4 angle 1, +0xB8 target angle,
 * +0xBC target y, +0xC0 target z.
 *
 * @par Confirmed modes
 * A non-zero command byte first runs func_8002F280, the command handler. Modes 0,
 * 4 and 5 rotate: the goal is +0xAC or +0xB8 selected by the direction byte, the
 * difference is wrapped into -0x800..0x800 over a 12-bit angle, the step is
 * clamped to it and applied; bit 2 of the options byte flips the direction on
 * arrival. The swept angle is passed as the x rotation for mode 0, as the y
 * rotation for mode 4 (x fixed at +0xB4) and as the z rotation for mode 5.
 * Modes 1 and 2 translate in the plane: the step is the matrix words 0/2 (mode 1)
 * or 6/8 (mode 2) times speed * dt, and x/z are clamped so the goal is never
 * passed. Mode 3 moves y toward +0xB0 or +0xBC by dt * speed.
 * The rotation modes then reload the matrix with func_800188D0; all moving modes
 * push it with func_80010750 and transform the model part with
 * func_80010234(model + 0x34 + part * 16 + 8, obj + 0x5C, 1).
 *
 * @par Confirmed sound and state
 * While something moves, the start sound +0xA3 is armed into +0x21 unless flag
 * 0x400000 is already set, and the matrix row at words 6..8 is copied into
 * +0x78..+0x80. When nothing moves any more the sound state is cleared; if flag
 * 0x400000 is set the looped sound is stopped with func_800457B8(id, +0x22), the
 * flag is cleared, and the end sound +0xA4 is played positionally with
 * func_80047284(sound, x, z, -1).
 *
 * @par Likely
 * A lift, platform or rotating prop driven from a script: one degree of freedom
 * per mode, a looping travel sound while it moves and a stop sound at the ends.
 *
 * @par Unknown
 * Of the flag word at +0x24 only 0x400000 is used here; the bits written by the
 * setup path (message 2) are not visible in this function.
 *
 * @par Callers
 * func_8002F7C8, the message table that dispatches message 6 here.
 *
 * @par Environment path
 * Message-6 tick of a one-degree-of-freedom mover that func_8002F7C8
 * dispatches at message 6: depending on the mode byte at +0xA0 it sweeps one
 * rotation angle (modes 0, 4 and 5, applied as the x, y or z rotation),
 * translates in the plane along the matrix x/z axes (modes 1 and 2) or moves
 * y (mode 3) toward a target at dt * speed, clamps every step so the goal is
 * never passed, flips the direction for ping-pong when bit 2 of the options
 * byte is set, and then reloads the matrix and transforms one model part.
 * While anything moves it arms the start sound at +0xA3 and caches matrix
 * words 6..8; once nothing moves it clears the sound state and, when flag
 * 0x400000 is set, stops the looped sound with func_800457B8 and plays the
 * end sound positionally with func_80047284. It is an object update outside
 * the render path - it shares only the gGteLoadMatrix /
 * gGteTransformVerticesWide convention with the view code and reads no
 * global.
 */
s32 gStepMoverObject(struct Obj *obj, s32 dt)
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
        gBuildRotationMatrix(rx, ry, rz, obj->matrix);
        gGteLoadMatrix(obj->matrix);
        gGteTransformVerticesWide(obj->inner->base + (obj->part * 16 + 8), obj->transform, 1);
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
        gGteLoadMatrix(obj->matrix);
        gGteTransformVerticesWide(obj->inner->base + (obj->part * 16 + 8), obj->transform, 1);
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
