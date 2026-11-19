// Categories: Graphics > Effects > World Quads | Graphics > Scene Graph > Object List Walk
#include "game/types.h"

struct PoolNode {
    struct PoolNode *next;
};

extern struct PoolNode *gTracerActiveList __asm__("D_800A353C");
extern void gDrawTracerBand(struct PoolNode *node, s32 arg) __asm__("func_8006F310");
void gDrawTracerList(s32 arg) __asm__("func_8006F844");


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
 *
 * @par Environment path
 * Draw pass for the beam/tracer pool: walks D_800A353C, which is the
 * active-list field (+4) of the pool header D_800A3538 ticked by
 * func_8006F170, and calls func_8006F310 for each node with the view
 * position. Its caller is gRunViewScenePasses (func_8003F6E4.c:87,
 * func_8006F844(&position)). It is an effect pass that consumes the camera
 * position, not a camera or level-construction routine.
 */
void gDrawTracerList(s32 arg)
{
    struct PoolNode *node;

    for (node = gTracerActiveList; node != 0; node = node->next) {
        gDrawTracerBand(node, arg);
    }
}
