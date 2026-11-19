// Categories: Gameplay > Object Spawning | Game Engine > Deferred Queue
#include "game/types.h"

struct Unk3FD74Source {
    u8 unk00[8];
    s32 value08;
};

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

extern struct Unk3FD74Node *gDeferredSpawnList __asm__("D_8009F30C");
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");

/**
 * @brief Allocates and prepends a deferred spawn request node to the pending spawn list.
 *
 * Allocates a 0x20-byte node from tag 1, populates type, row, source and arg18, and links it at D_8009F30C.
 *
 * @param type Spawn table column index (0..255) stored at +0x04.
 * @param arg08 Spawn table row index stored at +0x08.
 * @param source Optional owner/source pointer (+0x10); if non-null, source->value08 is cached at +0x14.
 * @param arg18 Opaque argument or placement block pointer stored at +0x18.
 * @return Pointer to the new 0x20-byte node, or null if allocation fails.
 *
 * @par Globals
 * D_8009F30C - singly-linked list head of deferred spawn request nodes.
 * @par Control flow
 * Allocates 0x20 bytes; returns 0 on null, reads source->value08 if source != 0, and prepends to D_8009F30C.
 * @par Calls
 * func_80017EBC - allocates 0x20 bytes with tag 1.
 * @par Side effects
 * Allocates heap memory and updates list head D_8009F30C.
 * @par Callers
 * Confirmed: func_8003C2A4 (spawn with generated placement), func_800402DC (cell flags & 5 == 4 deferred path).
 */
struct Unk3FD74Node *func_8003FD74(s32 type, s32 arg08,
                                   struct Unk3FD74Source *source, s32 arg18)
{
    struct Unk3FD74Node *node;

    node = gAllocMemory(0x20, 1);
    if (node == 0) {
        return 0;
    }
    node->next = 0;
    node->flags = 0;
    node->type = type;
    node->arg08 = arg08;
    node->source = source;
    if (source != 0) {
        node->sourceValue = source->value08;
    }
    node->arg18 = arg18;
    node->next = gDeferredSpawnList;
    gDeferredSpawnList = node;
    return node;
}
