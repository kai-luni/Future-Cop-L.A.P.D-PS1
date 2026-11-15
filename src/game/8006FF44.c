// Categories: Gameplay > Entity Update | Game Engine > Entity Lifecycle | Graphics > Scene Graph
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef int s32;
struct Parent { u8 pad[0x34]; void *child; };
struct Obj { u8 pad00[0xE]; s8 index; u8 pad0f[0xB]; u16 flags; struct Parent *parent; u8 pad20[0x14]; u8 matrix[0x30]; u8 pad64[0x30]; u8 point[8]; };
extern s32 func_8003C7B8(s32, struct Obj *, s32, s32);
extern s32 func_8003C980(s32, struct Obj *, s32, s32);
extern void *func_8003D9F8(void *, u16, s8, s32, struct Parent *, void *);
extern void func_8001B148(void *, void *, void *, s32);
extern void func_8006F898(struct Obj *);
extern void func_800181D4(struct Obj *);
/**
 * @brief Entity message handler: per-frame transform refresh, a status
 *        query and destruction, on top of the shared base handler.
 *
 * @param message Message number (0, 1, 6, 10 handled here).
 * @param obj Entity record (flags at +0x1A, parent at +0x1C, matrix at
 *            +0x34, point at +0x94 in this unit's layout).
 * @param arg2 Forwarded to the base handlers.
 * @param arg3 Forwarded to the base handlers.
 * @return 1 when the entity stays alive (messages 0 and 6), 0 otherwise.
 *
 * @par Confirmed
 * func_8003C7B8(message, obj, arg2, arg3) runs first; a nonzero result
 * returns 0. Message 1 returns 0. Message 0: when flags & 4,
 * func_8003D9F8(child, flags, index, 1, parent, obj->matrix) resolves the
 * parent transform and func_8001B148 transforms obj->point in place; then
 * func_8006F898(obj) runs and the handler returns 1. Message 6 returns 1.
 * Message 10 calls func_800181D4(obj) (release) and returns 0. Any other
 * message goes to func_8003C980.
 *
 * @par Callers
 * No direct callers in the call graph; dispatched indirectly through a
 * handler table.
 */
s32 func_8006FF44(s32 message, struct Obj *obj, s32 arg2, s32 arg3)
{
    struct Parent *parent;
    void *child;
    if (func_8003C7B8(message, obj, arg2, arg3)) goto ret0;
    if (message == 1) goto ret0;
    if (message == 0) goto case0;
    if (message == 6) goto ret1;
    if (message == 10) goto case10;
    goto dflt;
case0:
    if (obj->flags & 4) {
        parent = obj->parent;
        child = 0;
        if (parent) child = parent->child;
        func_8001B148(func_8003D9F8(child, obj->flags, obj->index, 1, parent, obj->matrix), obj->point, obj->point, 1);
    }
    func_8006F898(obj);
ret1:
    return 1;
case10:
    func_800181D4(obj);
    goto ret0;
dflt:
    return func_8003C980(message, obj, arg2, arg3);
ret0:
    return 0;
}
