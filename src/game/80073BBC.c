#include "game/types.h"

struct Link { u8 pad00[4]; void *owner; u8 pad08[0x2c]; void *coord; u8 disabled; };
struct Def {
    u8 pad00[0x2e]; s16 event; u8 pad30[2]; u16 flags32; u8 pad34[2];
    s16 p36, p38; u8 pad3a; u8 p3b, p3c, p3d; s16 p3e, p40, p42, p44;
    u8 pad46[2]; s16 p48; u16 flags4a;
};
struct Vec { s32 x, y, z; };
struct Obj {
    u8 pad00[0xe]; s8 kind; u8 flags0f; u8 pad10[2]; s16 angle; u8 pad14[6];
    u16 flags1a; struct Link *link; struct Def *def; u8 pad24[0x34];
    struct Vec pos; void *effect;
};
extern s32 func_8003C7B8(s32, struct Obj *, void *, s32);
extern s32 func_8003C980(s32, struct Obj *, void *, s32);
extern void *func_80017EBC(s32, s32);
extern void func_800181D4(void *);
extern void func_80070710(void *);
extern void func_80073BA8(void *);
extern void *func_80073500(void *, s32, s32, s32);
extern void *func_80073590(void *, void *, s32, s32, s32, s32);
extern void *func_8007377C(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

s32 func_80073BBC(s32 message, struct Obj *obj, void *arg2, s32 arg3)
{
    struct Def *def;
    register struct Link *link __asm__("$5");
    void *coord;
    void *owner;
    u32 a;
    u32 b;
    void *effect;
    register void *free_arg __asm__("$4");
    struct Obj *copy;
    u16 effectflags;
    register u32 flags __asm__("$4");
    register u32 initial __asm__("$3");
    register u32 tmp __asm__("$7");
    register u32 value __asm__("$2");
    s16 angle;

    if (func_8003C7B8(message, obj, arg2, arg3)) return 0;
    switch (message) {
    case 0:
        link = obj->link;
        coord = 0;
        if (link) coord = link->coord;
        def = obj->def;
        angle = obj->angle;
        flags = def->flags4a;
        initial = flags & 1;
        a = initial;
        __asm__("" : "+r"(initial));
        b = initial;
        if (flags & 0x20) {
            value = initial | 8;
            __asm__("" : "+r"(value));
            b = value;
            __asm__("" : "+r"(value));
            a = value;
        }
        if (def->flags32 & 0x80) b |= 0x10;
        tmp = (u16)a;
        if (flags & 4) tmp |= 2;
        a = tmp;
        effectflags = 2;
        if (flags & 2) effectflags = 0x2002;
        value = flags & 0x40;
        __asm__("" : "+r"(value));
        if (value) effectflags |= 0x20;
        if (coord) {
            owner = 0;
            if (link && !link->disabled) owner = link->owner;
            if (obj->flags1a & 0x100) effectflags |= 8;
            a = (u32)func_80073590(owner, coord, obj->kind, angle,
                               (def->flags4a & 8) ? 0 : def->p48, (u16)a);
            b = (u32)func_80073590(owner, coord, obj->kind, 0, def->p48, (u16)b);
        } else {
            b = (u32)func_80073500(&obj->pos, 0, def->p48, (u16)b);
            a = (u32)func_80073500(&obj->pos, angle,
                               (def->flags4a & 8) ? 0 : def->p48, (u16)a);
        }
        if (!b) {
            free_arg = (void *)a;
            if (a) {
            free_it:
                __asm__("" : "+r"(free_arg));
                func_80070710(free_arg);
            }
        failed:
            obj->effect = 0;
            goto return0;
        }
        free_arg = (void *)b;
        if (!a) goto free_it;
        if (obj->effect) func_80073BA8(obj->effect);
        effect = func_8007377C((void *)b, (void *)a, def->p36, def->p38, def->p3b, def->p3c,
                                def->p3d, def->p3e, def->p40, def->p42,
                                def->p44, 0, def->event, effectflags);
        obj->effect = effect;
        return effect != 0;
    case 2:
        copy = func_80017EBC(0x68, 1);
        if (!copy) goto return0;
        *copy = *obj;
        return (s32)copy;
    case 10:
        if (obj->effect) func_80073BA8(obj->effect);
        func_800181D4(obj);
        return 0;
    case 7:
        if (obj->def->flags4a & 0x10) obj->flags0f |= 1;
        return 1;
    case 6:
        value = obj->def->flags4a & 0x10;
        __asm__("" : "+r"(value));
        return value != 0;
    default:
        return func_8003C980(message, obj, arg2, arg3);
    case 1: case 3: case 4: case 9:
    return0:
        return 0;
    }
}
