// Categories: Resources > Loader Post-Process | Graphics > Lighting
#include "game/types.h"

struct Unk49734Data {
    s32 unk00;
    s32 unk04;
    s32 count;
};

struct Unk49734Resource {
    struct Unk49734Data *data;
    s32 unk04;
    s32 unk08;
};

extern s32 *gSpriteRegionTable __asm__("D_8009F778");
extern s32 D_8009F788;

extern u8 func_80019AA8(struct Unk49734Resource *resource);
extern void func_80019A74(struct Unk49734Resource *resource);
extern void func_80019D00(struct Unk49734Resource *resource);
void gLoadCdcsResource(struct Unk49734Resource *resource) __asm__("func_80049734");


/**
 * @brief Publishes the dynamic light table after a resource load.
 *
 * For a fresh resource (func_80019AA8 == 0) the status word is cleared, the
 * resource is prepared and the table pointer/count pair is published in
 * D_8009F778 and D_8009F788. An already-loaded resource only runs the
 * maintenance path func_80019D00.
 * @param resource Resource whose data block starts with the light table.
 * @par Globals
 * D_8009F778 - pointer to the table count word; D_8009F788 - table size.
 * @par Control flow
 * Two exclusive branches.
 * @par Calls
 * func_80019AA8 - resource state. func_80019A74 - prepare. func_80019D00 - maintenance.
 * @par Side effects
 * Publishes the table and updates the resource state.
 * @par Callers
 * Confirmed: func_80024EC8 (asset loader).
 */
void gLoadCdcsResource(struct Unk49734Resource *resource)
{
    if (func_80019AA8(resource) == 0) {
        resource->unk08 = 0;
        func_80019A74(resource);
        gSpriteRegionTable = &resource->data->count;
        D_8009F788 = resource->data->count;
    } else {
        func_80019D00(resource);
    }
}
