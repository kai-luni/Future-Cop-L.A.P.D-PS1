// Categories: Game Engine > Entity Lifecycle | Resources > Memory Management
#include "game/types.h"

struct Unk3CF20 {
    struct Unk3CF20 *next;
    u8 unk04[0x30];
    void *buffer;
    u8 unk38;
    u8 flags;
};

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Frees a transform-sync node and, when flagged, its owned buffer.
 *
 * @param object Node; +0x34 the buffer pointer, +0x39 the flags.
 * @return Nothing.
 *
 * @par Confirmed
 * Flag bit 1 frees object->buffer via func_800181D4 first, then the node
 * itself is freed.
 *
 * @par Likely role
 * The teardown of the depth-cued object list nodes managed by
 * func_8003D240: bit 1 marks nodes that allocated their own transform
 * buffer instead of pointing into shared data.
 *
 * @par Callers
 * func_8003D0AC and func_8003D240.
 */
void gFreeOwnedBufferNode(struct Unk3CF20 *object) __asm__("func_8003CF20");
void gFreeOwnedBufferNode(struct Unk3CF20 *object)
{
    if (object->flags & 2) {
        gFreeBlock(object->buffer);
    }
    gFreeBlock(object);
}
