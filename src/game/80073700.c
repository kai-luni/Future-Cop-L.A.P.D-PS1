// Categories: Graphics > Model Renderer > Scene Traversal
#include "game/types.h"

struct Unk73700 {
    struct Unk73700 *next;
    void *first;
    void *second;
};

extern struct Unk73700 *gModelNodeList __asm__("D_8009F38C");

extern void gDrawBeamNode(struct Unk73700 *node, s32 arg) __asm__("func_80071FA4");
extern void func_80072F7C(struct Unk73700 *node, s32 arg);

/**
 * @brief Walks the global model list and dispatches each node to one of the two
 *        renderer entry points.
 *
 * @param arg Forwarded unchanged to the renderer (a frame/time or state
 *            argument - both callees take it as @c s32).
 *
 * @par Confirmed flow
 * for (node = D_8009F38C; node; node = node->next):
 * - node->first or node->second set  -> func_80071FA4(node, arg)
 * - both NULL                        -> func_80072F7C(node, arg)
 *
 * @par Likely meaning
 * The two callees are the two model renderers (func_80071FA4 is the large
 * polygon path); the two pointers at +4 and +8 select which one owns the node -
 * an object with geometry goes through the full path, an empty one through the
 * cheap one.
 *
 * @par Callers
 * func_8003F6E4.
 */
void func_80073700(s32 arg)
{
    struct Unk73700 *node;

    for (node = gModelNodeList; node != 0; node = node->next) {
        if (node->first != 0 || node->second != 0) {
            gDrawBeamNode(node, arg);
        } else {
            func_80072F7C(node, arg);
        }
    }
}
