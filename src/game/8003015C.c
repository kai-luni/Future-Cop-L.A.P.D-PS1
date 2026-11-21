// Categories: Gameplay > Entity Script | Graphics > Model Renderer
#include "game/types.h"

struct Inner34 {
    u8 pad00[0x34];
    u8 *unk34;
};

struct Obj3015C {
    u8 pad00[8];
    s32 unk8;
    u8 pad0C[0x1C - 0x0C];
    s16 unk1C;
    u8 pad1E[0x24 - 0x1E];
    u32 unk24;
    struct Inner34 *unk28;
    u8 pad2C[0x70 - 0x2C];
    s16 unk70;
    u8 pad72[0xC0 - 0x72];
    u8 unkC0;
    u8 padC1[0xC4 - 0xC1];
    s16 unkC4;
    u16 unkC6;
    u32 unkC8;
    u8 padCC[0xD4 - 0xCC];
    s32 unkD4;
};

struct Arg3015C {
    u8 pad00[0x16];
    u8 unk16;
};

extern s32 gFrameTime __asm__("D_8009FAA4");

extern void func_8002FA20(struct Obj3015C *obj, struct Arg3015C *arg, s32 arg3);
extern void func_80030060(struct Obj3015C *obj, struct Arg3015C *arg);
extern s32 func_8002F8B8(u8 arg0, s16 arg1, struct Arg3015C *arg2, u8 arg3);
extern s32 gApplyRotatingPropProximityEffect(struct Obj3015C *obj, struct Arg3015C *arg) __asm__("func_8002FF6C");
extern s32 gBuildRotationMatrix(s32 a, s32 b, s32 c, void *d) __asm__("func_800188D0");
extern void gGteLoadMatrix(void *a) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *a, void *b, s32 c) __asm__("func_80010234");
extern s32 gEntityBaseHandler(s32 message, struct Obj3015C *object, struct Arg3015C *data) __asm__("func_8002AD58");
s32 gHandleRotatingPropMsg(s32 cmd, struct Obj3015C *obj, struct Arg3015C *arg, s32 arg3) __asm__("func_8003015C");


/**
 * @brief Entity message handler of the second script family.
 *
 * @param cmd Message id.
 * @param obj Entity: fields at +0x08, +0x1C, +0x24, model block at +0x28, matrix at
 *        +0x2C, output at +0x5C, frame at +0x70, values at +0xC0..+0xD4.
 * @param arg Argument block.
 * @param arg3 Forwarded to func_8002FA20.
 * @return 0 or 0xD8 for the simple messages, otherwise the delegated result.
 *
 * @par Confirmed
 * case 1 → func_8002FA20(obj, arg, arg3) and 0xD8; case 2 → func_80030060 and 0;
 * case 10 → guarded by `arg->unk16 == 0x25 && obj->unk1C >= 0` and then
 * `func_8002F8B8(obj->unkC0, obj->unkC4, arg, 0) & 0xFF`; case 11 → guarded by
 * obj->unk8 and then func_8002FF6C; case 6 → guarded by obj->unkD4 and the
 * `(obj->unkC8 & 0xF0) && (D_8009FAA4 & 0x80)` pair, then func_8002AD58. The shared
 * path builds the matrix with func_800188D0(val, 0, 0, obj + 0x2C) and fetches the
 * frame vertex from `obj->unk28->unk34 + (unk70 << 4) + 8` into obj + 0x5C.
 *
 * @par Unknown
 * The meaning of the return 0xD8 and of arg->unk16 == 0x25.
 *
 * @par Environment path
 * Message handler of one entity class: message 1 answers 0xD8, message 2 runs
 * the placement half func_8002FA20, message 10 dispatches to func_80030060 or
 * func_8002F8B8, message 11 goes to func_8002FF6C when obj->unk8 is set, and
 * message 6 advances the spin angle obj->unkC6 by arg times obj->unkD4 before
 * rebuilding the matrix and refreshing the frame vertex. Everything else is
 * forwarded to the base handler func_8002AD58. The shared path refreshes the
 * entity's own frame vertex through the GTE, but the unit draws nothing and
 * reads no view, region or camera state, so it is upstream of the renderer
 * instead of on the environment path. The class it belongs to is only visible
 * through the helpers it calls, not through a name in the file.
 */
s32 gHandleRotatingPropMsg(s32 cmd, struct Obj3015C *obj, struct Arg3015C *arg, s32 arg3)
{
    switch (cmd) {
    case 1:
        return 0xD8;
    case 2:
        func_8002FA20(obj, arg, arg3);
        return 0;
    case 10:
        if (arg->unk16 == 0x25 && obj->unk1C >= 0) {
            func_80030060(obj, arg);
            return 0;
        }
        return func_8002F8B8(obj->unkC0, obj->unkC4, arg, 0) & 0xFF;
    case 11:
        if (obj->unk8 == 0) {
            return 0;
        }
        return gApplyRotatingPropProximityEffect(obj, arg);
    case 6:
        if (obj->unkD4 != 0) {
            s32 val;
            val = (obj->unkC6 + (s32)arg * obj->unkD4) & 0xFFF;
            obj->unkC6 = val;
            gBuildRotationMatrix(val, 0, 0, (u8 *)obj + 0x2C);
            gGteLoadMatrix((u8 *)obj + 0x2C);
            gGteTransformVerticesWide(obj->unk28->unk34 + ((obj->unk70 << 4) + 8),
                          (u8 *)obj + 0x5C, 1);
            if ((obj->unkC8 & 0xF0) && (gFrameTime & 0x80)) {
                obj->unk24 |= 0x2000000;
            }
        }
        return 0;
    default:
        return gEntityBaseHandler(cmd, obj, arg);
    }
}
