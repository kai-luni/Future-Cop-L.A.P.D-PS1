// Categories: Graphics > Effects > Particles | Graphics > Model Renderer
#include "game/types.h"

struct Vec { s32 x, y, z; };
struct Mat { s16 m[3][3]; s16 pad[3]; s32 t[3]; };
struct Vec4 { s32 x, y, z, pad; };
struct Flags { u8 pad[0x18]; u8 flags; };
struct Object { struct Flags *flags; u8 pad[0x14]; struct Mat mat; struct Vec pos; s16 scale; };
struct State { struct Vec dir; s16 a[2]; s16 b[2]; s32 time; s8 index; u8 flags; };
extern s32 gDiscShadeByte __asm__("D_8009FAA4");
extern s32 func_80019668(void);
extern void gGteLoadRotation(void *) __asm__("func_80010820");
extern void gGteTransformWords(struct Vec *, struct Vec *, s32) __asm__("func_8001006C");
extern void gBuildRotationMatrix(s32, s32, s32, struct Mat *) __asm__("func_800188D0");
extern void *func_80073500(struct Vec *, s32, s32, s32);
extern void func_8007377C(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

/**
 * @brief Spawns and animates the parts of a particle/smoke object at a time
 *        interval.
 *
 * @param obj Object: transform matrix at +0x18, position at +0x34, scale at +0x40.
 * @param st  Spawn state: direction, two accumulator pairs, the next time and an
 *            index that selects one of two slots.
 * @return Nothing.
 *
 * @par Confirmed flow
 * Only when the global tick D_8009FAA4 has passed st->time:
 * 1. The other slot is selected (st->index ^ 1) and the next time is
 *    D_8009FAA4 + (rand & 0x7F) + 0x3F, i.e. 63..190 ticks later.
 * 2. The direction is scaled by the object's scale (>> 4, >> 8) into a vector.
 * 3. func_80010820(&obj->mat) loads the matrix, func_800188D0 builds a rotated
 *    matrix, func_80073500 attaches a node and func_8007377C emits it with a
 *    long argument list (position, colour, size, ...).
 *
 * @par Likely role
 * The exhaust/smoke emitter: a new puff every 63..190 ticks, alternating between
 * two slots so the two streams are phase-shifted.
 *
 * @par Callers
 * The effect update loop of the entity renderer.
 */
void func_8005D060(struct Object *obj, struct State *st)
{
    struct Vec4 v;
    struct Mat m;
    s8 other;
    void *node;
    s16 t;
    s32 interval;
    if (gDiscShadeByte > st->time) {
        other = st->index ^ 1;
        interval = (func_80019668() & 0x7f) + 0x3f;
        st->time = gDiscShadeByte + interval;
        v.x = ((st->dir.x >> 4) * obj->scale) >> 8;
        v.y = ((st->dir.y >> 4) * obj->scale) >> 8;
        v.z = ((st->dir.z >> 4) * obj->scale) >> 8;
        gGteLoadRotation(&obj->mat);
        gGteTransformWords(&v, &v, 1);
        v.x += obj->pos.x;
        v.y += obj->pos.y;
        v.z += obj->pos.z;
        node = func_80073500(&v, 0, 16, 0);
        st->a[st->index] = st->a[other] + (func_80019668() >> 22);
        st->b[st->index] = st->b[other] + (func_80019668() >> 22);
        if (obj->flags->flags & 4) {
            t = 0x400 - st->a[st->index];
            if (t < 0) st->a[st->index] = t + 0x400;
            else {
                t = st->a[st->index] + 0x400;
                if (t < 0) st->a[st->index] = -0x400 - t;
            }
        }
        if (obj->flags->flags & 1) {
            t = 0x200 - st->b[st->index];
            if (t < 0) st->b[st->index] = t + 0x200;
            else {
                t = st->b[st->index] + 0x200;
                if (t < 0) st->b[st->index] = -0x200 - t;
            }
        }
        gBuildRotationMatrix(st->a[st->index], st->b[st->index], 0, &m);
        st->dir = *(struct Vec *)m.t;
        v.x = ((st->dir.x >> 4) * obj->scale) >> 8;
        v.y = ((st->dir.y >> 4) * obj->scale) >> 8;
        v.z = ((st->dir.z >> 4) * obj->scale) >> 8;
        gGteLoadRotation(&obj->mat);
        gGteTransformWords(&v, &v, 1);
        v.x += obj->pos.x;
        v.y += obj->pos.y;
        v.z += obj->pos.z;
        func_8007377C(node, func_80073500(&v, 0, 16, 0), 8, 1, 0x30, 0x30, 0x40, 0x200, 0x3000, 0, 0, 0, 0, 1);
        st->index = other;
        if ((func_80019668() & 7) == 7) st->flags |= 1;
    }
}
