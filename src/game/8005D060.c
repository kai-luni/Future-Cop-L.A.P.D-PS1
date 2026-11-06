#include "game/types.h"

struct Vec { s32 x, y, z; };
struct Mat { s16 m[3][3]; s16 pad[3]; s32 t[3]; };
struct Vec4 { s32 x, y, z, pad; };
struct Flags { u8 pad[0x18]; u8 flags; };
struct Object { struct Flags *flags; u8 pad[0x14]; struct Mat mat; struct Vec pos; s16 scale; };
struct State { struct Vec dir; s16 a[2]; s16 b[2]; s32 time; s8 index; u8 flags; };
extern s32 D_8009FAA4;
extern s32 func_80019668(void);
extern void func_80010820(void *);
extern void func_8001006C(struct Vec *, struct Vec *, s32);
extern void func_800188D0(s32, s32, s32, struct Mat *);
extern void *func_80073500(struct Vec *, s32, s32, s32);
extern void func_8007377C(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

void func_8005D060(struct Object *obj, struct State *st)
{
    struct Vec4 v;
    struct Mat m;
    s8 other;
    void *node;
    s16 t;
    s32 interval;
    if (D_8009FAA4 > st->time) {
        other = st->index ^ 1;
        interval = (func_80019668() & 0x7f) + 0x3f;
        st->time = D_8009FAA4 + interval;
        v.x = ((st->dir.x >> 4) * obj->scale) >> 8;
        v.y = ((st->dir.y >> 4) * obj->scale) >> 8;
        v.z = ((st->dir.z >> 4) * obj->scale) >> 8;
        func_80010820(&obj->mat);
        func_8001006C(&v, &v, 1);
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
        func_800188D0(st->a[st->index], st->b[st->index], 0, &m);
        st->dir = *(struct Vec *)m.t;
        v.x = ((st->dir.x >> 4) * obj->scale) >> 8;
        v.y = ((st->dir.y >> 4) * obj->scale) >> 8;
        v.z = ((st->dir.z >> 4) * obj->scale) >> 8;
        func_80010820(&obj->mat);
        func_8001006C(&v, &v, 1);
        v.x += obj->pos.x;
        v.y += obj->pos.y;
        v.z += obj->pos.z;
        func_8007377C(node, func_80073500(&v, 0, 16, 0), 8, 1, 0x30, 0x30, 0x40, 0x200, 0x3000, 0, 0, 0, 0, 1);
        st->index = other;
        if ((func_80019668() & 7) == 7) st->flags |= 1;
    }
}
