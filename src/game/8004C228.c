// Categories: Graphics > Renderer > Scene Traversal | Graphics > Scene Graph
#include "game/types.h"

struct Unk4C228 {
    struct Unk4C228 *next;
    u8 unk04[0x0B];
    u8 flags0F;
    u8 unk10[0x6B];
    u8 flags7B;
};

extern void gStepDebrisFlight(struct Unk4C228 *node, s32 arg) __asm__("func_8004AFC0");
extern void func_8004BEE8(struct Unk4C228 *node, s32 arg);

/**
 * @brief Walks a scene node list and dispatches each visible node by two
 *        flag bits (twin of func_800441DC with 0x7C-byte nodes).
 *
 * @param node Head of the list (next at +0).
 * @param arg Forwarded to both draw paths.
 * @return Nothing.
 *
 * @par Confirmed
 * Skips nodes with flag bit 3 at +0x0F. Flag bit 3 at +0x7B selects
 * func_8004AFC0; otherwise flag bit 4 at +0x7B suppresses the node entirely,
 * and the remaining nodes go to func_8004BEE8.
 *
 * @par Likely role
 * A second layer pass of the scene traversal func_8003F4A0 with a three-way
 * split: special draw, hidden, normal draw.
 *
 * @par Callers
 * func_8003F4A0.
 */
void func_8004C228(struct Unk4C228 *node, s32 arg)
{
    while (node != 0) {
        if (!(node->flags0F & 8)) {
            if (node->flags7B & 8) {
                gStepDebrisFlight(node, arg);
            } else if (!(node->flags7B & 0x10)) {
                func_8004BEE8(node, arg);
            }
        }
        node = node->next;
    }
}
