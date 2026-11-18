// Categories: Resources > Memory Management | Resources > Resource Registry
#include "game/types.h"

struct Resource {
    u8 unk00[8];
    void (*release)(struct Resource *resource);
    struct Resource *next;
    struct Resource *prev;
};

extern struct Resource *gResourceList __asm__("D_8009F5B8");
extern struct Resource *gListTailFlag __asm__("D_8009F5BC");

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Invokes a resource node's release callback, unlinks it, and frees it.
 *
 * Runs resource->release if non-null, splices the node out of the global doubly-linked
 * list D_8009F5B8..D_8009F5BC, and returns the block to the heap via func_800181D4.
 *
 * @param resource Pointer to the Resource node (+0x08 release callback, +0x0C next, +0x10 prev).
 *
 * @par Globals
 * D_8009F5B8 - doubly-linked resource list head; D_8009F5BC - list tail (updated when removing head or tail).
 *
 * @par Control flow
 * Conditionally calls resource->release(resource), updates prev->next or D_8009F5B8 and next->prev or D_8009F5BC, then calls func_800181D4.
 *
 * @par Calls
 * Indirect call to resource->release (+0x08); func_800181D4 - frees the Resource structure.
 *
 * @par Side effects
 * Executes release callback, mutates neighbor links and/or D_8009F5B8/D_8009F5BC, and frees resource.
 *
 * @par Callers
 * Confirmed: func_80019BA4 (0x80019BC4, teardown walk over all registry nodes).
 */
void func_80019AF8(struct Resource *resource)
{
    if (resource->release != 0) {
        resource->release(resource);
    }
    if (resource->prev != 0) {
        resource->prev->next = resource->next;
    } else {
        gResourceList = resource->next;
        if (gResourceList != 0) {
            gResourceList->prev = 0;
        }
    }
    if (resource->next != 0) {
        resource->next->prev = resource->prev;
    } else {
        gListTailFlag = resource->prev;
        if (gListTailFlag != 0) {
            gListTailFlag->next = 0;
        }
    }
    gFreeBlock(resource);
}
