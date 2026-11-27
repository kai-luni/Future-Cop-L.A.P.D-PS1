// Categories: Gameplay > Combat > Targeting | Utilities > Mathematics
#include "game/types.h"

extern int gAttachPartEffect();
extern int gWeaponFireDispatch();

typedef struct {
    s32 x;
    s32 y;
    s32 z;
} Vec3;

extern s32 func_8001A4E8(s32 *a, s32 *b);
extern void gNormalizeVector(s32 *v) __asm__("func_8001A180");

/**
 * @brief Solves an aim direction by marching a ray toward the target and
 *        normalising the residual.
 *
 * @param target Target position.
 * @param out Receives the normalised aim vector.
 * @param arg2 Step size (<< 6 per iteration).
 * @param arg3 Maximum travel distance.
 * @param start Ray origin.
 * @param dir Ray direction (unit-ish).
 * @param scale Direction scale.
 * @return Nothing.
 *
 * @par Confirmed
 * The step vector is dir * scale >> 10; the loop runs at most 9 iterations
 * (starting at 8 when scale << 4 < arg2), advancing p and accumulating
 * travelled += arg2 << 6; it stops when travelled reaches the distance to
 * the target (func_8001A4E8) or arg3. The result is p - target - step/2,
 * normalised by func_8001A180.
 *
 * @par Likely role
 * The lead/deflection solver of the fire paths (func_8005F6E8,
 * gWeaponFireDispatch, gAttachPartEffect): marching toward the target and taking
 * the residual gives a smoothed aim direction that accounts for the
 * projectile travel; the half-step bias centres the estimate.
 *
 * @par Unknown
 * Why the iteration cap differs with the scale/step ratio.
 *
 * @par Callers
 * gAttachPartEffect, func_8005F6E8, gWeaponFireDispatch.
 */
void gSolveLeadDirection(Vec3 *target, Vec3 *out, s32 arg2, s32 arg3,
                   Vec3 *start, Vec3 *dir, s32 scale) __asm__("func_8005E2A0");

void gSolveLeadDirection(Vec3 *target, Vec3 *out, s32 arg2, s32 arg3,
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
    gNormalizeVector((s32 *)&p);

    *out = p;
}
