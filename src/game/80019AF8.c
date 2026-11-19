// Categories: Resources > Memory Management | Resources > Resource Registry
#include "game/types.h"

struct Resource {
    u8 unk00[8];
    void (*release)(struct Resource *resource);
    struct Resource *next;
    struct Resource *prev;
};

extern struct Resource *gResourceListHead __asm__("D_8009F5B8");
extern struct Resource *gResourceListTail __asm__("D_8009F5BC");

extern void gFreeBlock(void *block) __asm__("func_800181D4");

void gReleaseRegisteredResource(struct Resource *resource) __asm__("func_80019AF8");

/**
 * @brief Calls a resource's release callback, unlinks the descriptor, and
 * frees it.
 *
 * @param resource Registered descriptor with optional release callback at +8.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * If the +8 word is nonzero, invokes it via JALR with the descriptor still in
 * $a0. Patches adjacent +0x0C and +0x10 links or updates list endpoints, then
 * passes the descriptor to gFreeBlock.
 *
 * @par Globals
 * Updates gResourceListHead and gResourceListTail when removing an endpoint.
 *
 * @par Callers
 * Direct calls in the retail listing are from gReleaseAllResources.
 *
 * @par Role and path
 * The registry teardown path releases the payload through its callback and
 * returns the descriptor's allocation.
 *
 * @par Unknown
 * The action of the release callback depends on the resource type.
 */
void gReleaseRegisteredResource(struct Resource *resource)
{
    if (resource->release != 0) {
        resource->release(resource);
    }
    if (resource->prev != 0) {
        resource->prev->next = resource->next;
    } else {
        gResourceListHead = resource->next;
        if (gResourceListHead != 0) {
            gResourceListHead->prev = 0;
        }
    }
    if (resource->next != 0) {
        resource->next->prev = resource->prev;
    } else {
        gResourceListTail = resource->prev;
        if (gResourceListTail != 0) {
            gResourceListTail->next = 0;
        }
    }
    gFreeBlock(resource);
}
