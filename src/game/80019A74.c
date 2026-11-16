// Categories: Resources > Resource Registry | Resources > Asset Loader
#include "game/types.h"

struct Resource {
    u8 unk00[0x0C];
    struct Resource *next;
    struct Resource *prev;
};

extern struct Resource *gResourceList __asm__("D_8009F5B8");
extern struct Resource *D_8009F5BC;

/* Appends a resource to the global doubly linked list.
 * D_8009F5B8 is the first node and D_8009F5BC the last; a new entry is linked
 * behind the tail and becomes the new tail, or becomes head and tail when the
 * list is still empty. Registered nodes are what func_80019BF0 searches by
 * (group, name). */

/**
 * @brief Appends a resource record to the global doubly linked resource list.
 *
 * Clears resource->next and resource->prev and links the node at the tail D_8009F5BC (initializing head D_8009F5B8 as well if empty), registering it for lookups such as func_80019BF0.
 *
 * @param resource Resource descriptor with next (+0x0C) and prev (+0x10) pointers.
 *
 * @par Globals
 * D_8009F5B8 - head of resource list (written when empty); D_8009F5BC - tail of resource list (always updated to resource).
 *
 * @par Control flow
 * Single branch on D_8009F5BC != 0: appends after tail when non-empty, otherwise sets both head and tail.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Mutates resource->next/prev, previous tail->next, and D_8009F5B8/D_8009F5BC.
 *
 * @par Callers
 * Confirmed: func_80013550, func_80029E20, func_80049420, func_80049734, func_8004D924, func_800511E4, func_80051650, func_80060E74, func_80063C20.
 */
void func_80019A74(struct Resource *resource)
{
    resource->next = 0;
    resource->prev = 0;
    if (D_8009F5BC != 0) {
        resource->prev = D_8009F5BC;
        D_8009F5BC->next = resource;
        D_8009F5BC = resource;
    } else {
        D_8009F5BC = resource;
        gResourceList = resource;
    }
}
