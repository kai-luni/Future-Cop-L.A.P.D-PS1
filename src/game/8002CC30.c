// Categories: Gameplay > Object Update > Steering | AI > Target Selection | Graphics > 3D > Actor Transform
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
extern void gBuildRotationMatrix(s32, s32, s32, void *) __asm__("func_800188D0");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
extern struct Obj *func_8004E80C(struct Obj *, s32, s32, s32, s32, s32, s32);
extern s32 gVectorLength3D(s32 *) __asm__("func_8001A100");
extern s32 gAtan2(s32, s32) __asm__("func_80095C30");
extern void func_80044B34(struct Obj *, void *, s32, s32);
extern void gSetModelFrame(void *, s32) __asm__("func_800175F8");
s32 gUpdatePropTick(struct Obj *self, s32 dt) __asm__("func_8002CC30");


/**
 * @brief Message-6 tick of an animated object: serves the command queue, steps
 *        the script, then runs one of three behaviours - spin in place, steer
 *        at a target, or idle - with rate-limited yaw and pitch, and finally
 *        advances the animation.
 *
 * Confirmed role
 * --------------
 * func_8002D664 is the message table of this class; case 6 - the tick - calls
 * this function and returns 0. Its set-up counterpart is func_8002C584 (message
 * 2), which writes most of the fields read here (speed, range, yaw, kind).
 *
 * @param self Object block: id +0x8, command queue +0xC, model +0x28, matrix
 *        +0x2C, position +0x5C, radius +0x68, frame +0x70, script +0xA0, target
 *        +0xB8, target id +0xBC, animation +0xC8, frame index +0xD4, kind
 *        +0xDC, mode +0xDE, flags +0xDF, speed +0xE8, range +0xEA, heading
 *        +0xEC, timer +0xF0, yaw +0xF2, pitch +0xF4.
 * @param dt   Frame delta; multiplies speed for the yaw and pitch steps and is
 *        passed to the script and the animation stepper.
 *
 * @return Always 0. Confirmed: "return 0" appears exactly twice, once for a
 *         missing model and once at the end.
 *
 * @par Confirmed prelude
 * A non-zero command opcode at +0xC runs func_8002CB20, which consumes the
 * opcodes 0x29..0x2E (kind, a 4-bit flag and a model swap). Without a model at
 * +0x28 the function returns immediately. A script pointer at +0xA0 is stepped
 * with func_80060238(address of +0xA0, dt, self, 0, 1) - the address is passed,
 * so the script can replace its own pointer. Bit 4 of +0xDF ends the tick.
 *
 * @par Confirmed spin (bit 2)
 * yaw = (yaw + dt * speed) & 0xFFF, then func_800188D0(timer(+0xF0), 0, yaw,
 * matrix): the timer field is the x rotation, so the object spins about its
 * facing axis - Likely a rotating prop. The model frame at +0x70 (frame size
 * 0x10, table at model+0x34) is transformed to +0x5C.
 *
 * @par Confirmed steering (bit 1)
 * Target choice has three sources. Bit 0x10 uses the fixed target at +0xB8,
 * but only while +0xBC still equals target->id(+0x8). With mode(+0xDE) == 0 the
 * object simply faces backwards (yaw + 0x800, pitch 0). Otherwise the search
 * runs every 1500 frames through the timer at +0xF0:
 * func_8004E80C(self, mode, kind(+0xDC), 1, range << 4, 0xffff0000, 0), and the
 * result plus its id are cached at +0xE0/+0xE4. A cached target is only used
 * while the id still matches and while
 * func_8001A100(delta) - (target radius + own radius) < range << 4.
 *
 * @par Confirmed aim
 * yaw = atan2(dx, dz) with func_80095C30, masked to 0xFFF. Pitch is computed
 * only with bit 8: the planar distance is built with the octagonal
 * approximation used elsewhere in the engine (sort |dx| and |dz|, the
 * shift-add form of the square root) and the pitch is
 * atan2(dy, planar) shifted left and right by 12, then clamped to +-512. The
 * fallback label sets yaw = heading(+0xEC) and pitch 0.
 *
 * @par Confirmed rate limit
 * Bit 0x40 restricts the request to heading +-0x300. For yaw and pitch alike the
 * difference is wrapped into -0x800..0x800, the step is dt * speed and is
 * clamped to the absolute difference, and the angle is advanced by that step.
 * Only when something changed is the matrix rebuilt (func_800188D0 with yaw,
 * pitch, 0), loaded (func_80010750), used to transform the model part
 * (func_80010234) and its row 2 (words at +0x18/+0x1C/+0x20) copied into the
 * vector at +0x78 - for this rotation that row is the local z axis, Likely a
 * cached forward vector.
 *
 * @par Confirmed animation
 * At the end, when model+0x10 is non-zero, func_80044B34(self, +0xC8, dt, 0)
 * steps the animation and func_800175F8(address of the model pointer, +0xD4)
 * applies the frame. Note that the address of the pointer field is passed, not
 * the pointer itself.
 *
 * @par Likely
 * The 1500-frame search period, the 0xffff0000 argument (a "no filter" mask)
 * and the radius subtraction describe a turret or a homing prop that reacquires
 * a target about every 25 seconds and otherwise keeps its current one.
 *
 * @par Unknown
 * The meaning of kind +0xDC (a search class passed straight to func_8004E80C)
 * and of mode +0xDE (0 means "face backwards") is not provable from this file.
 *
 * @par Globals
 * None. Confirmed: no D_ symbol is referenced; everything is reached through
 * the object block and the model it points to.
 *
 * @par Model offsets used (Confirmed)
 * +0x10 animation flag, +0x34 frame table, frame stride 0x10. Object matrix at
 * +0x2C with the translation word at +0x50 shared with the position.
 *
 * @par Control flow
 * 1. Command queue, model check, script step, bit 4 exit.
 * 2. Spin, steer or idle.
 * 3. Yaw/pitch rate limiting and matrix rebuild.
 * 4. Cache matrix row 2.
 * 5. Animation step and frame.
 *
 * @par Calls
 * func_8002CB20 (commands), func_80060238 (script), func_8004E80C (target
 * search), func_8001A100 (distance), func_80095C30 (atan2, three sites),
 * func_800188D0 (matrix), func_80010750 (matrix to GTE), func_80010234 (part
 * transform), func_80044B34 (animation), func_800175F8 (frame).
 *
 * @par Side effects
 * Writes yaw, pitch, timer, cached target, matrix, the vector at +0x78, the
 * animation track and the frame index; advances the command queue and possibly
 * the script pointer. No global variable is written.
 *
 * @par Callers
 * func_8002D664, message 6 (analysis/edges.json, kind "direct").
 *
 * @par Environment path
 * Message-6 tick of the object classes dispatched by func_8002D664 (the same
 * classes whose init is func_8002C584): it services the pending command bytes
 * with func_8002CB20, returns when no model is bound, and ticks the attached
 * attack descriptor with func_80060238 (the attack timeline that fires
 * through func_8005F6E8). Depending on the flag byte +0xDF it then spins in
 * place (flag 2), aims yaw and pitch at a fixed target or at one re-acquired
 * every 1500 time units with func_8004E80C and range-checked with
 * func_8001A100 (flag 1, atan2 via func_80095C30, pitch clamped to +-512, yaw
 * optionally limited to +-0x300 around the heading), or stays idle. Angle
 * changes are rate-limited to dt * speed; when they change it rebuilds and
 * loads the matrix (func_800188D0, func_80010750), transforms the current
 * frame vertex (func_80010234) and caches matrix row 2 at +0x78, and it
 * always finishes with func_80044B34 and func_800175F8. No globals are
 * referenced.
 */
s32 gUpdatePropTick(struct Obj *self, s32 dt)
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
        gBuildRotationMatrix(self->timer, 0, self->yaw, self->matrix);
        gGteLoadMatrix(self->matrix);
        gGteTransformVerticesWide(&self->model->frames->frame[self->frame], &self->pos, 1);
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
            comparison = gVectorLength3D(delta) - (target->radius + self->radius) < (self->range << 4);
            if (!comparison) goto fallback;
        }
    }
    yaw = gAtan2(delta[0], delta[2]) & 0xfff;
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
        pitch = -((gAtan2(first, lo - hi) << 12)) >> 12;
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
        gBuildRotationMatrix(self->yaw, self->pitch, 0, self->matrix);
        gGteLoadMatrix(self->matrix);
        gGteTransformVerticesWide(&self->model->frames->frame[self->frame], &self->pos, 1);
        self->old.x = *(s32 *)(self->matrix + 0x18);
        self->old.y = *(s32 *)(self->matrix + 0x1c);
        self->old.z = *(s32 *)(self->matrix + 0x20);
    }
done:
    if (self->model->anim) {
        func_80044B34(self, self->anim, dt, 0);
        gSetModelFrame(&self->model, self->animFrame);
    }
    return 0;
}
