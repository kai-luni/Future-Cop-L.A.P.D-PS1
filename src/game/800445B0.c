// Categories: Graphics > Model Renderer > Object Placement | Graphics > Scene Graph > Object List Walk
#include "game/types.h"

struct Unk445B0Node {
    struct Unk445B0Node *next;
    u8 unk04[0x0B];
    u8 flags;
};

extern void gDrawObjectPointTiles(struct Unk445B0Node *node, void *arg) __asm__("func_80044260");

/**
 * @brief Walks a node list and places every node whose flag bit 3 is
 *        clear.
 *
 * @param node List head (next pointer at +0, flags byte at +0x0F).
 * @param arg Forwarded to the placement pass (the view position).
 * @return Nothing.
 *
 * @par Confirmed
 * while (node): when !(flags & 8), func_80044260(node, arg) (object
 * placement with GTE); node = node->next.
 *
 * @par Callers
 * func_8003F6E4 (scene traversal driver).
 */
void func_800445B0(struct Unk445B0Node *node, void *arg)
{
    while (node != 0) {
        if (!(node->flags & 8)) {
            gDrawObjectPointTiles(node, arg);
        }
        node = node->next;
    }
}
