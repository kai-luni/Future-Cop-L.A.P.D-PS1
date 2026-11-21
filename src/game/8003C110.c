// Categories: Resources > Asset Loader | Utilities > Memory Management
#include "game/types.h"

struct Unk3C110 {
    s32 unk00;
    u8 unk04[0x1C];
    s32 unk20;
};

extern void gParseCsfxChunks(s32 arg0, s32 arg1) __asm__("func_8003BFD0");
extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Feeds a resource's chunk stream to the dispatcher and frees the node.
 *
 * The node is the one func_80024EC8 reaches through the four-character tag 0x43736678: word +0 is the chunk data and +0x20 the total byte count. func_8003BFD0 walks the chunks (a size word, then a type byte from 0x56 to 0x5E selecting one of nine handlers).
 *
 * @param resource Resource node: +0 chunk data, +0x20 length in bytes.
 * @return Nothing.
 *
 * @par Globals
 * None directly; func_8003BFD0 saves and restores D_8009F310 around the walk.
 *
 * @par Control flow
 * Dispatch call, then an unconditional free; the dispatch result (0 on an unknown type, 1 when the stream is consumed) is ignored.
 *
 * @par Calls
 * func_8003BFD0 - walks and dispatches the chunk stream. func_800181D4 - returns the node to the heap.
 *
 * @par Side effects
 * The per-type handlers build or update asset structures and the node memory is released, so the loader must drop its pointer; a malformed stream still does not leak.
 *
 * @par Callers
 * Confirmed: func_80024EC8 (0x80025310), the tag switch of the asset loader.
 */
void func_8003C110(struct Unk3C110 *resource)
{
    gParseCsfxChunks(resource->unk00, resource->unk20);
    gFreeBlock(resource);
}
