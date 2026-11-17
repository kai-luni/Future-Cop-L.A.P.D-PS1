// Categories: Gameplay > Object Spawning | Graphics > Animation
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk5920CModel {
    u8 unk00[0x10];
    u8 *animation;
};

struct Unk5920CType {
    u8 unk00[0xE0];
    s32 unkE0;
};

struct Unk5920CDef {
    u8 unk00[8];
    struct Vec3s32 position;
    u8 unk14[0x10];
    struct Unk5920CType *type;
};

struct Unk5920CSpawn {
    u8 unk00[8];
    struct Unk5920CDef *def;
};

struct Unk5920C {
    u8 unk00[0x24];
    u32 flags;
    struct Unk5920CModel *model;
    u8 unk2C[0x24];
    struct Vec3s32 position;
    u8 unk5C[0x16];
    u8 flags72;
    u8 unk73;
    s32 unk74;
    u8 unk78[0x28];
    u8 track[0x14];
    struct Unk5920CType *type;
};

extern void func_8002AD58(s32 kind, struct Unk5920C *object, struct Unk5920CSpawn *spawn,
                          s32 arg3);
extern void func_80013BD8(struct Unk5920CModel **model, s32 arg1, s32 arg2);
extern void func_80044988(u8 *data, u8 *track, u8 forward, s32 index);

/**
 * @brief Spawn case of the kind-0x12 companion handler: binds the type's
 *        model and animation and places the object.
 *
 * @param object Entity block.
 * @param spawn Spawn record; +8 the definition with the position and type.
 * @param arg2 Forwarded to the shared message-2 hook.
 * @return Nothing.
 *
 * @par Confirmed
 * func_8002AD58(2, object, spawn, arg2) runs first; the type pointer is
 * copied, func_80013BD8 binds the model from type->unkE0, flags72 gains
 * 0x30, func_80044988 installs the animation track (forward, index 0),
 * unk74 is cleared, the definition position is copied and the flags become
 * (flags & 0xBFFFFFFF) | 0xA20210.
 *
 * @par Player model assembly
 * When spawned by func_80057914, def->type is the owning player object.
 * Its +0xE0 model is bound as the companion instance. The companion is placed
 * at point 0 of the player's main instance and participates in the five-instance
 * draw in func_800597C0. Its anatomical identity is not established.
 *
 * @par Likely role
 * The message-2 path of func_80059ABC: the companion object starts with
 * its type's default animation and a fixed flag set (bit 0x40000000
 * cleared, likely the "inactive" marker).
 *
 * @par Unknown
 * The meaning of flag bits 0xA20210 and the flags72 pair 0x30.
 *
 * @par Callers
 * func_80059ABC.
 */
void func_8005920C(struct Unk5920C *object, struct Unk5920CSpawn *spawn, s32 arg2)
{
    struct Unk5920CDef *def;

    func_8002AD58(2, object, spawn, arg2);
    def = spawn->def;
    object->type = def->type;
    func_80013BD8(&object->model, object->type->unkE0, 0);
    object->flags72 |= 0x30;
    func_80044988(object->model->animation, object->track, 1, 0);
    object->unk74 = 0;
    object->position = def->position;
    object->flags = (object->flags & 0xBFFFFFFF) | 0xA20210;
}
