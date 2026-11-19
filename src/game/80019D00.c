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

/**
 * @brief Scans D_8009F5B8 for a matching resource key and frees the supplied resource block.
 *
 * Confirmed: traverses the doubly-linked resource registry at D_8009F5B8 (+0x0C next) comparing group (+0x18) and name (+0x1C), breaks on match without referencing the found node, and unconditionally passes resource to func_800181D4. Likely: duplicate-resource cleanup path when func_80019AA8 reports an existing entry.
 *
 * @param resource Resource descriptor (+0x18 group, +0x1C name) to release.
 *
 * @par Globals
 * D_8009F5B8 - head pointer of the active resource registry list (read-only walk).
 *
 * @par Control flow
 * Single for loop along D_8009F5B8->next with early break on (group, name) match, followed by func_800181D4(resource).
 *
 * @par Calls
 * func_800181D4 - frees the resource memory block.
 *
 * @par Side effects
 * Releases resource via func_800181D4 without unlinking any node from D_8009F5B8.
 *
 * @par Callers
 * Confirmed: func_80013550 (0x800135A4), func_80049734 (0x80049780), func_800511E4 (0x800512E8), func_80051650 (0x80051684) / Likely: none / Unknown: none.
 */
void func_80019D00(struct Resource *resource)
{
    struct Resource *node;

    for (node = gResourceListHead; node != 0; node = node->next) {
        if (node->group == resource->group && node->name == resource->name) {
            break;
        }
    }
    gFreeBlock(resource);
}
