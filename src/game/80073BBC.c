// Categories: Graphics > Effects > Ribbon Beam | Game Engine > Object Behaviour > Message Handler | Graphics > Scene Graph > Node Allocation
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
extern void *gAllocMemory(s32, s32) __asm__("func_80017EBC");
extern void gFreeBlock(void *) __asm__("func_800181D4");
extern void func_80070710(void *);
extern void func_80073BA8(void *);
extern void *func_80073500(void *, s32, s32, s32);
extern void *func_80073590(void *, void *, s32, s32, s32, s32);
extern void *func_8007377C(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);

/**
 * @brief Message handler of a beam effect object: creates the ribbon between two
 *        attachment points, clones, queries and destroys it.
 *
 * @param message Message code: 0 create, 2 clone, 6/7 query and mark, 10 destroy,
 *                1/3/4/9 no-op, anything else is forwarded.
 * @param obj     Object: kind byte +0x0E, flag byte +0x0F, angle +0x12, flags at
 *                +0x1A, link +0x1C, definition +0x20, position +0x58, effect +0x64.
 * @param arg2    Event/argument forwarded to func_8003C7B8 and func_8003C980.
 * @param arg3    Fourth argument of those two calls.
 *
 * @return 1 when an effect was created or when message 7/6 reported the flag,
 *         the clone pointer for message 2, 0 otherwise.
 *
 * @par Confirmed control flow
 * 1. func_8003C7B8(message, obj, arg2, arg3) runs first and, when it returns
 *    non-zero, the message is swallowed and 0 is returned.
 * 2. Message 0 builds two scene nodes from the definition flags at +0x4A: node b
 *    without an angle, node a with obj->angle and, unless flag 8 is set, the
 *    half-word at +0x48. With a link coordinate the owner pointer and the disabled
 *    byte of the link decide between func_80073590(owner, coord, kind, ...) and
 *    func_80073500(&obj->pos, ...). Effect flags start at 2 (0x2002 when flag 2 is
 *    set), gain 0x20 for flag 0x40 and 8 for obj flag 0x100; the parameter word handed
 *    to the node functions starts at bit 0 of the flags at +0x4A, gains 8 for bit 0x20
 *    and 2 for bit 4, and bit 0x80 of the flags at +0x32 adds 0x10 to the second
 *    node's parameter word.
 * 3. Allocation failures are unwound through func_80070710, obj->effect is cleared
 *    and 0 is returned; on success an existing effect is marked for removal with
 *    func_80073BA8 and func_8007377C(b, a, p36, p38, p3B, p3C, p3D, p3E, p40, p42,
 *    p44, 0, event, flags) builds the ribbon from the two nodes, the two counts, the
 *    three colour bytes, the divisor, three half-words, the event id at +0x2E and
 *    the flag word.
 * 4. Message 2 allocates 0x68 bytes and copies the object into it, message 10
 *    releases the effect and the object through func_80073BA8 and func_800181D4,
 *    message 7 sets bit 0 of +0x0F when definition flag 0x10 is set, message 6
 *    returns whether that flag is set.
 *
 * @par Likely role
 * The per-object handler of a two-point energy beam, lightning arc or tracer: the
 * two nodes are the endpoints, the ribbon node holds the segment count, the colours
 * and the fade divisor.
 *
 * @par Unknown
 * - Which visual parameter p48, p36 and p38 stand for; they reach func_8007377C as
 *   count and half-word arguments, so they are probably segment count and width.
 *
 * @par Callers
 * None in analysis/edges.json: no direct call exists, so the handler is reached
 * through an indirect call from an object table.
 */
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
        copy = gAllocMemory(0x68, 1);
        if (!copy) goto return0;
        *copy = *obj;
        return (s32)copy;
    case 10:
        if (obj->effect) func_80073BA8(obj->effect);
        gFreeBlock(obj);
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
