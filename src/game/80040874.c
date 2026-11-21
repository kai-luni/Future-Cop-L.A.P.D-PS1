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
extern s32 gLightObjectCount __asm__("D_8009F318");
extern s32 gGuardEffectPoolMessage(s32, struct Obj *, s32, s32) __asm__("func_8003C7B8");
extern s32 gHandleObjectEventMessages(s32, struct Obj *, s32, s32) __asm__("func_8003C980");
extern void *gResolveNodeTransform(void *, u16, s8, s32, struct Parent *, void *) __asm__("func_8003D9F8");
extern void func_8001B148(void *, void *, void *, s32);
extern s32 func_800196C4(void);
extern u8 gSpawnLightForEffect(struct Obj *, void *) __asm__("func_80040734");
extern void func_800498F0(void *);
extern void gFreeBlock(struct Obj *) __asm__("func_800181D4");
s32 gHandleLightObjectMsg(s32 message, struct Obj *obj, s32 arg2, s32 arg3) __asm__("func_80040874");


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
 *
 * @par Environment path
 * Message handler of spawn kind 0x5D objects, stored at +0x24 by their
 * constructor func_80040E60. After the shared pre-handler func_8003C7B8
 * (non-zero returns 0) and with message 1 ignored, message 0 resolves the
 * world matrix from the parent with gResolveNodeTransform (func_8003D9F8),
 * transforms the point at +0x70 with func_8001B148 when flag bit 2 is set,
 * replaces negative s16 values at +0x7A/+0x7C by a random fraction of their
 * magnitude, and returns func_80040734, which requests a coloured map light
 * through func_8005A6A4 and stores it at +0x6C. Message 10 decrements the
 * live counter D_8009F318, unlinks the light at +0x6C with func_800498F0 and
 * frees the object; other messages go to func_8003C980. It is invoked
 * indirectly through the +0x24 handler pointer whenever such a light object
 * is created or destroyed.
 */
s32 gHandleLightObjectMsg(s32 message, struct Obj *obj, s32 arg2, s32 arg3)
{
    struct Parent *parent;
    void *child;
    char *matrix;
    if (gGuardEffectPoolMessage(message, obj, arg2, arg3)) goto ret0;
    if (message == 1) goto ret0;
    if (message == 0) goto case0;
    if (message == 10) goto case10;
    goto dflt;
case0:
    parent = obj->parent;
    child = 0;
    if (parent) child = parent->child;
    matrix = gResolveNodeTransform(child, obj->flags, obj->index, 1, parent, obj->matrix);
    if (obj->flags & 4) func_8001B148(matrix, obj->point, obj->point, 1);
    if (obj->field7a < 0) obj->field7a = ((func_800196C4() & 0xfff) * -obj->field7a) >> 12;
    if (obj->field7c < 0) obj->field7c = ((func_800196C4() & 0xfff) * -obj->field7c) >> 12;
    return gSpawnLightForEffect(obj, matrix + 0x24);
case10:
    gLightObjectCount--;
    if (obj->field6c) {
        func_800498F0(obj->field6c);
        obj->field6c = 0;
    }
    gFreeBlock(obj);
    goto ret0;
dflt:
    return gHandleObjectEventMessages(message, obj, arg2, arg3);
ret0:
    return 0;
}
