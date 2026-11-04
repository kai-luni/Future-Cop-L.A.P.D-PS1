#include "game/types.h"

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

extern s32 func_8001A4E8(s32 *a, s32 *b);
extern void func_8001A180(s32 *v);

void func_8005E2A0(Vec3 *target, Vec3 *out, s32 arg2, s32 arg3,
                   Vec3 *start, Vec3 *dir, s32 scale)
{
    Vec3 d;
    Vec3 p;
    s32 travelled;
    s32 step;
    s32 i;
    s32 dist;
    s32 mx, my, mz;

    d = *dir;
    mx = d.x * scale;
    my = d.y * scale;
    mz = d.z * scale;

    travelled = 0;
    step = arg2 << 6;
    i = ((scale << 4) < arg2) << 3;

    d.x = mx >> 10;
    d.y = my >> 10;
    d.z = mz >> 10;

    p = *start;

    do {
        i++;
        travelled += step;
        p.x += d.x;
        p.y += d.y;
        p.z += d.z;
        dist = func_8001A4E8((s32 *)&p, (s32 *)target);
        if (travelled >= dist) {
            break;
        }
        if (travelled >= arg3) {
            break;
        }
    } while (i < 9);

    p.x -= target->x;
    p.y -= target->y;
    p.z -= target->z;
    d.x >>= 1;
    d.y >>= 1;
    d.z >>= 1;
    p.x -= d.x;
    p.y -= d.y;
    p.z -= d.z;
    func_8001A180((s32 *)&p);

    *out = p;
}
