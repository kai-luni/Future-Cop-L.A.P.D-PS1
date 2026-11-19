// Categories: Resources > Asset Loader | Graphics > Lighting
#include "game/types.h"

struct Unk49EA8 {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

extern u8 *func_80019C38(s32 arg0, s32 arg1, u32 tag, s32 arg3);
extern s32 func_80049E88(u8 *data, s32 arg1);
extern void gFreeBlock(void *block) __asm__("func_800181D4");
void gLoadActorLightNode(struct Unk49EA8 *resource) __asm__("func_80049EA8");


/**
 * @brief Extracts a type-0x61 `0x74414354` light descriptor, spawns a persistent map light, and frees the request.
 *
 * Looks up the `0x74414354` chunk matching `resource->unk1C`, steps back 4 bytes to its descriptor header for func_80049E88 with duration `-1`, and frees `resource`. Category Resources > Asset Loader | Graphics > Lighting follows chunk lookup, map-light spawning (func_80049C38), and dispatch from func_8004D6D4.
 *
 * @param resource Request block (`+0x00` blob base, `+0x1C` section id, `+0x20` byte length), freed before return.
 *
 * @par Globals
 * None directly (func_80049E88 delegates to func_80049C38, reading D_8009F778..D_8009F784).
 *
 * @par Control flow
 * Unconditional linear sequence of three calls; no branches or loops.
 *
 * @par Calls
 * func_80019C38 - locates the `0x74414354` sub-chunk in `resource->unk00` (length `unk20`, id `unk1C`).
 * func_80049E88 - calls func_80049C38 with `chunk - 4` and `-1` (persistent duration) to allocate a light node.
 * func_800181D4 - frees the `resource` request block.
 *
 * @par Side effects
 * Allocates and initializes a map-light node in D_8009F784 via func_80049E88 and frees `resource`.
 *
 * @par Callers
 * Confirmed: func_8004D6D4 (0x8004D77C in asm/us/main.s, switch case `0x61`) / Likely: none / Unknown: none.
 *
 * @par Environment path
 * Handles a type-0x61 resource request: func_80019C38 locates the 'tACT'
 * (0x74414354) chunk with section id +0x1C inside the blob at +0x00 of length
 * +0x20, the pointer is moved back 4 bytes to the chunk header and passed to
 * func_80049E88 with duration -1, which spawns a persistent map light node
 * through func_80049C38, and the request block is then freed with gFreeBlock.
 * Its only caller is the entity-definition dispatcher func_8004D6D4 (case
 * 0x61 at 8004D6D4.c:110-111), so it runs while a level's actor resources are
 * loaded. It reads no global itself; the node lands in the gLightGrid buckets
 * allocated by gAllocLightGrid (func_800496D0).
 */
void gLoadActorLightNode(struct Unk49EA8 *resource)
{
    func_80049E88(func_80019C38(resource->unk00, resource->unk20, 0x74414354,
                                resource->unk1C) - 4, -1);
    gFreeBlock(resource);
}
