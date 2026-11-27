// Categories: Resources > Resource Registry | Resources > Memory Management
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

extern void gFreeBlock(void *block) __asm__("func_800181D4");

void gScanAndFreeResource(struct Resource *resource) __asm__("func_80019D00");

/**
 * @brief Scans the resource registry for the supplied descriptor's keys and
 * frees the supplied descriptor.
 *
 * @param resource Descriptor to free, containing the keys used for the scan.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Compares +0x18 and +0x1C along the list until matching or exhausting it.
 * Both exits call gFreeBlock with the original $a0. The matched node is left
 * linked.
 *
 * @par Globals
 * Reads gResourceListHead.
 *
 * @par Callers
 * Direct calls in the retail listing are from gPrepareModelResource,
 * func_80049734, func_800511E4, func_80051650.
 *
 * @par Role and path
 * Asset post-load handlers reach this path after gHasRegisteredResource finds
 * an existing entry.
 *
 * @par Unknown
 * What the repeated key scan was intended to establish is open.
 */
void gScanAndFreeResource(struct Resource *resource)
{
    struct Resource *node;

    for (node = gResourceListHead; node != 0; node = node->next) {
        if (node->group == resource->group && node->name == resource->name) {
            break;
        }
    }
    gFreeBlock(resource);
}
