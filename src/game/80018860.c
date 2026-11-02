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
