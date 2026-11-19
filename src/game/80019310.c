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
void gBuildBasisZFromPair(struct Vec3s32 *first, struct Vec3s32 *second, struct Basis *basis) __asm__("func_80019310");


/**
 * @brief Builds the mirrored 3x3 basis from two vectors.
 *
 * @param first, second Input vectors.
 * @param basis Output basis: rows[0] = second × first normalised, rows[1] =
 *        first × rows[0] normalised, rows[2] = first.
 * @return Nothing.
 *
 * @par Confirmed
 * Same three steps as func_800192AC, but the cross products are taken in the
 * opposite order and the rows land in the other slots.
 *
 * @par Likely role
 * The left-handed counterpart of func_800192AC; the two are used together where
 * a coordinate system is mirrored (screen-space vs. world-space).
 *
 * @par Environment path
 * Builds a 3x3 basis whose third row is the first vector: rows[0] = second x
 * first normalised by func_8001A180, rows[1] = first x rows[0] (not
 * normalised) and rows[2] = first. It differs from func_800192AC (which puts
 * first into row 1 and uses first x second) in which row receives the given
 * direction, so it orients a frame whose z row follows a motion direction.
 * Callers are gUpdateHoverEnemy (func_800324E0) and gGuidedProjectileUpdate
 * (func_80035B7C), the two flying-unit updaters.
 */
void gBuildBasisZFromPair(struct Vec3s32 *first, struct Vec3s32 *second, struct Basis *basis)
{
    gGteCrossProduct(second, first, &basis->rows[0]);
    gNormalizeVector(&basis->rows[0]);
    gGteCrossProduct(first, &basis->rows[0], &basis->rows[1]);
    basis->rows[2] = *first;
}
