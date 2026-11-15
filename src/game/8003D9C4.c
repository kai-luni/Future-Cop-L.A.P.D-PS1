// Categories: Graphics > Scene Graph > Object List | Utilities
#include "../../include/game/types.h"

struct ChildNode {
    u8 pad_00[0x34];
    void *child;
};

struct ParentNode {
    u8 pad_00[0x1C];
    struct ChildNode *node;
    u8 pad_20[0x14];
    void *fallback;
};

/**
 * @brief Returns the address of the "next mesh" slot belonging to a scene node.
 *
 * @param parent Parent node; the child pointer sits at +0x38 and a fallback
 *               slot at +0x50.
 * @return A pointer to the link slot to fill: when parent->node is NULL the
 *         address of parent->fallback; otherwise the address of
 *         parent->node->child, and - when that child exists - the address of
 *         the grandchild slot. The caller therefore receives a slot it can
 *         assign a mesh pointer to, one indirection below the parent.
 *
 * @par Likely role
 * Accessor of the scene graph used by the renderers: "the mesh to draw is the
 * first child, or the shared stand-in".
 *
 * @par Callers
 * func_80044260, func_8004C5C8, func_80074104, func_80075A84 and the unit at
 * 0x80075698 - all scene/polygon walkers.
 */
void **func_8003D9C4(struct ParentNode *parent)
{
    struct ChildNode *node;
    void **result;

    node = parent->node;
    if (node != 0) {
        result = (void **)((u8 *)node + 4);
        if (node->child != 0) {
            result = (void **)((u8 *)node->child + 4);
        }
    } else {
        result = &parent->fallback;
    }
    return result;
}
