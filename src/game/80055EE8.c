// Categories: Graphics > Camera > Visibility | Gameplay > Map > Grid Raycast
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

extern s32 gTraceMapRay(struct Vec3s32 *arg0, struct Vec3s32 *arg1,
                         struct Vec3s32 *arg2, s32 arg3, s32 arg4) __asm__("func_800684AC");
s32 gTestCameraSightLine(struct Vec3s32 *from, struct Vec3s32 *to) __asm__("func_80055EE8");


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
 *
 * @par Environment path
 * Line-of-sight test between the view target (from) and a candidate camera
 * eye (to) on top of the map raycast func_800684AC: when from.y <= 0x7FFF the
 * end point is bisected toward the start until the midpoint y is at most
 * 0x8000 (lines 58-66), then the segment end->start is cast and, if the
 * result has bit 0x8000, a second cast start->hit is made; 0 is returned only
 * when that second cast also has bit 0x8000, otherwise the raw raycast
 * result. Both callers treat 0 as an unobstructed view: gUpdateChaseCamera
 * (func_800569EC) uses it at 800569EC.c:373/400/419 to decide whether to keep
 * the current preset or step to the next one, and gFindClearCameraPreset
 * (func_80056870) at 80056870.c:66 to pick the first clear preset slot. It
 * runs inside the per-frame chase-camera update after the candidate eye has
 * been computed by gComputeCameraEye.
 */
s32 gTestCameraSightLine(struct Vec3s32 *from, struct Vec3s32 *to)
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

    result = gTraceMapRay(&end, &start, &hit, 0, flag);
    if (result & 0x8000) {
        result = gTraceMapRay(&start, &hit, 0, 0, flag);
        if (result & 0x8000) {
            return 0;
        }
    }
    return result;
}
