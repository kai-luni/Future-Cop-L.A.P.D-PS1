// Categories: Resources > Asset Loader | Resources > Memory Management
#include "game/types.h"

extern int gRegistryLoad();

struct Resource {
    u8 unk00[0x0C];
    struct Resource *next;
    struct Resource *prev;
    u8 unk14[4];
    s32 group;
    s32 name;
};

extern struct Resource *gResourceListHead __asm__("D_8009F5B8");

struct Resource *gFindRegisteredResource(s32 group, s32 name) __asm__("func_80019BF0");

/**
 * @brief Searches the resource registry for a descriptor with the given two
 * key words.
 *
 * @param group First key at descriptor offset +0x18.
 * @param name Second key at descriptor offset +0x1C.
 * @return Matching descriptor, or null if exhausted.
 *
 * @par Confirmed mechanics
 * Loads the head and follows +0x0C links, comparing the incoming registers
 * with +0x18 and +0x1C. Returns the first matching pointer; the exit on
 * exhaustion leaves zero.
 *
 * @par Globals
 * Reads gResourceListHead.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_8004D6D4, func_80060838,
 * func_80060E74, gRegistryLoad, func_80084110.
 *
 * @par Role and path
 * Asset consumers use this registry lookup to obtain a previously registered
 * descriptor.
 *
 * @par Unknown
 * Individual callers define the interpretation of their key values.
 */
struct Resource *gFindRegisteredResource(s32 group, s32 name)
{
    struct Resource *node;

    for (node = gResourceListHead; node != 0; node = node->next) {
        if (node->group == group && node->name == name) {
            return node;
        }
    }
    return 0;
}
