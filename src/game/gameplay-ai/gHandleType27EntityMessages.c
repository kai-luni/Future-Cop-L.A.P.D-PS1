// Categories: Gameplay > Entity Script | Graphics > Animation
#include "game/types.h"

struct Model { u8 pad[0x10]; void *data; };
struct Object {
    u8 pad00[0x19];
    u8 flags19;
    u8 pad1A[2];
    s16 state1C;
    u8 pad1E[2];
    u8 column;
    u8 pad21[3];
    u32 flags;
    struct Model *model;
    u8 pad2C[0xC8-0x2C];
    u8 anim[0x14];
    u8 state;
    u8 padDD[0x111-0xDD];
    u8 value;
    u8 anim_index;
};

extern s32 gEntityBaseHandler(s32, struct Object *, void *) __asm__("func_8002AD58");
extern s32 gSetupType27Entity(struct Object *, void *) __asm__("func_8002DFC4");
extern s32 gTickActor(struct Object *, void *) __asm__("func_8002E630");
extern s32 gSubmitModelWithUvOffset(struct Object *, s32) __asm__("func_8002ACB4");
extern void *gSelectAnimationClip(void *, s32, s32, u8) __asm__("func_800448D8");
extern void gSpawnEntitySlotTypes(u8, struct Object *, s32, u16, s32) __asm__("func_8003B898");
s32 gHandleType27EntityMessages(s32 message, struct Object *object, void *data) __asm__("func_8002ED7C");


/**
 * @brief Message handler of object type 27: spawn, attack tick, frame draw
 *        and an animation-trigger case.
 *
 * @param message Handler case (1 type, 2 spawn, 3 draw, 4 trigger, 6 tick,
 *        16 value query).
 * @param object Entity block; +0x1C a state word, +0x20 the column,
 *        +0xC8 the animation track, +0xDC the state byte, +0x110 the frame
 *        value, +0x111 the animation index.
 * @param data Case payload.
 * @return Case 1: 0x114. Case 16: the frame value. Case 4: 0. Other cases:
 *         the sub-helper or base results.
 *
 * @par Confirmed
 * Case 2 runs the base hook then func_8002DFC4; case 6 -> func_8002E630
 * (the attack dispatcher tick); case 3 draws with func_8002ACB4(object,
 * object->value); case 4 (when state != 4/5 and the model data and anim
 * index are valid) starts the animation via func_800448D8(anim, index, 1,
 * 1), sets state1C = 1, state = 4, flag bit 0, and registers map occupancy
 * through func_8003B898(column, object, 0, (flags19 >> 3) & 0x1E, 0).
 * Unhandled cases fall through to func_8002AD58.
 *
 * @par Likely role
 * One of the numbered entity handlers registered by func_8002AE54 (case 27
 * returns this address): an attackable object whose case 4 plays a reaction
 * animation and marks its map cell.
 *
 * @par Unknown
 * What the type id 0x114 selects and the state 4/5 semantics.
 *
 * @par Callers
 * No direct callers in the call graph. func_8002AE54 returns the address
 * for object type 27, so the dispatcher reaches it indirectly.
 */
s32 gHandleType27EntityMessages(s32 message, struct Object *object, void *data)
{
    switch (message) {
    case 1:
        return 0x114;
    case 2:
        gEntityBaseHandler(message, object, data);
        return gSetupType27Entity(object, data);
    case 6:
        return gTickActor(object, data);
    case 3:
        return gSubmitModelWithUvOffset(object, object->value);
    case 16:
        return object->value;
    case 4:
        if (object->state != 4) {
            if (object->state == 5) return gEntityBaseHandler(message, object, data);
            if (object->model->data == 0 || object->anim_index == 255)
                return gEntityBaseHandler(message, object, data);
            gSelectAnimationClip(object->anim, object->anim_index, 1, 1);
            object->state1C = 1;
            object->state = 4;
            object->flags |= 1;
            gSpawnEntitySlotTypes(object->column, object, 0, (object->flags19 >> 3) & 0x1e, 0);
        }
        return 0;
    default:
        return gEntityBaseHandler(message, object, data);
    }
}
