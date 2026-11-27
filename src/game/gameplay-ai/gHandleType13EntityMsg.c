// Categories: Gameplay > Entity Script | Graphics > Animation
#include "game/types.h"

struct Model {
    u8 pad00[0x10];
    void *unk10;
};
struct Object {
    u8 pad00[0x28];
    struct Model *model;
    u8 pad2C[0x74];
    u8 unkA0[0xC];
    s16 unkAC;
};
struct Data {
    u8 pad00[0x16];
    u8 unk16;
};

extern s32 gInitGroundObject(struct Object *, void *, s32) __asm__("func_8002DAA4");
extern s32 gEntityBaseHandler(s32, struct Object *, struct Data *, s32) __asm__("func_8002AD58");
extern void gAdvanceAnimTrack(struct Object *, void *, void *, s32) __asm__("func_80044B34");
extern void gSetModelFrame(void *, s32) __asm__("func_800175F8");
s32 gHandleType13EntityMsg(s32 message, struct Object *object, struct Data *data, s32 arg3) __asm__("func_8002DB9C");


/**
 * @brief Message handler of object type 13: spawn, animation tick and a
 *        data-byte query, everything else through the base handler.
 *
 * @param message Handler case number.
 * @param object Entity block; +0x28 the model, +0xA0 the animation track,
 *        +0xAC the frame word.
 * @param data Case payload; +0x16 queried by case 10.
 * @param arg3 Forwarded to the spawn helper.
 * @return Case 1: 0xB4. Case 10: data->unk16 == 1. Cases 9/14: 0. Other
 *         cases: the sub-helper or base results.
 *
 * @par Confirmed
 * Case 2 -> func_8002DAA4 (spawn); case 6 advances the animation via
 * func_80044B34 and commits the frame with func_800175F8 when the model's
 * +0x10 word is non-zero, then falls through to the base handler; cases
 * 9/14 return 0 (damage suppressed); unhandled cases go to func_8002AD58.
 *
 * @par Likely role
 * One of the numbered entity handlers registered by func_8002AE54 (case 13
 * returns this address): a simple animated object whose case-10 query
 * tests a payload byte (likely a team or trigger id).
 *
 * @par Unknown
 * What the type id 0xB4 selects.
 *
 * @par Callers
 * No direct callers in the call graph. func_8002AE54 returns the address
 * for object type 13, so the dispatcher reaches it indirectly.
 */
s32 gHandleType13EntityMsg(s32 message, struct Object *object, struct Data *data, s32 arg3)
{
    switch (message) {
    case 1:
        return 0xB4;
    case 2:
        return gInitGroundObject(object, data, arg3);
    case 9:
    case 14:
        return 0;
    case 10:
        return data->unk16 == 1;
    case 6:
        if (object->model->unk10 != 0) {
            gAdvanceAnimTrack(object, object->unkA0, data, 0);
            gSetModelFrame(&object->model, object->unkAC);
        }
    default:
        return gEntityBaseHandler(message, object, data, arg3);
    }
}
