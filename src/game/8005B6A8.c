#include "game/types.h"

struct Vec { s32 x, y, z; };
struct Tag { u8 pad[4]; u8 kind; };
struct Sphere { struct Sphere *next; struct Vec pos; s16 radius; s8 state; u8 pad13[5]; u16 flags; };
struct Target {
    u8 pad00[4]; void (*callback)(s32, struct Target *, void *, s32);
    s32 active; u8 pad0c[8]; u16 id; u8 kind; u8 pad17; u8 team;
    u8 pad19[11]; u32 flags; u8 pad28[0x34]; struct Vec pos;
};
struct Source {
    u8 pad00[4]; struct Target *target; struct Target *detail;
    u8 pad0c[12]; s32 timer; u16 value; u8 column; u8 pad1f[11];
    u8 flags; u8 pad2b; struct Vec pos; s16 power;
};
struct Model { u8 pad00[4]; struct Vec v0; struct Vec v1; struct Vec v2; };
struct Link { u8 pad00[0x34]; struct Model *model; u8 pad38[0x10]; struct Source **source; };
struct Owner {
    u8 pad00[15]; u8 flag0f; u8 flag10; u8 pad11[11]; struct Link *link;
    struct Tag *tag; void (*callback)(s32, struct Owner *, void *, s32);
    u8 pad28[0x40]; struct Sphere *spheres; u8 pad6c[0x22]; u16 flag8e;
    u8 pad90[10]; s16 x, y, z;
};
struct Message { s32 value; u8 pad04[12]; struct Vec vec; u8 pad1c[0x2d]; u8 type; u8 pad4a[6]; };
struct Candidates { s32 count; s32 pad; struct Target *items[1]; };
extern struct Candidates D_800A8F90;
extern s16 D_8009F7F0[3];
extern s16 D_8009F7F2, D_8009F7F4;
extern void func_8005B5A0(struct Source *, struct Target *, struct Sphere *);
extern s32 func_800684AC(struct Vec *, struct Vec *, struct Vec *, s32, s32);
extern s32 func_8003BF50(struct Target *);
extern s32 func_800402DC(void *, u8, struct Message *, s32, s32);
extern void func_8001A180(struct Vec *);
extern s32 func_8001AB00(struct Vec *, struct Vec *);
extern u32 func_80095900(s32);
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))

void func_8005B6A8(struct Owner *owner, struct Source *source)
{
    struct Vec diff;
    struct Message message;
    struct Vec *scratch, *first;
    register struct Sphere *sphere __asm__("$16");
    struct Sphere *prev;
    struct Target *target;
    s32 i;
    if (owner->tag->kind != 0x57) return;
    sphere = owner->spheres;
    prev = 0;
    while (sphere != 0) {
        if (!(source->flags & 8) && sphere->state <= 0) {
            func_8005B5A0(source, source->target, sphere);
            for (i = 0; i < D_800A8F90.count; i++) {
                target = D_800A8F90.items[i];
                if (target->active > 0) {
                    if (source->detail->id != target->id && !(target->flags & 0x80) &&
                        source->detail->team != target->team &&
                        target->kind != 2 && target->kind != 7 && target->kind != 15) {
                    } else if (target->kind != 25) continue;
                    diff.x = target->pos.x - source->detail->pos.x;
                    diff.y = target->pos.y - source->detail->pos.y;
                    diff.z = target->pos.z - source->detail->pos.z;
                    if (ABS(diff.x) <= 65535 && ABS(diff.y) <= 65535 && ABS(diff.z) <= 65535)
                        func_8005B5A0(source, target, sphere);
                }
            }
        }
        if (prev) {
            if (func_800684AC(&prev->pos, &sphere->pos, &message.vec, 0, 1)) goto hit;
            goto next_sphere;
        }
        goto first_sphere;
    hit:
        sphere->flags |= 1;
        message.type = 5;
        message.value = sphere->radius << 4;
        func_800402DC(0, source->column, &message, func_8003BF50(source->detail), 0);
        return;
    first_sphere:
        if (func_800684AC(&source->pos, &sphere->pos, &message.vec, 0, 1)) goto hit;
    next_sphere:
        prev = sphere;
        sphere = sphere->next;
    }
}

