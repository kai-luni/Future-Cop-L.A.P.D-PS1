// Categories: Utilities > Mathematics > Vector | Graphics > Camera > Smoothing
#include "game/types.h"

extern int gGuidedProjectileUpdate();
void gEaseVectorQuarter(s32 *src, s32 *dst) __asm__("func_8001A6F8");


/**
 * @brief Vector low-pass filter: dst moves a quarter of the way toward src
 *        per call.
 *
 * @param src Target vector (three s32).
 * @param dst Filtered vector, updated in place.
 * @return Nothing.
 *
 * @par Confirmed
 * Per component dst = (dst * 3 + src) >> 2 — an exponential ease with a
 * 4-frame time constant, integer-truncated.
 *
 * @par Likely role
 * The position/heading smoothing of the mover and camera code
 * (func_800324E0, gGuidedProjectileUpdate, func_8003A160, func_800549F0): cheap
 * per-frame damping without state beyond the vector itself.
 *
 * @par Callers
 * func_800324E0, gGuidedProjectileUpdate, func_8003A160, func_800549F0.
 *
 * @par Environment path
 * Moves a three-component s32 vector a quarter of the way toward a target in
 * place: for each axis dst = (3*dst + src) >> 2, with no other state, globals
 * or calls. It is a generic per-frame damping helper used by
 * gUpdateHoverEnemy (func_800324E0), gGuidedProjectileUpdate (func_80035B7C),
 * gUpdateForceField (func_8003A160) and func_800549F0 according to the call
 * graph; none of its callers is camera code. In a .NET port it is a pure
 * function on three ints with integer (arithmetic-shift) truncation.
 */
void gEaseVectorQuarter(s32 *src, s32 *dst)
{
    register s32 d __asm__("$6") = dst[0]; dst[0] = (d * 3 + src[0]) >> 2;
    dst[1] = (dst[1] * 3 + src[1]) >> 2;
    dst[2] = (dst[2] * 3 + src[2]) >> 2;
}
