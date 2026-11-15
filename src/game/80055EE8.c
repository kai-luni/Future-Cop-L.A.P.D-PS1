// Categories: Graphics > Camera > Visibility | Gameplay > Map > Grid Raycast
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

extern s32 func_800684AC(struct Vec3s32 *arg0, struct Vec3s32 *arg1,
                         struct Vec3s32 *arg2, s32 arg3, s32 arg4);

/**
 * @brief Camera line-of-sight test with a ceiling clamp, built on the map
 *        grid raycast.
 *
 * @param from View target position.
 * @param to Candidate camera position.
 * @return 0 when the view is clear, else the raycast result.
 *
 * @par Confirmed
 * The midpoint is computed; when from.y <= 0x7FFF the end point is bisected
 * toward the start until mid.y <= 0x8000 (keeps the ray below the ceiling
 * plane). func_800684AC(end, start, &hit, 0, flag) casts; on a hit
 * (result & 0x8000) a second cast start -> hit checks the near segment;
 * only when that also hits is the view reported blocked (0 returned for
 * the clear case of the second cast).
 *
 * @par Likely role
 * The obstruction test of the chase camera (func_80056870's preset search
 * and func_800569EC): the ceiling clamp avoids false hits against the sky
 * box, the two-segment test distinguishes near from far geometry.
 *
 * @par Unknown
 * The exact meaning of the raycast's 0x8000 result bit.
 *
 * @par Callers
 * func_80056870, func_800569EC.
 */
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
