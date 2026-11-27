// Categories: Resources > Asset Loader | Game Engine > Resource Management
#include "game/types.h"

struct Resource {
    u8 unk00[0x0C];
    struct Resource *next;
    struct Resource *prev;
    u8 unk14[4];
    s32 group;
    s32 name;
};

extern struct Resource *gResourceListHead __asm__("D_8009F5B8");

/* checks if a resource is already loaded */

s32 gHasRegisteredResource(struct Resource *resource) __asm__("func_80019AA8");

/**
 * @brief Tests whether a descriptor's key pair is present in the shared
 * resource registry.
 *
 * @param resource Descriptor whose key words at +0x18 and +0x1C are tested.
 * @return One if the pair is present, otherwise zero.
 *
 * @par Confirmed mechanics
 * Walks the +0x0C links from gResourceListHead and compares the words at +0x18
 * and +0x1C. SLTU converts the final node pointer to a Boolean in the return
 * delay slot.
 *
 * @par Globals
 * Reads gResourceListHead.
 *
 * @par Callers
 * Direct calls in the retail listing are from gPrepareModelResource,
 * func_80049734, func_800511E4, func_80051650.
 *
 * @par Role and path
 * Asset preparation uses this test to choose the new or duplicate load path.
 *
 * @par Unknown
 * The interpretation of individual key values depends on the asset family.
 */
s32 gHasRegisteredResource(struct Resource *resource)
{
    struct Resource *node;

    for (node = gResourceListHead; node != 0; node = node->next) {
        if (node->group == resource->group && node->name == resource->name) {
            break;
        }
    }
    return node != 0;
}
