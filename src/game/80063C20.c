// Categories: Resources > Resource State
#include "game/types.h"

struct Unk63C20 {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

extern void func_80019A74(struct Unk63C20 *resource);
extern void func_800639D4(s32 arg0, s32 arg1, s32 arg2);

/**
 * @brief Re-registers a loaded resource with the level setup.
 *
 * The release bookkeeping func_80019A74 runs first, then the resource's header
 * triple (0x00, 0x20, 0x1C) is passed to func_800639D4, which re-enters the
 * resource into the level's table.
 * @param resource Resource block (header 0x00, 0x1C and 0x20).
 * @par Globals
 * None.
 * @par Control flow
 * None.
 * @par Calls
 * func_80019A74 - resource release. func_800639D4 - table registration.
 * @par Side effects
 * Updates the resource state and the level table.
 * @par Callers
 * Unknown: no direct edge in edges.json.
 */
void func_80063C20(struct Unk63C20 *resource)
{
    func_80019A74(resource);
    func_800639D4(resource->unk00, resource->unk20, resource->unk1C);
}
