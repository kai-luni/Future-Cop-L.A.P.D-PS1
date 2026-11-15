// Categories: Graphics > Renderer > Scene Traversal | Graphics > Scene Graph
#include "game/types.h"

struct Unk441DC {
    struct Unk441DC *next;
    u8 unk04[0x0B];
    u8 flags0F;
    u8 unk10[0x7D];
    u8 flags8D;
};

extern void func_80043CA8(struct Unk441DC *node, s32 arg);
extern void func_800437E8(struct Unk441DC *node, s32 arg);

/**
 * @brief Walks a scene node list and dispatches each visible node to one of
 *        two draw paths.
 *
 * @param node Head of the 0x8E-byte node list (next at +0).
 * @param arg Forwarded to both draw paths.
 * @return Nothing.
 *
 * @par Confirmed
 * Skips nodes with flag bit 3 at +0x0F. For the rest, flag bit 0 at +0x8D
 * selects func_80043CA8, otherwise func_800437E8 runs.
 *
 * @par Likely role
 * One of the layer passes of the scene traversal func_8003F4A0: the two
 * paths are likely the textured and the flat-shaded draw of the same node
 * class.
 *
 * @par Unknown
 * What distinguishes the two draw paths beyond the flag bit.
 *
 * @par Callers
 * func_8003F4A0.
 */
void func_800441DC(struct Unk441DC *node, s32 arg)
{
    while (node != 0) {
        if (!(node->flags0F & 8)) {
            if (node->flags8D & 1) {
                func_80043CA8(node, arg);
            } else {
                func_800437E8(node, arg);
            }
        }
        node = node->next;
    }
}
