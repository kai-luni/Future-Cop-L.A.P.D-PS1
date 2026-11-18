// Categories: Graphics > Effects > Object Pool | Gameplay > Object Spawning
#include "game/types.h"

struct Unk73590Owner {
    u8 unk00[8];
    s32 id;
    u8 unk0C[0x1C];
    u8 coord[4];
};

struct Unk73590 {
    void *coord;
    struct Unk73590Owner *owner;
    s32 ownerId;
    u8 kind;
    u8 unk0D[0x0B];
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    s16 unk1E;
};

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");

/**
 * @brief Allocates a 0x20-byte effect node bound to an owner and a coordinate
 *        source.
 *
 * @param owner Optional owner block; +8 its id, +0x28 a coordinate pointer.
 * @param coord Coordinate source; when 0 and an owner exists, the owner's
 *        +0x28 pointer is used.
 * @param kind Effect kind byte.
 * @param arg3, arg4, arg5 Three s16 parameters stored at +0x1A/+0x1C/+0x18.
 * @return The new node, or 0 when the pool is exhausted.
 *
 * @par Confirmed
 * func_80017EBC(0x20, 1) allocates; the node stores coord, owner, owner->id
 * and kind, with the three s16 parameters in the tail. The owner's coord is
 * the fallback when no explicit source is given.
 *
 * @par Likely role
 * The shared constructor of the object-attached effects: the spawner
 * func_8005D8F4, func_8006A524 and the handler func_80073BBC use it for
 * markers that follow their owner's position.
 *
 * @par Callers
 * func_8005D8F4, func_8006A524, func_80073BBC.
 */
struct Unk73590 *func_80073590(struct Unk73590Owner *owner, void *coord, u8 kind, s16 arg3,
                               s16 arg4, s16 arg5)
{
    struct Unk73590 *node;

    node = gAllocMemory(0x20, 1);
    if (node == 0) {
        return 0;
    }
    node->unk18 = arg5;
    node->unk1A = arg3;
    node->unk1C = arg4;
    if (coord == 0 && owner != 0) {
        coord = owner->coord;
    }
    node->coord = coord;
    node->kind = kind;
    node->owner = owner;
    if (owner != 0) {
        node->ownerId = owner->id;
        if (coord == 0) {
            node->coord = owner->coord;
        }
    }
    return node;
}
