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
extern void func_8001A180(struct Vec3s32 *vector);

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
 */
void func_8001937C(struct Vec3s32 *axis, struct Basis *basis)
{
    basis->rows[0].x = 0;
    basis->rows[0].y = -axis->z;
    basis->rows[0].z = axis->x;
    func_8001A180(&basis->rows[0]);
    gGteCrossProduct(axis, &basis->rows[0], &basis->rows[1]);
    basis->rows[2] = *axis;
}
