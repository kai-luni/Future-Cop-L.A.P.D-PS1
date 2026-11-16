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

extern struct Resource *gResourceList __asm__("D_8009F5B8");

/* checks if a resource is already loaded */

/**
 * @brief Checks whether a resource with matching group and name is already in D_8009F5B8.
 *
 * Walks the loaded-resource singly-linked list starting at D_8009F5B8 (+0x0C next pointer),
 * comparing node->group (+0x1C) and node->name (+0x20) against the target struct Resource.
 *
 * @param resource Resource descriptor whose group (+0x1C) and name (+0x20) are queried.
 * @return 1 if a matching entry is present in D_8009F5B8, else 0.
 *
 * @par Globals
 * D_8009F5B8 - head pointer of the loaded resource list.
 *
 * @par Control flow
 * Single pointer-chasing for-loop breaking on first matching group and name, returning node != 0.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80013550 (0x80013560), func_80049734 (0x80049740), func_800511E4 (0x800511F8), and func_80051650 (0x8005165C) / Likely: none / Unknown: none.
 */
s32 func_80019AA8(struct Resource *resource)
{
    struct Resource *node;

    for (node = gResourceList; node != 0; node = node->next) {
        if (node->group == resource->group && node->name == resource->name) {
            break;
        }
    }
    return node != 0;
}
