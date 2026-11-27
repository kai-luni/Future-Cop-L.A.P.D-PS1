// Categories: Gameplay > Entity Script | Graphics > Model Renderer
#include "game/types.h"

struct Src {
    u8 pad00[0x24];
    s16 x24;
    s16 x26;
    s16 x28;
    u8 x2A;
};

struct Ent28 { u8 pad[0x34]; u8 *p34; };

struct Obj {
    u8 pad00[0x28];
    struct Ent28 *p28;
    u8 pad2C[0x24];
    s32 v50;
    s32 v54;
    u8 pad58[0x18];
    s16 v70;
    u8 pad72[0x2E];
    u8 vA0;
};

struct Arg { u8 pad[8]; struct Src *src; };

extern s32 gEntityBaseHandler(s32 a0, struct Obj *a1, struct Arg *a2, s32 a3) __asm__("func_8002AD58");
extern s32 gBuildRotationMatrix(s32 a, s32 b, s32 c, void *d) __asm__("func_800188D0");
extern s32 gGetGroundHeight(void *a, s32 b, s32 c, s32 d) __asm__("func_80066F78");
extern void gRegisterGroundObject(struct Obj *a, s32 b) __asm__("func_8006314C");
extern void gRemoveGroundKeyEntry(struct Obj *a) __asm__("func_80063194");
extern void gGteLoadMatrix(void *a) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *a, void *b, s32 c) __asm__("func_80010234");
s32 gHandlePlacedPropMsg(s32 msg, struct Obj *obj, struct Arg *arg, s32 a3) __asm__("func_80041C6C");


/**
 * @brief Entity message handler: dispatches by message id, then refreshes the
 *        frame vertex.
 *
 * @param msg Message id.
 * @param obj Entity.
 * @param arg Argument block whose source record supplies two half-words.
 * @param a3 Forwarded to func_8002AD58.
 * @return 0xA4 for message 1, 0 for messages 2/5/6/11, otherwise the value of
 *         func_8002AD58.
 *
 * @par Confirmed
 * Messages 3, 4 and 7..10 are forwarded unchanged to func_8002AD58(msg, obj, arg,
 * a3). The handled path builds the matrix with
 * func_800188D0(src->x24, src->x26, 0, obj + 0x2C), queries func_80066F78(obj +
 * 0x50), calls func_8006314C(obj, obj->vA0), loads the matrix and transforms one
 * vertex from `obj->p28->p34 + (v70 << 4) + 8` into obj + 0x5C, then
 * func_80063194(obj).
 *
 * @par Unknown
 * The meaning of the return value 0xA4 (distance, state or handle).
 *
 * @par Environment path
 * Message handler of another entity class: message 1 answers 0xA4, message 2
 * rebuilds the orientation from the descriptor's two angles with
 * func_800188D0, snaps the object onto the terrain with func_80066F78, stores
 * the resulting height plus the state byte at +0xA0, and refreshes the frame
 * vertex through func_80010750 and func_80010234; message 5 releases the
 * object through func_80063194 before forwarding, and messages 3, 4 and 7 to
 * 10 go straight to func_8002AD58. Like the other handlers of this family it
 * only prepares the model input and reads no view, region or camera state, so
 * it is not part of the environment render pass itself.
 */
s32 gHandlePlacedPropMsg(s32 msg, struct Obj *obj, struct Arg *arg, s32 a3)
{
    struct Src *src;
    s32 r;

    switch (msg) {
    case 1:
        return 0xA4;
    case 2:
        gEntityBaseHandler(2, obj, arg, a3);
        src = arg->src;
        gBuildRotationMatrix(src->x24, src->x26, 0, (u8 *)obj + 0x2C);
        r = gGetGroundHeight((u8 *)obj + 0x50, 0, 0, 0);
        obj->v54 = r + (src->x28 << 4);
        obj->vA0 = src->x2A;
        gRegisterGroundObject(obj, obj->vA0);
        gGteLoadMatrix((u8 *)obj + 0x2C);
        gGteTransformVerticesWide(obj->p28->p34 + ((obj->v70 << 4) + 8), (u8 *)obj + 0x5C, 1);
        return 0;
    case 5:
        gRemoveGroundKeyEntry(obj);
        gEntityBaseHandler(msg, obj, arg, a3);
        return 0;
    case 6:
        return 0;
    case 11:
        gEntityBaseHandler(msg, obj, arg, a3);
        return 0;
    case 3:
    case 4:
    case 7:
    case 8:
    case 9:
    case 10:
    default:
        return gEntityBaseHandler(msg, obj, arg, a3);
    }
}
