// Categories: Graphics > Renderer > Scene Traversal | Game Engine > Frame
#include "game/types.h"

extern void *gListHeads[] __asm__("D_800A1070");
extern void func_8007364C(s32 arg);
extern void func_8006F170(void *arg);
extern void func_8006D770(void *a0, void *a1);
extern void func_800441DC(void *a0, void *a1);
extern void func_80040A00(void *a0, void *a1);
extern void func_80070048(void *a0, void *a1);
extern void func_8004C228(void *a0, void *a1);
extern void gRunPolygonPass(void *a0, void *a1) __asm__("func_80075A84");
void gRunSceneListPasses(void *arg) __asm__("func_8003F4A0");


/**
 * @brief Runs all render passes over the six global scene lists, in a fixed
 *        order.
 *
 * @param arg Scene/frame argument, forwarded to every pass.
 *
 * @par Confirmed order
 * func_8007364C(arg), func_8006F170(arg), then for i = 2, 3, 0, 1, 4, 5:
 * func_8006D770 / func_800441DC / func_80040A00 / func_80070048 /
 * func_8004C228 / func_80075A84, each called as (D_800A1070[i], arg).
 *
 * @par Likely meaning
 * D_800A1070 is an array of six per-layer object lists (background, level,
 * effects, foreground, ...), and the order is the draw order: the first two
 * passes are global (camera/visibility), the six list passes follow the layer
 * sequence 2, 3, 0, 1, 4, 5.
 *
 * @par Callers
 * func_8003F530.
 *
 * @par Environment path
 * Runs the six per-layer render passes over the global scene lists in a fixed
 * order; the entry a frame uses to draw every object list on top of the
 * environment.
 */
void gRunSceneListPasses(void *arg)
{
    func_8007364C((s32)arg);
    func_8006F170(arg);
    func_8006D770(gListHeads[2], arg);
    func_800441DC(gListHeads[3], arg);
    func_80040A00(gListHeads[0], arg);
    func_80070048(gListHeads[1], arg);
    func_8004C228(gListHeads[4], arg);
    gRunPolygonPass(gListHeads[5], arg);
}
