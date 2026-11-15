// Categories: Gameplay > Entity Script | Game Engine > Entity Lifecycle
#include "game/types.h"

struct Child {
    u8 pad00[0x100];
    void *unk100;
};
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
    u8 padAE[6];
    struct Child *unkB4;
};

extern s32 func_8002DC94(struct Object *, void *, s32);
extern s32 func_8002AD58(s32, struct Object *, void *);
extern void func_8002D664(s32, struct Child *, void *, s32);
extern void func_80044B34(struct Object *, void *, void *, s32);
extern void func_800175F8(void *, s32);

/**
 * @brief Message handler of object type 11: a parent entity that forwards
 *        damage to an attached child and answers identity queries.
 *
 * @param message Handler case number.
 * @param object Entity block; +0xB4 the child pointer.
 * @param data Case payload (compared against the child in case 10).
 * @param arg3 Forwarded to spawn/child helpers.
 * @return Case 1: 0xB8. Case 10: 0 when the child matches, else 1. Case 6:
 *         0. Other cases: the sub-helper or base results.
 *
 * @par Confirmed
 * Case 2 -> func_8002DC94 (spawn). Case 10 returns child != data unless
 * child->unk100 == data (then 0). Case 9 is blocked (0) while a child
 * exists, else base. Cases 11/14 forward to the child via func_8002D664
 * when present, else base. Case 6 ticks the animation (func_80044B34 +
 * func_800175F8) when the model data is set.
 *
 * @par Likely role
 * One of the numbered entity handlers registered by func_8002AE54 (case 11
 * returns this address): a carrier/parent whose attached child absorbs the
 * hit messages, the case-10 query likely asks "is this my attacker".
 *
 * @par Unknown
 * What the child's +0x100 word holds.
 *
 * @par Callers
 * No direct callers in the call graph. func_8002AE54 returns the address
 * for object type 11, so the dispatcher reaches it indirectly.
 */
s32 func_8002DEA8(s32 message, struct Object *object, void *data, s32 arg3)
{
    s32 flag;
    register s32 cmp __asm__("$2");
    struct Child *child;
    switch (message) {
    case 1:
        return 0xB8;
    case 2:
        return func_8002DC94(object, data, arg3);
    case 10:
        child = object->unkB4;
        cmp = child != data;
        __asm__("" : "=r"(cmp) : "0"(cmp));
        flag = cmp;
        if (child != 0 && child->unk100 == data) {
            flag = 0;
        }
        return flag;
    case 9:
        if (object->unkB4 != 0) {
            return 0;
        }
        return func_8002AD58(message, object, data);
    case 11:
    case 14:
        if (object->unkB4 != 0) {
            func_8002D664(message, object->unkB4, data, arg3);
            return 0;
        }
        return func_8002AD58(message, object, data);
    case 6:
        if (object->model->unk10 != 0) {
            func_80044B34(object, object->unkA0, data, 0);
            func_800175F8(&object->model, object->unkAC);
        }
        return 0;
    default:
        return func_8002AD58(message, object, data);
    }
}
