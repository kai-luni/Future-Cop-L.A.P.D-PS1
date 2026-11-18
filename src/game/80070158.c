// Categories: Graphics > Model Renderer > Node Walk | Graphics > GTE > Projection
#include "game/types.h"

struct Vec70158 {
    s32 x;
    s32 y;
    s32 z;
};

struct Sub70158 {
    u8 unk00[0x34];
    void *model;
};

struct Node70158 {
    struct Node70158 *next;
    u8 unk04[0x0A];
    s8 unk0E;
    u8 unk0F;
    u8 unk10[2];
    s16 unk12;
    u8 unk14[2];
    u16 unk16;
    u8 unk18[2];
    u16 unk1A;
    struct Sub70158 *unk1C;
    u8 unk20[0x14];
    u8 unk34[0x44];
    s32 unk78;
    s32 unk7C;
    u8 unk80[0x0C];
    u16 unk8C;
    u8 unk8E;
    u8 unk8F;
    s16 unk90;
};

struct Camera70158 {
    u8 unk00[0x28];
    u8 unk28[0x54];
    s32 unk7C;
    s32 unk80;
    s32 unk84;
};

struct Transform70158 {
    u8 unk00[0x24];
    s32 x;
    s32 y;
    s32 z;
};

extern struct Camera70158 *gCamera __asm__("D_8009F9E4");

extern struct Transform70158 *gResolveNodeTransform(void *model, s32 flags, s32 part,
                                            s32 count, void *sub, void *matrix) __asm__("func_8003D9F8");
extern s32 func_8001A100(s32 *vector);
extern void gGteLoadRotation(void *matrix) __asm__("func_80010820");
extern void gGteTransformVertices(void *src, void *dst, s32 arg2) __asm__("func_8001010C");
extern s32 gTestFrustumFixedNear(s16 *point, s32 margin) __asm__("func_80019D90");
extern void func_80010704(u32 *src, u32 *dst);
extern void gEmitRayBurst(void *dst, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6) __asm__("func_80014490");
extern void gEmitGouraudDisc(void *dst, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6) __asm__("func_8001484C");
void gDrawGlowNodeList(struct Node70158 *node, s32 *origin) __asm__("func_80070158");


/**
 * @brief Walks a model node list, culls by radius and draws the textured nodes.
 *
 * @param node First node: next at +0, sub at +0x1C, fields at +0x0E/+0x12/+0x1A,
 *        matrix at +0x34, geometry at +0x78..+0x90.
 * @param origin Camera/origin block.
 * @return Nothing.
 *
 * @par Confirmed
 * Per node: func_8003D9F8(model, n->unk1A, n->unk0E, 0, sub, &n->unk34) resolves the
 * transform, `func_8001A100(vec) - (n->unk12 << 4) > 0x1FFFF` culls distant nodes,
 * func_80010820(&camera->unk28) loads the camera matrix and func_8001010C transforms
 * one vertex; func_80019D90(dst, n->unk12 << 4) & 0xFF must return exactly 2 (the
 * on-screen test), then func_80010704 (RTPS) projects and func_8001484C /
 * func_80014490 emit the packet with the node's fields.
 *
 * @par Likely role
 * The textured-node pass of the scene walker: it is the RTPS-based sibling of
 * func_80075A84 and is driven by the same render lists.
 *
 * @par Environment path
 * Walks the node list from the argument, resolves each node's transform with
 * func_8003D9F8, culls it by radius against the origin, converts the centre
 * to camera space and projects it as a single point through func_80010820,
 * func_8001010C and func_80010704. Nodes that pass the on-screen test emit a
 * gouraud disc with func_8001484C, or its rotating variant func_80014490 when
 * the node flag asks for it, using the node's colour, size and depth fields.
 * It is called by gRunViewScenePasses (func_8003F6E4) at
 * src/game/8003F6E4.c:90, the step that closes the environment pass
 * gDrawViewLayers opens, so it draws the glow/light nodes that belong to the
 * visible region.
 */
void gDrawGlowNodeList(struct Node70158 *node, s32 *origin)
{
    register struct Node70158 *n __asm__("$16");
    register s32 *org __asm__("$21");
    struct Sub70158 *sub;
    struct Transform70158 *t;
    struct Camera70158 *camera;
    void *model;
    s32 vec[3];
    u32 dst[2];
    s16 src[3];
    s32 color;

    n = node;
    org = origin;
    for (; n != 0; n = n->next) {
        if (n->unk0F & 8) {
            continue;
        }

        sub = n->unk1C;
        model = 0;
        if (sub != 0) {
            model = sub->model;
        }
        t = gResolveNodeTransform(model, n->unk1A, n->unk0E, 0, sub, &n->unk34);

        vec[0] = t->x - org[0];
        vec[1] = t->y - org[1];
        vec[2] = t->z - org[2];
        if (func_8001A100(vec) - (n->unk12 << 4) > 0x1FFFF) {
            continue;
        }

        camera = gCamera;
        vec[0] = t->x - camera->unk7C;
        vec[1] = t->y - camera->unk80;
        vec[2] = t->z - camera->unk84;
        src[0] = vec[0] >> 4;
        src[1] = vec[1] >> 4;
        src[2] = vec[2] >> 4;
        gGteLoadRotation(&camera->unk28);

        gGteTransformVertices(src, dst, 1);
        if ((gTestFrustumFixedNear(dst, n->unk12 << 4) & 0xFF) != 2) {
            continue;
        }
        func_80010704(src, dst);

        if (n->unk8C & 2) {
            gEmitGouraudDisc(dst, n->unk78, n->unk7C, n->unk8E, n->unk12, 0x200,
                          (n->unk90 << 8) | n->unk8F);
        } else if (n->unk8C & 4) {
            gEmitRayBurst(dst, n->unk78, n->unk7C, n->unk8E, n->unk12, 0x200,
                          (n->unk90 << 8) | n->unk8F);
        } else {
            gEmitGouraudDisc(dst, n->unk78, n->unk7C, n->unk8E, n->unk12, 0x200, 0);
        }
    }
}
