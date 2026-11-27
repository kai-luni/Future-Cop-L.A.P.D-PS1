// Categories: Graphics > Effects > Dispatcher | Gameplay > Effects
#include "game/types.h"

struct Vec44610 {
    s32 x, y, z;
};

struct Node44610 {
    u8 pad00[0x28];
    struct Vec44610 pos;
    struct Node44610 *child;
};

struct Obj44610 {
    u8 pad00[0x0c];
    s16 unk0c;
    s16 unk0e;
    u8 pad10[2];
    s16 unk12;
    u8 pad14[2];
    s16 unk16;
    u8 pad18[4];
    struct Node44610 *parent;
    void *unk20;
    u8 pad24[4];
    struct Vec44610 pos;
    s32 unk34;
    u8 pad38[0x70];
    void *unkA8;
};

extern s32 gGuardEffectPoolMessage(s32, struct Obj44610 *, s32, s32) __asm__("func_8003C7B8");
extern s32 gHandleObjectEventMessages(s32, struct Obj44610 *, s32, s32) __asm__("func_8003C980");
extern void gBuildNodePlacement(void *, s32, struct Node44610 *, s32, s32 *, s16 *, s16 *) __asm__("func_8003DB70");
extern void gSpawnEmitterParticles(struct Obj44610 *, void *, s32) __asm__("func_80042AF4");
extern void gFreeEmitterObject(struct Obj44610 *) __asm__("func_800427E8");
s32 gHandlePeriodicEffect(s32 message, struct Obj44610 *obj, s32 arg2, s32 arg3) __asm__("func_80044610");


/**
 * @brief Effect message handler: behaviour, effect pass and debris burst.
 *
 * @param message Message id.
 * @param obj Effect object: sub-block at +0x20, two fields at +0xA8, a node parent
 *        at +0x28 and three half-words at +0x0E/+0x12/+0x34.
 * @param arg2, arg3 Forwarded to the two behaviour helpers.
 * @return 0 when func_8003C7B8 rejects the message, otherwise the result of
 *         func_8003C980.
 *
 * @par Confirmed
 * On success the object is stepped twice with func_80042AF4(obj, obj->unkA8, 1)
 * and func_80042AF4(obj, obj->unkA8, 0) - the flag selects the begin/end phase -
 * and a burst is spawned with func_8003DB70(obj->unk20, 0, obj->parent, 0,
 * &obj->unk34, &obj->unk12, &obj->unk0e). func_800427E8(obj) re-registers the node
 * afterwards.
 *
 * @par Likely role
 * The single entry point of an effect object, called once per tick.
 */
s32 gHandlePeriodicEffect(s32 message, struct Obj44610 *obj, s32 arg2, s32 arg3)
{
    register struct Node44610 *parent __asm__("$2");
    register struct Node44610 *child __asm__("$6");
    if (gGuardEffectPoolMessage(message, obj, arg2, arg3)) {
        goto ret0;
    }
    if (message == 1) goto case1;
    if (message == 0) goto case0;
    if (message == 7) goto ret0;
    if (message == 10) goto case10;
    goto dflt;
case0:
        parent = obj->parent;
        if (parent != 0 && (child = parent->child) != 0) {
            obj->pos = child->pos;
        } else {
            obj->pos = parent->pos;
        }
        gSpawnEmitterParticles(obj, obj->unkA8, 1);
        obj->unk0c = 0;
        obj->unk16 = 0;
        return 1;
case1:
    gBuildNodePlacement(obj->unk20, 0, obj->parent, 0, &obj->unk34, &obj->unk12, &obj->unk0e);
    gSpawnEmitterParticles(obj, obj->unkA8, 0);
    return 1;
case10:
    gFreeEmitterObject(obj);
    return 0;
dflt:
    return gHandleObjectEventMessages(message, obj, arg2, arg3);
ret0:
    return 0;
}
