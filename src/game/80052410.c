// Categories: Gameplay > Collision | Graphics > Geometry
#include "game/types.h"

struct Vec52410 { s32 x, y, z; };
struct Obj52410 {
    u8 pad00[0x2c];
    u8 view[0x74];
    struct Vec52410 target;
    u8 *other;
    s16 angle;
};

extern void gBuildObjectBasis(struct Obj52410 *, s32, struct Vec52410 *, void *) __asm__("func_80051C50");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *, struct Vec52410 *, s32) __asm__("func_80010234");
extern s32 func_800684AC(void *, void *, void *, s32, s32);
s32 gTestFootprintEdges(struct Obj52410 *obj, struct Vec52410 *points, struct Vec52410 *out) __asm__("func_80052410");


/**
 * @brief Transforms a quad and tests its four edges.
 *
 * @param obj Object with rotation angle, target and view vectors.
 * @param points Four input vertices.
 * @param out Result block: the transformed vertices at +4.. and the per-edge
 *        results at +0x10.
 * @return The accumulated per-edge result of func_800684AC.
 *
 * @par Confirmed
 * func_80051C50(obj, obj->angle, target, view) prepares the matrices,
 * func_80010750 loads one and func_80010234 transforms four vertices. Two loops
 * then call func_800684AC(&points[i], &points[(i + 1) & 3], &out[i + 4], 0, 0) and
 * accumulate `& 7` - four calls, one per edge of the quad. The block offsets
 * 0x20/0x2C/0x74/0xEC belong to the object and result records.
 *
 * @par Likely role
 * The footprint test of a vehicle or building: each edge is checked against the
 * level geometry and the four codes say which sides are blocked.
 *
 * @par Environment path
 * Builds the object's basis with func_80051C50, loads it with func_80010750
 * and transforms the four footprint corners of the record at other+0xEC with
 * func_80010234. It then runs the map-grid segment test func_800684AC eight
 * times - four rays from the footprint's reference points to the transformed
 * corners and the four quad edges between the corners - and packs the eight
 * 3-bit hit codes into one return word, writing the hit points to the out
 * array. Its only caller func_80052708 uses that word as the footprint
 * collision mask; this is collision, not rendering.
 */
s32 gTestFootprintEdges(struct Obj52410 *obj, struct Vec52410 *points, struct Vec52410 *out)
{
    s32 i;
    register s32 result __asm__("$18");
    u8 *base = obj->other + 0xec;
    register struct Vec52410 *p __asm__("$16");
    register struct Vec52410 *q __asm__("$19");
    s32 offset;
    register struct Vec52410 *target __asm__("$6");
    register struct Vec52410 *view __asm__("$7");

    target = &obj->target;
    __asm__("" : "+r"(target));
    p = (struct Vec52410 *)obj->view;
    view = p;
    __asm__("" : "+r"(view));
    result = 0;
    i = result;
    offset = 0x20;
    gBuildObjectBasis(obj, obj->angle, target, view);
    gGteLoadMatrix(p);
    gGteTransformVerticesWide(base, points, 4);
    q = out;
    p = points;
    for (; i < 4; i++) {
        result <<= 3;
        result |= func_800684AC(base + offset, p, q, 0, 0) & 7;
        q++;
        p++;
        offset += 12;
    }
    for (i = 0; i < 4; i++) {
        result <<= 3;
        result |= func_800684AC(&points[i], &points[(i + 1) & 3], &out[i + 4], 0, 0) & 7;
    }
    return result;
}
