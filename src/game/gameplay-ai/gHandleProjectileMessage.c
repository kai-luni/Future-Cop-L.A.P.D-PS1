// Categories: Gameplay > Entity Update | Game Engine > Entity Lifecycle | Graphics > Scene Graph
#include "game/types.h"

struct Obj;
struct Link { u8 pad[0x18]; u8 kind; };
struct Data { u8 pad0[8]; struct Link *link; u16 flags; u8 pad0e[6]; u16 id; u8 type; };
struct Sub { u8 pad[0x34]; u8 *data; };
struct Obj {
    u8 pad0[0x18]; u8 kind; u8 pad19; s16 val1a; s16 val1c;
    u8 pad1e[0xa]; struct Sub *sub;
    u8 pad2c[0x24]; u8 pos[12]; u8 mat[0x14]; s16 val70;
    u8 pad72[0x4a]; s32 valbc; u8 padc0[8]; struct Data *other;
};
extern void gInitKind15Projectile(struct Obj *, struct Data *, s32) __asm__("func_80037064");
extern void gStepProjectileMotion(struct Obj *, struct Data *) __asm__("func_800371FC");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
extern s32 gEntityBaseHandler(s32, struct Obj *, struct Data *, s32) __asm__("func_8002AD58");
extern s32 func_8002AD58_case9(s32, struct Obj *, struct Data *, s32);
extern void gApplyRadialContactDamage(void *, s32, s32, struct Data *) __asm__("func_800410BC");
s32 gHandleProjectileMessage(s32 msg, struct Obj *obj, struct Data *data, s32 arg3) __asm__("func_80037708");


/**
 * @brief Entity message handler with size query, spawn, per-frame
 *        transform, link tests and a guarded destroy.
 *
 * @param msg Message number.
 * @param obj Entity record.
 * @param data Spawn/link data record.
 * @param arg3 Forwarded to the sub-handlers.
 * @return Message 1: 0xD4; message 10: 0 or 1; all others 0 or the base
 *         handler's result.
 *
 * @par Confirmed
 * 1: returns 0xD4 (the record size). 2: func_80037064(obj, data, arg3).
 * 6: func_800371FC(obj, data), func_80010750(obj + 0x2C) loads the matrix,
 * func_80010234 transforms one frame vertex into obj->mat.
 * 10: returns 0 when obj->other->id == data->id or data->type == 15,
 * otherwise 1 (destroy veto). 11: obj->val1c = -1. 9: returns 0 when
 * data->link is null or (data->flags & 1 and link->kind == obj->kind),
 * else func_8002AD58_case9. 4: func_800410BC(obj->pos, val1a, valbc,
 * obj->other), *(obj + 0x68) = valbc >> 1, then the base handler.
 * 5: returns 0. Default: func_8002AD58.
 *
 * @par Callers
 * No direct callers in the call graph; dispatched indirectly through a
 * handler table.
 *
 * @par Environment path
 * Message handler of the kind-0xF projectile class; func_8005ECB8 and
 * func_8005EE04 store its address in the spawn requests they queue, so it has
 * no direct callers. Message 1 returns the 0xD4 record size, message 2 runs
 * the spawn setup func_80037064, message 6 runs the mover tick func_800371FC
 * and then refreshes the frame vertex from the matrix at +0x2C, and message 4
 * sets off the radial blast func_800410BC at the object position before
 * falling through to the base handler. Message 10 returns 0 when the owner's
 * id matches the data id or the data type is 15, message 11 writes -1 to
 * +0x1C, message 9 is filtered on the data link, and every other message goes
 * to func_8002AD58.
 */
s32 gHandleProjectileMessage(s32 msg, struct Obj *obj, struct Data *data, s32 arg3)
{
    switch (msg) {
    case 1:
        return 0xD4;
    case 2:
        gInitKind15Projectile(obj, data, arg3);
        return 0;
    case 6:
        gStepProjectileMotion(obj, data);
        gGteLoadMatrix((u8 *)obj + 0x2c);
        gGteTransformVerticesWide(obj->sub->data + ((obj->val70 << 4) + 8), obj->mat, 1);
        return 0;
    case 10:
        if (obj->other->id == data->id || data->type == 15)
            return 0;
        return 1;
    case 11:
        obj->val1c = -1;
        return 0;
    case 9:
        if (!data->link)
            return 0;
        if ((data->flags & 1) && data->link->kind == obj->kind)
            return 0;
        func_8002AD58_case9(msg, obj, data, arg3);
        return 0;
    case 4:
        gApplyRadialContactDamage(obj->pos, obj->val1a, obj->valbc, obj->other);
        *(s32 *)((u8 *)obj + 0x68) = obj->valbc >> 1;
        gEntityBaseHandler(msg, obj, data, arg3);
        return 0;
    case 5:
        return 0;
    default:
        return gEntityBaseHandler(msg, obj, data, arg3);
    }
}
