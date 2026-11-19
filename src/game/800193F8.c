// Categories: Graphics > Geometry | Utilities > Mathematics
#include "game/types.h"

extern int gAttachPartEffect();
extern int gProjectileInit();

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
void gBuildBasisFromAxis(struct Vec3s32 *axis, struct Basis *basis) __asm__("func_800193F8");


/**
 * @brief Builds an orthonormal basis whose third row is the given axis,
 *        with a horizontal first row (full-angle variant).
 *
 * @param axis Direction vector (not necessarily normalized).
 * @param basis Output basis, three rows of three s32.
 * @return Nothing.
 *
 * @par Confirmed
 * row0 = (axis->z, 0, -axis->x), normalized by func_8001A180;
 * row1 = func_80010974(axis, row0) (cross product); row2 = *axis.
 *
 * @par Likely role
 * Orientation frame for movement directions: the horizontal perpendicular
 * keeps the roll level. func_8001937C is the small-angle twin with the
 * perpendicular in the y/z plane.
 *
 * @par Callers
 * func_80032180, func_80035698, gProjectileInit, func_80036E6C,
 * func_80037064, gAttachPartEffect.
 *
 * @par Environment path
 * Builds a 3x3 basis whose third row is the given axis: the first row is the
 * horizontal perpendicular (axis.z, 0, -axis.x) normalised by
 * gNormalizeVector (func_8001A180), the second is the GTE cross product of
 * the axis and that row. Callers are func_80032180, func_80035698,
 * func_80035904, gHandleProjectileMsg, func_80037064 and gAttachPartEffect.
 * It is orientation math for projectiles and effects; no camera unit calls
 * it.
 */
void gBuildBasisFromAxis(struct Vec3s32 *axis, struct Basis *basis)
{
    basis->rows[0].x = axis->z;
    basis->rows[0].y = 0;
    basis->rows[0].z = -axis->x;
    gNormalizeVector(&basis->rows[0]);
    gGteCrossProduct(axis, &basis->rows[0], &basis->rows[1]);
    basis->rows[2] = *axis;
}
