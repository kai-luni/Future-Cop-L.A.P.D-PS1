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

void func_800193F8(struct Vec3s32 *axis, struct Basis *basis)
{
    basis->rows[0].x = axis->z;
    basis->rows[0].y = 0;
    basis->rows[0].z = -axis->x;
    func_8001A180(&basis->rows[0]);
    func_80010974(axis, &basis->rows[0], &basis->rows[1]);
    basis->rows[2] = *axis;
}
