// Categories: Resources > Asset Loader | Resources > Memory Management
#include "game/types.h"

struct Resource {
    u8 unk00[0x0C];
    struct Resource *next;
    struct Resource *prev;
    u8 unk14[4];
    s32 group;
    s32 name;
};

extern struct Resource *D_8009F5B8;

/**
 * @brief Searches the global resource list D_8009F5B8 for a node matching `(group, name)`.
 *
 * Traverses the doubly linked registry maintained by func_80019A74/func_80019BA4 and compares `group` (+0x18) and `name` (+0x1C) on each `Resource` node. Category Resources > Asset Loader | Resources > Memory Management follows the D_8009F5B8 resource registry and asset loader callers.
 *
 * @param group Resource group identifier compared against `node->group` (+0x18).
 * @param name Resource entry identifier compared against `node->name` (+0x1C).
 * @return Pointer to the first matching `struct Resource`, or 0 if not found.
 *
 * @par Globals
 * D_8009F5B8 - head of the active global resource linked list.
 *
 * @par Control flow
 * Single `for` loop following `node->next` from D_8009F5B8 until null, returning early on the first `(group, name)` match.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80060838 (0x800609A4), func_80060E74 (0x80060FC0), func_80084110 (0x80084164), func_8004D6D4 (0x8004D894 in asm/us/main.s), and func_8006A918 (0x8006AA9C in asm/us/main.s) / Likely: none / Unknown: none.
 */
struct Resource *func_80019BF0(s32 group, s32 name)
{
    struct Resource *node;

    for (node = D_8009F5B8; node != 0; node = node->next) {
        if (node->group == group && node->name == name) {
            return node;
        }
    }
    return 0;
}
