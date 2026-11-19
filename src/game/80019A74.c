// Categories: Resources > Resource Registry | Resources > Asset Loader
#include "game/types.h"

struct Resource {
    u8 unk00[0x0C];
    struct Resource *next;
    struct Resource *prev;
};

extern struct Resource *gResourceListHead __asm__("D_8009F5B8");
extern struct Resource *gResourceListTail __asm__("D_8009F5BC");

/* Appends a resource to the global doubly linked list.
 * D_8009F5B8 is the first node and D_8009F5BC the last; a new entry is linked
 * behind the tail and becomes the new tail, or becomes head and tail when the
 * list is still empty. Registered nodes are what gFindRegisteredResource searches by
 * (group, name). */

void gAppendResourceRegistryEntry(struct Resource *resource) __asm__("func_80019A74");

/**
 * @brief Appends a resource descriptor to the shared doubly linked registry.
 *
 * @param resource Resource descriptor to append.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clears links at +0x0C and +0x10. A non-null old tail becomes the new node's
 * previous link and receives its next link. If the tail was null, the new node
 * also becomes the head.
 *
 * @par Globals
 * Reads and writes gResourceListTail and sets gResourceListHead when the list
 * was empty.
 *
 * @par Callers
 * Direct calls in the retail listing are from gPrepareModelResource,
 * func_80029E20, func_80049420, func_80049734, func_8004D924, func_800511E4,
 * func_80051650, func_80060E74, func_80063C20.
 *
 * @par Role and path
 * Asset post-load handlers register prepared resources for later lookups and
 * release.
 *
 * @par Unknown
 * The caller supplies the descriptor's keys and release callback.
 */
void gAppendResourceRegistryEntry(struct Resource *resource)
{
    resource->next = 0;
    resource->prev = 0;
    if (gResourceListTail != 0) {
        resource->prev = gResourceListTail;
        gResourceListTail->next = resource;
        gResourceListTail = resource;
    } else {
        gResourceListTail = resource;
        gResourceListHead = resource;
    }
}
