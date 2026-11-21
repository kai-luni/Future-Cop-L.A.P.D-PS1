// Categories: Physics > Motion | Graphics > Scene Graph
#include "game/types.h"

struct Entity {
    struct Entity *next;
    u8 pad04[0x0B];
    u8 flags;
    u8 pad10[2];
    s16 angle;
    u8 pad14[0x44];
    s32 x, y, z;
    u8 pad64[2];
    s16 rotation;
    u8 pad68[0x2C];
    s16 vx, vy, vz, acceleration;
};

extern void gStepBoundedByteChannels(struct Entity *, s32) __asm__("func_8006FB7C");
void gIntegrateActor59List(struct Entity *entity, s32 dt) __asm__("func_80070048");


/**
 * @brief Integrates one entity list: gravity, velocity, spin, then the
 *        per-node follow-up.
 *
 * @param entity Head of the list (next at +0).
 * @param dt Frame delta; the scale is dt << 4.
 * @return Nothing.
 *
 * @par Confirmed
 * Skips nodes with flag bit 3. For the rest: vy += (acceleration * scale)
 * >> 12 when acceleration is non-zero; x/y/z += (v * scale) >> 8; angle +=
 * (rotation * scale) >> 12; then func_8006FB7C(entity, dt) runs.
 *
 * @par Likely role
 * The physics pass of the scene traversal func_8003F4A0: the 8-bit velocity
 * shift and the 12-bit acceleration/spin shifts are the shared motion
 * units of the effect nodes.
 *
 * @par Unknown
 * What func_8006FB7C does after the integration (likely the ground clamp).
 *
 * @par Callers
 * func_8003F4A0.
 *
 * @par Environment path
 * Advances every node of the scene list gListHeads[1] (D_800A1070[1], passed
 * by gRunSceneListPasses, 8003F4A0.c:48) by dt << 4: nodes with bit 3 of the
 * +0x0F flag byte are skipped, otherwise vy += accel(+0x9A) * scale >> 12,
 * x/y/z (+0x58..) += v(+0x94..) * scale >> 8, the angle +0x12 += spin(+0x66)
 * * scale >> 12, and func_8006FB7C(node, dt) then steps the six bounded byte
 * values at +0x79..+0x7F. The offsets +0x12, +0x66, +0x94 and +0x9A are
 * exactly the fields func_8007043C fills for kind-0x59 objects, and the same
 * list is drawn by gDrawGlowNodeList (func_80070158, 8003F6E4.c:93). It runs
 * once per gRunSceneListPasses call (the scene update pass) and never unlinks
 * or frees nodes.
 */
void gIntegrateActor59List(struct Entity *entity, s32 dt)
{
    s32 scale = dt << 4;
    while (entity != 0) {
        if (!(entity->flags & 8)) {
            if (entity->acceleration != 0)
                entity->vy += (entity->acceleration * scale) >> 12;
            entity->x += (entity->vx * scale) >> 8;
            entity->y += (entity->vy * scale) >> 8;
            entity->z += (entity->vz * scale) >> 8;
            entity->angle += (entity->rotation * scale) >> 12;
            gStepBoundedByteChannels(entity, dt);
        }
        entity = entity->next;
    }
}
