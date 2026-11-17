// Categories: Gameplay > Entity Update | Game Engine > Entity Lifecycle | Graphics > Scene Graph
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
struct Parent { char pad[0x34]; void *child; };
struct Obj {
    char pad00[0xe]; s8 index; char pad0f[0xb]; u16 flags;
    struct Parent *parent; char pad20[0x14]; char matrix[0x30];
    char pad64[8]; void *field6c; char point[8]; char pad78[2];
    s16 field7a; s16 field7c;
};
extern s32 gMessage10Counter __asm__("D_8009F318");
extern s32 func_8003C7B8(s32, struct Obj *, s32, s32);
extern s32 func_8003C980(s32, struct Obj *, s32, s32);
extern void *func_8003D9F8(void *, u16, s8, s32, struct Parent *, void *);
extern void func_8001B148(void *, void *, void *, s32);
extern s32 func_800196C4(void);
extern u8 func_80040734(struct Obj *, void *);
extern void func_800498F0(void *);
extern void func_800181D4(struct Obj *);

/**
 * @brief Entity message handler with randomized countdown fields and a
 *        counted destruction path.
 *
 * @param message Message number (0, 1, 10 handled here).
 * @param obj Entity record; field7a/field7c are s16 countdowns, field6c an
 *            owned attachment, D_8009F318 a live-object counter.
 * @param arg2 Forwarded to the base handlers.
 * @param arg3 Forwarded to the base handlers.
 * @return Message 0: the result of func_80040734; message 10 and base
 *         hits: 0; other messages: the result of func_8003C980.
 *
 * @par Confirmed
 * func_8003C7B8 runs first (nonzero -> return 0); message 1 returns 0.
 * Message 0: resolve the parent transform via func_8003D9F8; when
 * flags & 4, func_8001B148 transforms obj->point. Each negative countdown
 * is replaced by ((func_800196C4() & 0xFFF) * -value) >> 12 - a random
 * fraction of its magnitude. Returns func_80040734(obj, matrix + 0x24).
 * Message 10: D_8009F318--, release field6c via func_800498F0 when set,
 * func_800181D4(obj), return 0. Default: func_8003C980.
 *
 * @par Likely role
 * A timed entity (the random negative-to-positive conversion reads like a
 * fuse or delay drawn once at spawn); the counter tracks live instances.
 *
 * @par Callers
 * No direct callers in the call graph; dispatched indirectly through a
 * handler table.
 */
s32 func_80040874(s32 message, struct Obj *obj, s32 arg2, s32 arg3)
{
    struct Parent *parent;
    void *child;
    char *matrix;
    if (func_8003C7B8(message, obj, arg2, arg3)) goto ret0;
    if (message == 1) goto ret0;
    if (message == 0) goto case0;
    if (message == 10) goto case10;
    goto dflt;
case0:
    parent = obj->parent;
    child = 0;
    if (parent) child = parent->child;
    matrix = func_8003D9F8(child, obj->flags, obj->index, 1, parent, obj->matrix);
    if (obj->flags & 4) func_8001B148(matrix, obj->point, obj->point, 1);
    if (obj->field7a < 0) obj->field7a = ((func_800196C4() & 0xfff) * -obj->field7a) >> 12;
    if (obj->field7c < 0) obj->field7c = ((func_800196C4() & 0xfff) * -obj->field7c) >> 12;
    return func_80040734(obj, matrix + 0x24);
case10:
    gMessage10Counter--;
    if (obj->field6c) {
        func_800498F0(obj->field6c);
        obj->field6c = 0;
    }
    func_800181D4(obj);
    goto ret0;
dflt:
    return func_8003C980(message, obj, arg2, arg3);
ret0:
    return 0;
}
