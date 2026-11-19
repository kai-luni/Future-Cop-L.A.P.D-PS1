// Categories: Graphics > Scene Graph > Object List Walk | Renderer
#include "game/types.h"

struct Parent {
    u8 pad[0x34];
    s32 value;
};

struct Object {
    u8 pad[0x24];
    s32 x, y, z;
};

struct Node {
    struct Node *next;
    u8 pad04[0xa];
    s8 type;
    u8 flags;
    u8 pad10[2];
    s16 radius;
    u8 pad14[6];
    u16 param;
    struct Parent *parent;
    u8 pad20[0x14];
    u8 data34[0x55];
    u8 a89, a8a, a8b, a8c, a8d, a8e, a8f;
    u8 pad90[8];
    u16 flags98;
    u8 pad9a[2];
    s32 data9c;
};

extern struct Object *gResolveNodeTransform(s32, u16, s8, s32, struct Parent *, void *) __asm__("func_8003D9F8");
extern s32 gVectorLength3D(s32 *) __asm__("func_8001A100");
extern void gDrawShadedModel(s32, struct Object *, s32, s32, s32, s32, s32, s32, s32) __asm__("func_8006CB58");
extern void gDrawFlatRing(struct Node *, s32 *) __asm__("func_8006DC24");
void gDrawAttachedModelList(struct Node *node, s32 *origin) __asm__("func_8006E988");


/**
 * @brief Walks the model node list and draws each node (or its parent) with a
 *        radius test.
 *
 * @param node First node: next at +0, flags at +0x14/+0x98, parent at +0x24,
 *        per-node data at +0x34/+0x9C, radius, type, param.
 * @param origin Camera/origin block passed to the draw calls.
 * @return Nothing.
 *
 * @par Confirmed
 * While the node exists: nodes without flag bit 3 are skipped; when bit 0x40 of
 * +0x98 is set func_8003D9F8(arg, node->param, node->type, mask, parent,
 * node->data34) produces the object, and if `func_8001A100(vec) - (radius << 4) <=
 * 0x1FFFF` the node is drawn with func_8006CB58(data9c, o) or, for nodes without
 * that data, with func_8006DC24(node, origin).
 *
 * @par Likely role
 * The "second model layer" walker next to func_80073700 - it resolves the parent
 * chain first, which is what attached models need.
 *
 * @par Environment path
 * Walks a linked list of model nodes (next at +0x00) for one view and skips
 * nodes whose flag byte +0x0F has bit 3. A node with bit 0x40 in its +0x98
 * word gets its world transform from gResolveNodeTransform (func_8003D9F8)
 * using the parent at +0x24 (parent+0x34 as first argument), is culled when
 * func_8001A100(position - origin) - (radius << 4) exceeds 0x1FFFF, and is
 * otherwise drawn with gDrawShadedModel (func_8006CB58) using +0x9C and the
 * colour bytes +0x89..+0x8F; every other node is drawn by gDrawFlatRing
 * (func_8006DC24). Its only caller gRunViewScenePasses (func_8003F6E4) runs
 * it each frame per view on the list head at gListHeads+0x08 (8003F6E4.c:91),
 * with the per-view ground anchor or the gFocusPositions override as origin;
 * it uses no globals.
 */
void gDrawAttachedModelList(struct Node *node, s32 *origin)
{
    struct Object *object;
    struct Parent *parent;
    s32 arg;
    register s32 mask __asm__("$2");
    s32 vec[3];

    while (node != 0) {
        if (!(node->flags & 8)) {
            if (node->flags98 & 0x40) {
                arg = 0;
                mask = ((u8 *)&node->param)[0] & 4;
                parent = node->parent;
                if (parent != 0) {
                    arg = parent->value;
                }
                object = gResolveNodeTransform(arg, node->param, node->type, mask, parent, node->data34);
                vec[0] = object->x - origin[0];
                vec[1] = object->y - origin[1];
                vec[2] = object->z - origin[2];
                if (gVectorLength3D(vec) - (node->radius << 4) <= 0x1ffff) {
                    gDrawShadedModel(node->data9c, object, node->radius, node->a89, node->a8a, node->a8b, node->a8d, node->a8e, node->a8f);
                }
            } else {
                gDrawFlatRing(node, origin);
            }
        }
        node = node->next;
    }
}
