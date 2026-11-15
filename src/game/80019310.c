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

extern void func_80010974(struct Vec3s32 *a, struct Vec3s32 *b, struct Vec3s32 *out);
extern void func_8001A180(struct Vec3s32 *vector);

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
 */
void func_80019310(struct Vec3s32 *first, struct Vec3s32 *second, struct Basis *basis)
{
    func_80010974(second, first, &basis->rows[0]);
    func_8001A180(&basis->rows[0]);
    func_80010974(first, &basis->rows[0], &basis->rows[1]);
    basis->rows[2] = *first;
}
