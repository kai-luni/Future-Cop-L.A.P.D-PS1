// Categories: Graphics > Geometry > Basis Construction | Utilities > Mathematics
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
void gBuildBasisFromPair(struct Vec3s32 *first, struct Vec3s32 *second, struct Basis *basis) __asm__("func_800192AC");


/**
 * @brief Builds a right-handed 3x3 basis from two vectors.
 *
 * @param first, second Input vectors.
 * @param basis Output basis: rows[0] = first × second normalised, rows[2] =
 *        rows[0] × first normalised, rows[1] = first.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80010974 (GTE cross product) then func_8001A180 (normalise in place) for
 * rows[0]; a second cross with `first` gives rows[2]; `first` itself becomes
 * rows[1].
 *
 * @par Compare
 * func_80019310 builds the mirrored basis (rows[0] = second × first, rows[2] =
 * first × rows[0], rows[1] = second is replaced by `first` at rows[2] - the other
 * handedness).
 *
 * @par Callers
 * The camera and object-transform code (func_80051C50, func_80053A74).
 *
 * @par Environment path
 * Builds a 3x3 basis from two vectors: rows[0] is first x second normalised
 * by func_8001A180, rows[2] is rows[0] x first (not normalised again) and
 * rows[1] is first. The call graph lists gInitType11Entity (func_8002DC94)
 * and gOrientEntityBasis (func_80037EB4) as callers, not the
 * func_80051C50/func_80053A74 named in the file comment, and the comment's
 * 'rows[2] normalised' is not what the code does. It is entity orientation
 * math, adjacent to but not part of the camera path.
 */
void gBuildBasisFromPair(struct Vec3s32 *first, struct Vec3s32 *second, struct Basis *basis)
{
    gGteCrossProduct(first, second, &basis->rows[0]);
    gNormalizeVector(&basis->rows[0]);
    gGteCrossProduct(&basis->rows[0], first, &basis->rows[2]);
    basis->rows[1] = *first;
}
