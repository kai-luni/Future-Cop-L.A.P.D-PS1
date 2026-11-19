// Categories: Graphics > Geometry | Utilities > Mathematics
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Basis {
    struct Vec3s32 rows[3];
};

extern void gGteCrossProduct(struct Vec3s32 *a, struct Vec3s32 *b, struct Vec3s32 *out) __asm__("func_80010974");
extern void gNormalizeVector(struct Vec3s32 *vector) __asm__("func_8001A180");
void gBuildBasisFromAxisYZ(struct Vec3s32 *axis, struct Basis *basis) __asm__("func_8001937C");


/**
 * @brief Builds an orthonormal basis whose third row is the given axis,
 *        with the first row in the y/z plane (small-angle variant).
 *
 * @param axis Direction vector (not necessarily normalized).
 * @param basis Output basis, three rows of three s32.
 * @return Nothing.
 *
 * @par Confirmed
 * row0 = (0, -axis->z, axis->x), normalized by func_8001A180;
 * row1 = func_80010974(axis, row0) (cross product); row2 = *axis.
 *
 * @par Likely role
 * The near-axis-aligned case of func_800193F8: func_80036E6C picks this
 * form when |dx| + |dy| < 0x10, where the horizontal perpendicular of the
 * full variant would degenerate.
 *
 * @par Callers
 * func_80036E6C.
 *
 * @par Environment path
 * Builds a 3x3 basis whose third row is the axis, with the first row (0,
 * -axis.z, axis.x) normalised by func_8001A180 and the second row the GTE
 * cross product axis x row0. Its only caller is gHandleProjectileMsg
 * (func_80036E6C, 80036E6C.c:152), which uses it instead of func_800193F8
 * when |axis.x| + |axis.y| < 0x10, i.e. for a direction close to the z axis.
 * Row 0 is perpendicular to the axis only in that case (their dot product is
 * axis.z * (axis.x - axis.y)), so the result is orthonormal only for near-z
 * directions.
 */
void gBuildBasisFromAxisYZ(struct Vec3s32 *axis, struct Basis *basis)
{
    basis->rows[0].x = 0;
    basis->rows[0].y = -axis->z;
    basis->rows[0].z = axis->x;
    gNormalizeVector(&basis->rows[0]);
    gGteCrossProduct(axis, &basis->rows[0], &basis->rows[1]);
    basis->rows[2] = *axis;
}
