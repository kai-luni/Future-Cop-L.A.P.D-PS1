#include "game/types.h"

typedef struct Vec { s32 x, y, z; } Vec;
typedef struct Base { u8 pad[0x28]; s32 x, y, z; } Base;
typedef struct Id { u8 pad[8]; s32 id; } Id;
typedef struct Obj {
    Base *base;
    Id *id;
    s32 saved;
    Vec pos;
    u8 flags;
} Obj;
typedef struct Vel { s16 x, y, z; } Vel;
typedef struct Part { Vec *pos; Vel *vel; s16 count; u8 pad[6]; } Part;
typedef struct Node {
    void *next;
    Obj *obj1;
    Obj *obj2;
    s16 count;
    u16 flags;
    Part *parts;
    s16 active;
    u8 pad16[2];
    s16 timer;
    u8 pad1a[4];
    u16 speedx, speedy, speedz;
    u8 pad24[4];
    s16 reset;
} Node;
extern void func_8003D0F0(Base *, s32, s32, Vec *);
extern void func_80010820(void *);
extern void func_8001006C(Vec *, Vec *, s32);
extern s32 func_800718B8(Node *, s32, s32, s32, s32);

void func_80071B40(Node *node, s32 dt)
{
    register s32 i __asm__("$16");
    register Node *n __asm__("$17") = node;
    register s32 step __asm__("$18") = dt;
    register s32 result __asm__("$19") = 0;
    register s32 x __asm__("$22");
    register s32 y __asm__("$21");
    register s32 z __asm__("$20");
    s32 j;
    register Obj *obj __asm__("$5");
    Part *part;

    obj = n->obj2;
    if (obj && obj->id && obj->base && obj->id->id != obj->saved) {
        func_8003D0F0(obj->base, *(s8 *)&obj->pos, obj->flags & 0x10, &obj->pos);
        if (n->flags & 8) {
            func_80010820((u8 *)n->obj1->base + 4);
            {
                register Obj *p __asm__("$4") = n->obj2;
                func_8001006C(&p->pos, &p->pos, 1);
            }
            n->obj2->pos.x += n->obj1->base->x;
            n->obj2->pos.y += n->obj1->base->y;
            n->obj2->pos.z += n->obj1->base->z;
        }
        n->obj2->base = 0;
        n->obj2->id = 0;
    }
    obj = n->obj1;
    if (obj && obj->id && obj->base && obj->id->id != obj->saved) {
        func_8003D0F0(obj->base, *(s8 *)&obj->pos, obj->flags & 0x10, &obj->pos);
        if (n->flags & 8) {
            func_80010820((u8 *)n->obj1->base + 4);
            {
                register Obj *p __asm__("$4") = n->obj1;
                func_8001006C(&p->pos, &p->pos, 1);
            }
            n->obj1->pos.x += n->obj1->base->x;
            n->obj1->pos.y += n->obj1->base->y;
            n->obj1->pos.z += n->obj1->base->z;
        }
        n->obj1->base = 0;
        n->obj1->id = 0;
        n->flags &= ~8;
    }
    if (!(n->flags & 0x4000)) {
        x = (n->speedx * step) >> 8;
        y = (n->speedy * step) >> 8;
        z = (n->speedz * step) >> 8;
        if (n->flags & 2) {
            n->timer -= step;
            if (n->timer < 0) {
                n->flags |= 0x8000;
                n->timer = n->reset;
            }
        }
        for (i = 0; i < n->count; i++) {
            result |= func_800718B8(n, i, x, y, z);
            if (n->active) {
                part = &n->parts[i];
                for (j = 1; j < part->count; j++) {
                    part->pos[j].x += (part->vel[j].x * step) >> 3;
                    part->pos[j].y += (part->vel[j].y * step) >> 3;
                    part->pos[j].z += (part->vel[j].z * step) >> 3;
                }
            }
        }
        {
            register s32 low __asm__("$2") = result & 0xff;
            if (!low && (n->flags & 1))
                n->flags |= 0x1000;
        }
        n->flags &= ~0x8000;
    }
}
