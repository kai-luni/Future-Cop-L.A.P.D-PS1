// Categories: Gameplay > Entity Update | Game Engine > Frame
#include "game/types.h"

struct Unk3F428Node;

struct Unk3F428Ops {
    s32 unk00;
    void (*handler)(s32 message, struct Unk3F428Node *node, s32 arg2, s32 arg3);
};

struct Unk3F428Def {
    u8 unk00[0x48];
    struct Unk3F428Ops *ops;
};

struct Unk3F428Node {
    struct Unk3F428Node *next;
    u8 unk04[0x18];
    struct Unk3F428Def *def;
};
void gDispatchSpawnSlotTicks(struct Unk3F428Node *node, s32 arg) __asm__("func_8003F428");


/**
 * @brief Dispatches message 1 (frame tick) to every node with non-null def->ops along a linked list.
 *
 * Confirmed: walks node->next (+0x00) until null; when node->def (+0x1C) and node->def->ops (+0x48) are both non-null, calls node->def->ops->handler(1, node, arg, 0) (slot +0x04). Invoked each frame on entity bucket list D_800A1070[5].
 *
 * @param node Head of the singly-linked node list (D_800A1070[5] in func_8003F570).
 * @param arg Frame delta forwarded as arg2 to ops->handler(1, node, arg, 0).
 *
 * @par Globals
 * None directly (caller func_8003F570 passes D_800A1070[5]).
 *
 * @par Control flow
 * Single while loop (node != 0) guarding the indirect call on node->def != 0 && node->def->ops != 0.
 *
 * @par Calls
 * Indirect call node->def->ops->handler(1, node, arg, 0) via pointer at +0x1C -> +0x48 -> +0x04.
 *
 * @par Side effects
 * Invokes message 1 update handlers across all eligible nodes in the list.
 *
 * @par Callers
 * Confirmed: func_8003F570 (0x8003F668 in asm/us/main.s, per-frame entity world tick) / Likely: none / Unknown: none.
 */
void gDispatchSpawnSlotTicks(struct Unk3F428Node *node, s32 arg)
{
    while (node != 0) {
        if (node->def != 0 && node->def->ops != 0) {
            node->def->ops->handler(1, node, arg, 0);
        }
        node = node->next;
    }
}
