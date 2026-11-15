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

extern void func_80010974(struct Vec3s32 *a, struct Vec3s32 *b, struct Vec3s32 *out);
extern void func_8001A180(struct Vec3s32 *vector);

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
 * func_80032180, func_80035698, func_80035904, func_80036E6C,
 * func_80037064, func_8005CBCC.
 */
void func_800193F8(struct Vec3s32 *axis, struct Basis *basis)
{
    basis->rows[0].x = axis->z;
    basis->rows[0].y = 0;
    basis->rows[0].z = -axis->x;
    func_8001A180(&basis->rows[0]);
    func_80010974(axis, &basis->rows[0], &basis->rows[1]);
    basis->rows[2] = *axis;
}
