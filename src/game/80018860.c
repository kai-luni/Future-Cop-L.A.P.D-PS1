// Categories: Graphics > GTE > Matrix Build
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Matrix32 {
    s32 m[3][3];
};

struct Matrix {
    struct Matrix32 rotation;
    struct Vec3s32 translation;
};

extern void func_80019470(struct Matrix32 *source, struct Matrix32 *destination);
extern void func_8001A81C(struct Matrix32 *matrix, struct Vec3s32 *input, struct Vec3s32 *output);

/**
 * @brief Converts a rotation-plus-translation pose into the inverse view
 *        matrix (transposed rotation, negated rotated translation).
 *
 * @param rotation Source pose: 3x3 rotation followed by the translation
 *        triple.
 * @param matrix Destination view matrix, same layout.
 * @return Nothing.
 *
 * @par Confirmed
 * func_80019470 transposes the rotation into the destination;
 * func_8001A81C rotates the source translation by it, then all three
 * components are negated in place.
 *
 * @par Likely role
 * The world-to-view conversion behind the camera rotation setter
 * func_80017860 and the collision matrix path func_80061CAC: the standard
 * rigid-body inverse (R^T, -R^T t).
 *
 * @par Callers
 * func_80017860, func_80061CAC.
 */
void func_80018860(u8 *rotation, u8 *matrix)
{
    struct Matrix *rot = (struct Matrix *)rotation;
    struct Matrix *mat = (struct Matrix *)matrix;
    struct Vec3s32 *t;

    func_80019470(&rot->rotation, &mat->rotation);
    t = &mat->translation;
    func_8001A81C(&mat->rotation, &rot->translation, t);
    mat->translation.x = -mat->translation.x;
    t->y = -t->y;
    t->z = -t->z;
}
