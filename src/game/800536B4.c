// Categories: Graphics > Renderer > Depth Cue Pass | Graphics > Effects
#include "game/types.h"

extern s32 gCameraNearBase __asm__("D_8009FA64");

/**
 * @brief Computes the near and far depth-cue scale factors from the
 *        global view distance.
 *
 * @param near Receives (D_8009FA64 << 8) / 608.
 * @param far Receives ((D_8009FA64 - 0x1A000) << 8) / 192 when the
 *            distance is >= 0x1A000, else 0.
 * @return Nothing.
 *
 * @par Confirmed
 * Two guarded fixed-point divisions of the distance word D_8009FA64;
 * the far term starts only beyond the 0x1A000 threshold.
 *
 * @par Likely role
 * Fog/depth-cue ramp for the multi-point polygon effects: 608 and 192
 * are the ramp slopes in 8.8 fixed point.
 *
 * @par Callers
 * func_800592D0.
 */
void func_800536B4(s32 *near, s32 *far)
{
    s32 distance;

    distance = gCameraNearBase;
    *near = (distance << 8) / 608;
    if (distance >= 0x1A000) {
        *far = ((distance - 0x1A000) << 8) / 192;
    } else {
        *far = 0;
    }
}
