// Categories: Utilities > Mathematics > Vector | Graphics > Camera > Smoothing
#include "game/types.h"

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
 * (func_800324E0, func_80035B7C, func_8003A160, func_800549F0): cheap
 * per-frame damping without state beyond the vector itself.
 *
 * @par Callers
 * func_800324E0, func_80035B7C, func_8003A160, func_800549F0.
 */
void func_8001A6F8(s32 *src, s32 *dst)
{
    register s32 d __asm__("$6") = dst[0]; dst[0] = (d * 3 + src[0]) >> 2;
    dst[1] = (dst[1] * 3 + src[1]) >> 2;
    dst[2] = (dst[2] * 3 + src[2]) >> 2;
}
