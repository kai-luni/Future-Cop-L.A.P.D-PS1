// Categories: Resources > Asset Loader | Game Engine > Resource Management
#include "game/types.h"

struct Unk51650 {
    u8 unk00[8];
    s32 unk08;
};

extern u8 func_80019AA8(struct Unk51650 *resource);
extern void func_80019A74(struct Unk51650 *resource);
extern void func_80019D00(struct Unk51650 *resource);
void gRegisterUniqueResource(struct Unk51650 *resource) __asm__("func_80051650");


/**
 * @brief Registers a fresh resource descriptor in D_8009F5B8 or frees it if already loaded.
 *
 * Checks func_80019AA8(resource): if not yet loaded (0), clears resource->unk08 (+0x08) and
 * appends it via func_80019A74; otherwise hands it to func_80019D00 to free the duplicate.
 *
 * @param resource Resource descriptor checked and either registered or freed.
 *
 * @par Globals
 * None directly; callees traverse and append to the D_8009F5B8/D_8009F5BC resource list.
 *
 * @par Control flow
 * Single conditional branch on func_80019AA8(resource) == 0 dispatching to registration vs teardown.
 *
 * @par Calls
 * func_80019AA8 - checks loaded list; func_80019A74 - appends to tail; func_80019D00 - scans list and frees resource.
 *
 * @par Side effects
 * Clears resource->unk08 and appends to D_8009F5B8/D_8009F5BC, or deallocates resource via func_80019D00.
 *
 * @par Callers
 * Confirmed: none in direct-call list or asm/us/main.s / Likely: unreferenced minimal resource post-load handler / Unknown: whether invoked indirectly.
 */
void gRegisterUniqueResource(struct Unk51650 *resource)
{
    if (func_80019AA8(resource) == 0) {
        resource->unk08 = 0;
        func_80019A74(resource);
    } else {
        func_80019D00(resource);
    }
}
