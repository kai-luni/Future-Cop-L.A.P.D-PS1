// Categories: Gameplay > Entity Update | Game Engine > Entity Lifecycle | Graphics > Scene Graph
typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef int s32;
struct Parent { u8 pad[0x34]; void *child; };
struct Obj { u8 pad00[0xE]; s8 index; u8 pad0f[0xB]; u16 flags; struct Parent *parent; u8 pad20[0x14]; u8 matrix[0x30]; u8 pad64[0x30]; u8 point[8]; };
extern s32 gGuardEffectPoolMessage(s32, struct Obj *, s32, s32) __asm__("func_8003C7B8");
extern s32 gHandleObjectEventMessages(s32, struct Obj *, s32, s32) __asm__("func_8003C980");
extern void *gResolveNodeTransform(void *, u16, s8, s32, struct Parent *, void *) __asm__("func_8003D9F8");
extern void func_8001B148(void *, void *, void *, s32);
extern void gInitGlowColorChannelSlopes(struct Obj *) __asm__("func_8006F898");
s32 gHandleActor59Msg(s32 message, struct Obj *obj, s32 arg2, s32 arg3) __asm__("func_8006FF44");

extern void gFreeBlock(struct Obj *) __asm__("func_800181D4");
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
 *
 * @par Environment path
 * Message handler of the 0x9C-byte kind-0x59 object built by func_8007043C,
 * which stores this function's address in the object's +0x24 word
 * (8007043C.c:151); there is no direct caller. After the shared pre-handler
 * func_8003C7B8 (non-zero result returns 0), message 0 rotates the s16 triple
 * at +0x94 in place by the parent node transform (gResolveNodeTransform
 * func_8003D9F8 + func_8001B148, only when the +0x1A flags have bit 4) and
 * calls func_8006F898 to turn the byte ranges at +0x6C..+0x77 and durations
 * +0x68/+0x6A into per-tick slopes, returning 1; message 6 returns 1, message
 * 1 returns 0, message 10 frees the object and anything else goes to
 * func_8003C980. The +0x94 triple is the velocity that func_8007043C seeds
 * from a scaled matrix row and that func_80070048 integrates, so message 0
 * turns a local launch direction into world space.
 */
s32 gHandleActor59Msg(s32 message, struct Obj *obj, s32 arg2, s32 arg3)
{
    struct Parent *parent;
    void *child;
    if (gGuardEffectPoolMessage(message, obj, arg2, arg3)) goto ret0;
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
        func_8001B148(gResolveNodeTransform(child, obj->flags, obj->index, 1, parent, obj->matrix), obj->point, obj->point, 1);
    }
    gInitGlowColorChannelSlopes(obj);
ret1:
    return 1;
case10:
    gFreeBlock(obj);
    goto ret0;
dflt:
    return gHandleObjectEventMessages(message, obj, arg2, arg3);
ret0:
    return 0;
}
