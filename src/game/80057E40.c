// Categories: Gameplay > Entity Update > Orientation | Graphics > GTE > Matrix Setup | Graphics > Animation
#include "game/types.h"

typedef struct { s32 x, y, z; } Vec3;
typedef struct { s32 m[9]; } Matrix;
struct Target { u8 pad00[8]; s32 id; u8 pad0c[0x50]; Vec3 pos; };
struct Part { u8 pad00[0x1c]; u8 a[0x0c]; u8 b[0x24]; };
struct State {
    u8 pad00[0x68]; s32 id; struct Target *target;
    u8 pad70[0x104]; s16 timer[2]; struct Part parts[2];
    u8 pad210[4]; Matrix matrix; Vec3 pos;
};
struct Info {
    u8 pad00[0x28]; u8 instance[0x50]; Vec3 pos; s32 unk84;
    u8 pad88[0x24]; struct State *state; s16 yaw;
    u8 padb2[0x0c]; s16 yawOffset; s16 pitch;
    u8 padc2[0x2e]; s16 roll;
};
struct Model { u8 pad00[0x34]; u8 *frames; };
struct Object {
    u8 pad00[0x28]; struct Model *model; Matrix matrix; Vec3 pos; Vec3 vertex;
    u8 pad68[8]; s16 frame; u8 pad72[6]; Vec3 origin; s32 unk84;
    u8 pad88[0x2c]; struct Info *info;
};
extern void func_80017668(void *, s32, void *);
extern s32 func_80095C30(s32, s32);
extern void gBuildRotationMatrix(s32, s32, s32, void *) __asm__("func_800188D0");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
extern void func_80052D7C(void *, s32);
extern void func_8001AC28(void *, void *, s32, void *);

/**
 * @brief Steers an entity's orientation toward its target and refreshes it.
 *
 * Computes pitch and yaw to the tracked target, slews the stored angles toward
 * them with a limited turn rate, builds and loads the basis matrix, fetches the
 * frame vertex, runs the remaining entity update and steps two child part
 * animations.
 *
 * @param obj Entity; matrix at +0x2C, position at +0x50, vertex at +0x5C,
 *        frame index at +0x70, origin at +0x78, unk84 at +0x84, info pointer
 *        at +0xB4.
 * @param step Frame delta forwarded to func_80052D7C and to the part timers.
 * @return Nothing.
 *
 * @par Confirmed
 * obj->unk84 = info->unk84, func_80017668(info->instance, 0, &obj->pos) samples
 * the animated instance, and state->target is dropped when state->id differs
 * from target->id.
 * With a target: delta = target->pos - obj->pos; the horizontal distance is the
 * octagonal approximation assembled from shift/add terms (larger and smaller
 * axis magnitudes plus a correction pair), and func_80095C30(delta.y, distance)
 * gives the elevation. pitch = -(angle << 12) >> 12, clamped to -400..400.
 * Yaw comes from func_80095C30(delta.x, delta.z); the difference to info->yaw is
 * wrapped into -0x800..0x800 and clamped to -128..128.
 * Both angles then slew: a difference of 23 or less snaps info->pitch /
 * info->yawOffset to the target, anything larger steps them by 24 per call.
 * func_800188D0(info->yaw + info->yawOffset, info->pitch, info->roll,
 * &obj->matrix) builds the basis, obj->origin = info->pos, func_80010750 loads
 * the matrix into the GTE, and func_80010234(model frames + (frame << 4) + 8,
 * &obj->vertex, 1) transforms one vertex.
 * func_80052D7C(obj, step) runs the entity's own update.
 * The two child parts: state->timer[i] > 0 is decremented by step; while it
 * stays positive func_8001AC28(part.b, part.a, sum, part.b) is called with
 * sum = (timer - 132) << 4 when timer >= 89 and sum = -timer << 3 otherwise; a
 * timer reaching 0 is clamped to 0 and stored back.
 * Finally the 3x3 basis is copied into state->matrix and state->pos is refreshed
 * by func_80017668(&obj->model, 2, &state->pos) - part 2.
 *
 * @par Player model assembly
 * obj is the companion and info is the owning player object in this path.
 * Point 0 of the player's main instance anchors the companion. func_80052D7C
 * derives matrices for the two instances at player state +0x178 and +0x1C4
 * from the companion's model hierarchy; its optional timer offsets are applied
 * afterwards. The instance at +0x210 receives the companion basis and point 2
 * as its position. These placements do not by themselves identify body parts.
 *
 * @par Likely
 * Aiming/tracking for a turret, character or chase camera: the +-400 pitch limit
 * suggests a restricted vertical arc, and the 24-unit slew a deliberately slow
 * turn rate instead of instant snapping. The two timers are Likely hip/knee or
 * wheel phase animations that decay over time.
 *
 * @par Unknown
 * The identity check on state->id/target->id (Likely a stale-target guard across
 * respawns), and the work done by func_80052D7C.
 *
 * @par Callers
 * func_800588D4 (sole direct caller).
 */
void func_80057E40(struct Object *obj, s32 step)
{
    struct Info *info;
    struct State *state;
    struct Target *target;
    Vec3 delta;
    s32 pitch, yaw, diff, a, b, sum, angle, i, timer;
    register s32 offset __asm__("$4");
    register u8 *pointer __asm__("$2");
    info = obj->info;
    obj->unk84 = info->unk84;
    func_80017668(info->instance, 0, &obj->pos);
    state = info->state;
    target = state->target;
    pitch = 0;
    if (target != 0) {
        if (state->id != target->id) {
            state->target = 0;
            target = 0;
        }
    }
    if (target != 0) {
        delta.x = target->pos.x - obj->pos.x;
        delta.y = target->pos.y - obj->pos.y;
        delta.z = target->pos.z - obj->pos.z;
        a = delta.x;
        if (a < 0) a = -a;
        b = delta.z;
        if (b < 0) b = -b;
        if (a < b) {
            a ^= b; b ^= a; a ^= b;
        }
        {
            register s32 distSum __asm__("$4");
            distSum = a + b;
            if ((a >> 1) < b) {
                register s32 t __asm__("$3");
                register s32 acc __asm__("$2");
                acc = distSum << 6;
                __asm__("" : "+r"(acc));
                t = distSum << 3;
                __asm__("" : "+r"(t));
                acc += t;
                __asm__("" : "+r"(acc));
                t = distSum << 1;
                __asm__("" : "+r"(t));
                acc += t;
                acc += a;
                __asm__("" : "+r"(acc));
                t = b << 5;
                __asm__("" : "+r"(t));
                distSum = acc + t;
                b = a - b;
                a = distSum >> 7;
            }
        }
        angle = func_80095C30(delta.y, a + (b >> 2) - (b >> 6));
        pitch = -(angle << 12) >> 12;
        pitch = pitch < -400 ? -400 : pitch > 400 ? 400 : pitch;
    }
    diff = pitch - info->pitch;
    if (diff < 0) {
        if (diff < -23) info->pitch -= 24;
        else info->pitch = pitch;
    } else {
        if (diff < 24) info->pitch = pitch;
        else info->pitch += 24;
    }
    yaw = 0;
    if (target != 0) {
        angle = func_80095C30(delta.x, delta.z);
        yaw = ((angle << 12) >> 12) - info->yaw;
        yaw &= 0xfff;
        if (yaw > 0x800) yaw -= 0x1000;
        yaw = yaw < -128 ? -128 : yaw > 128 ? 128 : yaw;
    }
    diff = yaw - info->yawOffset;
    if (diff < 0) {
        if (diff < -23) info->yawOffset -= 24;
        else info->yawOffset = yaw;
    } else {
        if (diff < 24) info->yawOffset = yaw;
        else info->yawOffset += 24;
    }
    gBuildRotationMatrix(info->yaw + info->yawOffset, info->pitch, info->roll, &obj->matrix);
    obj->origin = info->pos;
    gGteLoadMatrix(&obj->matrix);
    offset = obj->frame;
    pointer = (u8 *)obj->model;
    offset <<= 4;
    __asm__("" : "=r"(offset) : "0"(offset));
    pointer = ((struct Model *)pointer)->frames;
    offset += 8;
    gGteTransformVerticesWide(pointer + offset, &obj->vertex, 1);
    func_80052D7C(obj, step);
    for (i = 0; i < 2; i++) {
        timer = state->timer[i];
        if (timer > 0) {
            timer -= step;
            if (timer > 0) {
                if (timer >= 89) sum = (timer - 132) << 4;
                else sum = -timer << 3;
                func_8001AC28(state->parts[i].b, state->parts[i].a, sum, state->parts[i].b);
            } else {
                timer = 0;
            }
            state->timer[i] = timer;
        }
    }
    state->matrix.m[0] = obj->matrix.m[0];
    state->matrix.m[1] = obj->matrix.m[1];
    state->matrix.m[2] = obj->matrix.m[2];
    state->matrix.m[3] = obj->matrix.m[3];
    state->matrix.m[4] = obj->matrix.m[4];
    state->matrix.m[5] = obj->matrix.m[5];
    state->matrix.m[6] = obj->matrix.m[6];
    state->matrix.m[7] = obj->matrix.m[7];
    state->matrix.m[8] = obj->matrix.m[8];
    func_80017668(&obj->model, 2, &state->pos);
}
