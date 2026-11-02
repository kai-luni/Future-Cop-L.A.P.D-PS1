#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

extern s32 func_800684AC(struct Vec3s32 *arg0, struct Vec3s32 *arg1,
                         struct Vec3s32 *arg2, s32 arg3, s32 arg4);

s32 func_80055EE8(struct Vec3s32 *from, struct Vec3s32 *to)
{
    struct Vec3s32 start;
    struct Vec3s32 end;
    struct Vec3s32 hit;
    struct Vec3s32 mid;
    struct Vec3s32 *p;
    register s32 result __asm__("$4");
    register s32 flag __asm__("$17") = 1;

    start = *from;
    end = *to;

    mid.x = (start.x + end.x) >> 1;
    p = &start;
    mid.y = (p->y + end.y) >> 1;
    mid.z = (p->z + end.z) >> 1;

    if (p->y <= 0x7FFF) {
        while (mid.y > 0x8000) {
            end = mid;
            mid.x = (p->x + end.x) >> 1;
            mid.y = (p->y + end.y) >> 1;
            mid.z = (p->z + end.z) >> 1;
        }
        flag = 1;
    }

    result = func_800684AC(&end, &start, &hit, 0, flag);
    if (result & 0x8000) {
        result = func_800684AC(&start, &hit, 0, 0, flag);
        if (result & 0x8000) {
            return 0;
        }
    }
    return result;
}
