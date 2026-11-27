// Categories: Graphics > Lighting > Scene Node | Resources > Memory Management
#include "game/types.h"

struct Unk75478 {
    u8 unk00[0x24];
    void *light;
};

extern void gFreeLightGridNode(void *object) __asm__("func_800498F0");
void gReleaseParticleLight(struct Unk75478 *object) __asm__("func_80075478");


/**
 * @brief Releases the light object attached to a scene node.
 *
 * @param object Node; the light pointer sits at +0x24.
 * @return Nothing. The pointer is cleared, so a second call does nothing.
 *
 * @par Confirmed
 * if (object->light != 0) { func_800498F0(object->light); object->light = 0; }
 *
 * @par Callers
 * func_80075A84 and the unit at 0x80075698 - the node teardown of the polygon
 * renderer.
 *
 * @par Environment path
 * If the light-node pointer at +0x24 of a particle is non-null, releases it
 * through func_800498F0 (which unlinks the node from its D_8009F780 map-cell
 * bucket list and returns it to the D_8009F784 pool) and clears the pointer,
 * so a second call is a no-op. gRunPolygonPass (func_80075A84) calls it at
 * 80075A84.c:287 for a dying particle of an emitter whose flag word +0x8E has
 * bit 2, just before the particle itself is returned to the pool with
 * func_8003BF1C; gInitEmitterParticle (func_800754B8) calls it at
 * 800754B8.c:375 on a recycled pool node. It is the teardown counterpart of
 * gUpdateParticleLight (func_800753D0)/func_800752E0, which create and move
 * the same +0x24 light node.
 */
void gReleaseParticleLight(struct Unk75478 *object)
{
    if (object->light != 0) {
        gFreeLightGridNode(object->light);
        object->light = 0;
    }
}
