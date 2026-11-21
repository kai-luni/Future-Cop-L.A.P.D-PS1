// Categories: Graphics > Renderer > Scene Traversal | Graphics > Scene Graph
#include "game/types.h"

struct DestinationTriple {
    s32 second;
    s32 zero;
    s32 first;
};

struct Unk8009F9A0 {
    u8 unk00[0x50];
    struct DestinationTriple position;
};

struct Unk800A1070 {
    u8 unk00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
};

extern struct Unk8009F9A0 *gFocusPositions __asm__("D_8009F9A0");
extern struct Unk800A1070 gListHeads __asm__("D_800A1070");

extern void gGetViewGroundAnchor(u32 index, struct DestinationTriple *destination) __asm__("func_80069580");
extern void gDrawBeamNodeList(s32 arg) __asm__("func_80073700");
extern void gDrawTracerList(s32 arg) __asm__("func_8006F844");
extern void gDrawDomeEffectSlots(s32 arg) __asm__("func_8005E160");
extern void gDrawNodeListModels(s32 arg) __asm__("func_8003CF6C");
extern void gDrawDebrisFragments(s32 arg0, struct DestinationTriple *arg1) __asm__("func_8004C5C8");
extern void gDrawAttachedModelList(s32 arg0, struct DestinationTriple *arg1) __asm__("func_8006E988");
extern void gDrawPointTileList(s32 arg0, struct DestinationTriple *arg1) __asm__("func_800445B0");
extern void gDrawGlowNodeList(s32 arg0, struct DestinationTriple *arg1) __asm__("func_80070158");
extern void gDrawRibbonList(s32 arg0, struct DestinationTriple *arg1) __asm__("func_80074104");
void gRunViewScenePasses(s32 arg0) __asm__("func_8003F6E4");


/**
 * @brief Scene traversal driver: resolves one view position and runs the
 *        object dispatcher plus the layer passes over it, in fixed order.
 *
 * @param arg0 View selector; only used when D_8009F9A0 is null, then passed
 *             to func_80069580() to compute the position.
 * @return Nothing.
 *
 * @par Confirmed sequence
 * 1. If D_8009F9A0 != 0, copy its position triple (offset 0x50); otherwise
 *    func_80069580(arg0, &position) fills the local triple.
 * 2. func_80073700(&position) - the object dispatcher.
 * 3. func_8006F844(&position), func_8005E160(&position),
 *    func_8003CF6C(&position) - the depth-cue pass.
 * 4. func_8004C5C8(D_800A1070.unk10, &position),
 *    func_8006E988(D_800A1070.unk08, &position),
 *    func_800445B0(D_800A1070.unk0C, &position),
 *    func_80070158(D_800A1070.unk04, &position),
 *    func_80074104(D_800A1070.unk14, &position).
 *
 * @par Likely role
 * A sibling of func_8003F4A0 for a second view: the five words of
 * D_800A1070 select the list or mode of each attached pass.
 *
 * @par Callers
 * func_80066E98 (region drawing).
 *
 * @par Environment path
 * Resolves the view position (from gFocusPositions, otherwise func_80069580)
 * and then runs nine list passes in a fixed order: three position-filling
 * passes (func_80073700, func_8006F844, func_8005E160) followed by the model,
 * debris, glow and ribbon lists. gDrawViewLayers calls it last, which is
 * where actors and effects appear over the terrain; it does not run the
 * region or scene-list passes.
 */
void gRunViewScenePasses(s32 arg0)
{
    struct DestinationTriple position;
    register struct Unk8009F9A0 *p __asm__("$6");

    p = gFocusPositions;
    if (p != 0) {
        position = p->position;
    } else {
        gGetViewGroundAnchor(arg0, &position);
    }
    gDrawBeamNodeList((s32)&position);
    gDrawTracerList((s32)&position);
    gDrawDomeEffectSlots((s32)&position);
    gDrawNodeListModels((s32)&position);
    gDrawDebrisFragments(gListHeads.unk10, &position);
    gDrawAttachedModelList(gListHeads.unk08, &position);
    gDrawPointTileList(gListHeads.unk0C, &position);
    gDrawGlowNodeList(gListHeads.unk04, &position);
    gDrawRibbonList(gListHeads.unk14, &position);
}
