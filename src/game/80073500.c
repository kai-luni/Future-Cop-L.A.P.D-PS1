// Categories: Graphics > Scene Graph > Node Allocation | Resources > Memory Pools
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk73500 {
    void *coord;
    void *owner;
    s32 ownerId;
    struct Vec3s32 position;
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    s16 unk1E;
};

extern void *func_80017EBC(s32 size, s32 tag);

/**
 * @brief Allocates and initialises a scene node for a position.
 *
 * @param position Source position (three 32-bit components, copied by value).
 * @param arg1, arg2, arg3 Three half-word parameters: arg3 → +0x18, arg1 → +0x1A,
 *        arg2 → +0x1C.
 * @return The new node, or 0 when the allocator fails.
 *
 * @par Confirmed
 * func_80017EBC(0x20, 1) takes 32 bytes from the tag-1 pool; the node's
 * coordinate pointer (+0) and owner (+4) are cleared, the owner id (+8) is left
 * untouched, and the position is stored at +0xC.
 *
 * @par Likely role
 * The generic "attach an object to a position" node of the effect/renderer
 * lists; the three half-words are the size, the type and a timer.
 *
 * @par Callers
 * func_800137D4, func_8004C8D4, func_8005D060 and the effect spawners.
 */
struct Unk73500 *func_80073500(struct Vec3s32 *position, s16 arg1, s16 arg2, s16 arg3)
{
    struct Unk73500 *node;

    node = func_80017EBC(0x20, 1);
    if (node == 0) {
        return 0;
    }
    node->unk18 = arg3;
    node->unk1A = arg1;
    node->coord = 0;
    node->owner = 0;
    node->unk1C = arg2;
    node->position = *position;
    return node;
}
