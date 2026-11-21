// Categories: Resources > Asset Loader | Resources > Loader Post-Process
#include "game/types.h"

struct Unk29E20 {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

extern void func_80019A74(struct Unk29E20 *resource);
extern u8 *func_80019C38(s32 arg0, s32 arg1, u32 arg2, s32 arg3);
extern u8 *gResourceSubChunkPtr __asm__("D_8009F698");
void gRegisterPortraitStreamTable(struct Unk29E20 *resource) __asm__("func_80029E20");


/**
 * @brief Registers a loaded `0x43746F73` resource and publishes its `0x74544F53` sub-chunk pointer to D_8009F698.
 *
 * Links `resource` onto the global doubly linked resource registry and locates the sub-chunk tagged `0x74544F53` matching `resource->unk1C` inside the blob (`unk00`, size `unk20`). Category Resources > Asset Loader | Resources > Loader Post-Process follows resource list registration, chunk tag lookup, and dispatch from func_80024EC8.
 *
 * @param resource Loaded resource header (`+0x00` data base, `+0x1C` section id, `+0x20` byte length).
 *
 * @par Globals
 * D_8009F698 - stores the returned `0x74544F53` payload pointer (used with 12-byte stride in func_80029C34).
 *
 * @par Control flow
 * Straight-line sequence of two helper calls and one global store; no branches or loops.
 *
 * @par Calls
 * func_80019A74 - appends `resource` to the global list D_8009F5B8/D_8009F5BC.
 * func_80019C38 - scans `resource->unk00` (length `unk20`) for tag `0x74544F53` and id `unk1C`.
 *
 * @par Side effects
 * Mutates D_8009F5B8/D_8009F5BC linkage via func_80019A74 and writes D_8009F698.
 *
 * @par Callers
 * Confirmed: func_80024EC8 (0x800252D0 in asm/us/main.s, switch case `0x43746F73`) / Likely: none / Unknown: none.
 */
void gRegisterPortraitStreamTable(struct Unk29E20 *resource)
{
    func_80019A74(resource);
    gResourceSubChunkPtr = func_80019C38(resource->unk00, resource->unk20, 0x74544F53,
                               resource->unk1C);
}
