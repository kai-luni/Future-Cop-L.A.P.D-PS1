// Categories: Resources > Memory Management | Resources > Asset Loader
#include "game/types.h"

struct Resource {
    u8 unk00[0x0C];
    struct Resource *next;
};

extern struct Resource *D_8009F5B8;
extern struct Resource *D_8009F5BC;

extern void func_80019AF8(struct Resource *resource);

/**
 * @brief Releases every node of the resource registry D_8009F5B8/D_8009F5BC,
 *        letting each node run its own release callback.
 *
 * The registry is a doubly linked list of 0x14-byte nodes: eight unknown bytes,
 * a release function pointer at +0x8, next at +0xC and previous at +0x10 (see
 * func_80019AF8). Each node is unlinked and freed through that helper, which
 * keeps both end pointers consistent, and the two globals are forced to 0
 * afterwards.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F5B8 - registry head.
 * D_8009F5BC - registry tail.
 * Both are cleared after the walk; func_80019A64 performs the same clearing
 * without freeing, so a reset order matters.
 *
 * @par Control flow
 * While loop over the chain with the successor captured first, then two stores.
 *
 * @par Calls
 * func_80019AF8 - calls node->release, unlinks the node from both ends and frees
 * it.
 *
 * @par Side effects
 * Runs every node's release callback, frees the nodes, zeroes both globals.
 *
 * @par Callers
 * Confirmed: func_800249BC, func_80024D14 (global subsystem reset, step 24).
 */
void func_80019BA4(void)
{
    struct Resource *node;
    struct Resource *current;

    node = D_8009F5B8;
    while (node != 0) {
        current = node;
        node = node->next;
        func_80019AF8(current);
    }
    D_8009F5B8 = 0;
    D_8009F5BC = 0;
}
