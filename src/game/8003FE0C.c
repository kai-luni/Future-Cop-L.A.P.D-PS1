// Categories: Gameplay > Object Spawning | Gameplay > Map | Game Engine > Object Setup
#include "game/types.h"

struct Unk3FD74Source;

struct Unk3FD74Node {
    struct Unk3FD74Node *next;
    u8 type;
    u8 unk05[3];
    s32 arg08;
    u8 unk0C[4];
    struct Unk3FD74Source *source;
    s32 sourceValue;
    s32 arg18;
    u8 flags;
    u8 unk1D[3];
};

struct Unk3FE0CObject {
    u8 unk00[0x44];
    struct Unk3FD74Node *owner;
};

extern struct Unk3FE0CObject *gCreateModelNode(struct Unk3FD74Source *source,
                                            u8 type, s32 arg08, s32 arg18,
                                            s32 arg4, s32 arg5) __asm__("func_8003D680");
extern s32 gSpawnObjectFromTable(struct Unk3FE0CObject *object, s32 arg1, u8 type,
                         s32 arg18, s32 arg08) __asm__("func_8003E984");
void gSpawnDeferredNode(struct Unk3FD74Node *node) __asm__("func_8003FE0C");


/**
 * @brief Instantiates and registers a deferred map spawn node when it enters player range.
 *
 * Allocates and builds the object via func_8003D680 from node's descriptor fields, sets object->owner (+0x44) back to node, registers it in the active world with func_8003E984, and sets node->flags bit 0 on success.
 *
 * @param node Spawn request node (+0x04 type, +0x08 row/arg08, +0x10 source, +0x18 col/arg18, +0x1C flags).
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Calls func_8003D680; if non-null, writes object->owner = node and calls func_8003E984, setting node->flags |= 1 when func_8003E984 returns non-zero.
 *
 * @par Calls
 * func_8003D680 - allocates and initializes the 0x4C-byte object from node->source.
 * func_8003E984 - registers the object into world buckets and spawns its active entity.
 *
 * @par Side effects
 * Allocates/registers a world object, links object->owner (+0x44), and may set bit 0 of node->flags (+0x1C).
 *
 * @par Callers
 * Confirmed: func_8003FF7C (0x800402A0, when an inactive spawn node enters player range).
 */
void gSpawnDeferredNode(struct Unk3FD74Node *node)
{
    struct Unk3FE0CObject *object;

    object = gCreateModelNode(node->source, node->type, node->arg08,
                           node->arg18, 0, 1);
    if (object != 0) {
        object->owner = node;
        if (gSpawnObjectFromTable(object, 0, node->type, node->arg18,
                          node->arg08) != 0) {
            node->flags |= 1;
        }
    }
}
