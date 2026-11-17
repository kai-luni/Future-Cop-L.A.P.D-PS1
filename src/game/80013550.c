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
extern void func_800131A0(void **out, void *data, s32 arg2, s32 arg3);
extern void func_80019A74(struct Unk13550 *resource);
extern void func_80019D00(struct Unk13550 *resource);

/**
 * @brief Rebuilds one resource through the node expander.
 *
 * For a fresh resource the expander func_800131A0 is called with a local output
 * pointer and the resource's data/size words; the resulting block replaces the
 * resource data, the status is cleared and the resource is re-prepared. Loaded
 * resources take the maintenance path.
 * @param resource Resource block (data 0x00, 0x1C, 0x20).
 * @par Globals
 * None.
 * @par Control flow
 * Two exclusive branches around one expander call.
 * @par Calls
 * func_80019AA8 - state. func_800131A0 - node expander.
 * func_80019A74 - prepare. func_80019D00 - maintenance.
 * @par Side effects
 * Replaces the resource data pointer.
 * @par Callers
 * Confirmed: func_80024EC8 (asset loader).
 * @par Texture load path
 * Called by func_80024EC8 on tag `0x436F626A` ("Cobj"). No pixel
 * data is uploaded to VRAM here: func_800131A0 / func_80012E48 only extract
 * geometry and animation chunk pointers (and translate polygon records via
 * D_800A8E80); model/ribbon texture entries come from the static table
 * D_8009B234 read by func_80074104.
 */
void func_80013550(struct Unk13550 *resource)
{
    void *loaded;

    loaded = 0;
    if (func_80019AA8(resource) == 0) {
        func_800131A0(&loaded, resource->data, resource->unk20, resource->unk1C);
        resource->unk08 = 0;
        resource->data = loaded;
        func_80019A74(resource);
    } else {
        func_80019D00(resource);
    }
}
