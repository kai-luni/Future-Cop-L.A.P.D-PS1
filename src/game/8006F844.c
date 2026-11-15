// Categories: Graphics > Effects > World Quads | Graphics > Scene Graph > Object List Walk
#include "game/types.h"

struct PoolNode {
    struct PoolNode *next;
};

extern struct PoolNode *D_800A353C;
extern void func_8006F310(struct PoolNode *node, s32 arg);

/**
 * @brief Walks the effect list at D_800A353C and processes every node.
 *
 * @param arg Forwarded to the per-node handler (the view position triple
 *            in the scene driver).
 * @return Nothing.
 *
 * @par Confirmed
 * for (node = D_800A353C; node; node = node->next)
 * func_8006F310(node, arg) - the world-quad pass.
 *
 * @par Callers
 * func_8003F6E4 (scene traversal driver).
 */
void func_8006F844(s32 arg)
{
    struct PoolNode *node;

    for (node = D_800A353C; node != 0; node = node->next) {
        func_8006F310(node, arg);
    }
}
