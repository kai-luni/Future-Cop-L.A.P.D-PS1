// Categories: Resources > Loader Post-Process
#include "game/types.h"

struct Unk13550 {
    void *data;
    s32 unk04;
    s32 unk08;
    u8 unk0C[0x10];
    s32 unk1C;
    s32 unk20;
};

extern u8 func_80019AA8(struct Unk13550 *resource);
extern void gRelocateModelResource(void **out, void *data, s32 arg2, s32 arg3) __asm__("func_800131A0");
extern void func_80019A74(struct Unk13550 *resource);
extern void func_80019D00(struct Unk13550 *resource);

void gPrepareModelResource(struct Unk13550 *resource) __asm__("func_80013550");

/**
 * @brief Relocates a newly loaded Cobj resource or releases the duplicate
 * through the resource reuse path.
 *
 * @param resource Resource descriptor with data at +0 and loader metadata at
 *     +0x1C and +0x20.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Tests the low byte of func_80019AA8. If it is zero, passes descriptor words
 * +0, +0x20, and +0x1C to gRelocateModelResource, installs its output at +0,
 * clears +8, and calls func_80019A74. The other branch calls func_80019D00.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from gLoadQueuedAssets.
 *
 * @par Role and path
 * The dispatcher at gLoadQueuedAssets selects this post-load handler for the
 * Cobj asset tag.
 *
 * @par Unknown
 * The model-specific interpretation of the descriptor word at +0x1C is open.
 */
void gPrepareModelResource(struct Unk13550 *resource)
{
    void *loaded;

    loaded = 0;
    if (func_80019AA8(resource) == 0) {
        gRelocateModelResource(&loaded, resource->data, resource->unk20, resource->unk1C);
        resource->unk08 = 0;
        resource->data = loaded;
        func_80019A74(resource);
    } else {
        func_80019D00(resource);
    }
}
