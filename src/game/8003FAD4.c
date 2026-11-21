// Categories: Game Engine > Screen Teardown | Graphics > Scene List | Graphics > Display Setup
#include "game/types.h"

struct Unk3BObject;

typedef struct UnkFAD4Node {
    struct UnkFAD4Node *next;
    struct Unk3BObject *object;
} UnkFAD4Node;

typedef struct UnkFAD4Local {
    u32 color[2];
    s16 rect[4];
} UnkFAD4Local;

extern UnkFAD4Node *gDeferredRegList __asm__("D_8009F708");
extern s32 gDeferredRegPool __asm__("D_8009F70C");
extern s32 gGreyTexpageAdd __asm__("D_8009F9B8");
extern s32 gGreyTexpageSub __asm__("D_8009FAA0");

extern void gMarkSpawnRecordChains(s32 arg0) __asm__("func_8003C590");
extern void gSpawnEntityPrimaryType(struct Unk3BObject *object) __asm__("func_8003BC0C");
extern void func_80019584(s32 arg0);
extern void gLoadImage(s16 *rect, u32 *color) __asm__("func_80092F38");
extern void func_80092C8C(s32 arg0);
void gFinishLevelLoad(void) __asm__("func_8003FAD4");


/**
 * @brief Clears the screen's scene: drops the two render lists, releases every
 *        object of the per-screen list and paints the backdrop colour.
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * 1. func_8003C590(0) and func_8003C590(1) - reset/drop the two render lists
 *    (buffer index 0 and 1).
 * 2. Walk the singly linked list D_8009F708 ({next, object}) and call
 *    func_8003BC0C(object) per node - the objects registered for this screen
 *    are released/closed.
 * 3. func_80019584(D_8009F70C) releases the list anchor block.
 * 4. D_8009F9B8 = 0x13E, D_8009FAA0 = 0x15E (two colour/limit values used by
 *    the renderer), D_8009F70C = 0 and D_8009F708 = 0 clear the list state.
 * 5. A rectangle (0x380, 0x100, 2, 2) is emitted with two colour words
 *    (0xC210C210 each) through func_80092F38 and the GPU is synchronised with
 *    func_80092C8C(0).
 *
 * @par Globals
 * D_8009F708 (object list head), D_8009F70C (list anchor), D_8009F9B8,
 * D_8009FAA0 (renderer limits/colours).
 *
 * @par Likely role
 * The end-of-screen cleanup that the front-end runs right after the module has
 * been started (func_800287F4 calls it unconditionally), i.e. "forget the
 * previous screen's scene before the new one fills the lists".
 *
 * @par Unknown
 * The exact purpose of the 2x2 rectangle and of the 0xC210C210 colour constant
 * (the same value twice suggests a 16-bit pair, e.g. a fill and a clear value).
 *
 * @par Environment path
 * Post-load finalisation run once per module start by gRunScreenModule
 * (func_800287F4, 800287F4.c:82), after the module has been started and
 * before gRunGameFrameLoop. It runs func_8003C590 on banks 0 and 1 of
 * gSpawnRecordMap, which walks the definition chains and sets flag bit 2 in
 * D_800A6578; it then applies every registration that func_8003FA34 deferred
 * during loading by calling func_8003BC0C (which spawns through
 * gSpawnTypeAtOwner) for each {next, object} node of D_8009F708, releases the
 * node pool D_8009F70C with func_80019584 and clears both. Finally it sets
 * D_8009F9B8 = 0x13E and D_8009FAA0 = 0x15E and uploads a 2x2 block of the
 * 15-bit colour 0xC210 to VRAM (0x380, 0x100) with func_80092F38, followed by
 * a draw sync (func_80092C8C); by the PS1 texpage layout both words address
 * that block as a 15-bit page, with blend mode 1 (additive) and 2
 * (subtractive) respectively.
 */
void gFinishLevelLoad(void)
{
    UnkFAD4Node *node = gDeferredRegList;
    UnkFAD4Local local;

    gMarkSpawnRecordChains(0);
    gMarkSpawnRecordChains(1);

    while (node != 0) {
        gSpawnEntityPrimaryType(node->object);
        node = node->next;
    }

    func_80019584(gDeferredRegPool);

    local.color[0] = 0xC210C210;
    local.color[1] = 0xC210C210;
    gGreyTexpageAdd = 0x13E;
    gGreyTexpageSub = 0x15E;
    local.rect[0] = 0x380;
    local.rect[1] = 0x100;
    gDeferredRegPool = 0;
    gDeferredRegList = 0;
    local.rect[2] = 2;
    local.rect[3] = 2;

    gLoadImage(local.rect, local.color);
    func_80092C8C(0);
}
