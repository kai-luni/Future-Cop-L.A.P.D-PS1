// Categories: Graphics > Culling | Graphics > Renderer
#include "game/types.h"

extern s32 D_8009FA0C;

/**
 * @brief Frustum visibility test for a point plus a safety margin.
 *
 * @param point Three s16 coordinates (x, y, z); z is the depth.
 * @param margin Extra extent added to the depth and the screen bounds.
 * @return 1 when the point is very close (depth + margin < 0x406),
 *         0 when it lies outside the horizontal or vertical bound,
 *         2 when it passes the vertical bound.
 *
 * @par Confirmed
 * depth = z << 4. The horizontal limit and the x scale come in two sets
 * selected by D_8009FA0C (the screen-width mode, 320 vs 160 pixels, see
 * func_800102A0): mode 1 uses margin << 4 and |x| << 8, otherwise
 * margin << 5 and |x| << 9. The vertical bound is
 * (depth << 4) - depth + (margin << 6) + (((margin*8 - margin)*16 - margin) >> 6),
 * compared against |y| << 10; the result is shifted left by one.
 *
 * @par Likely role
 * Culling test for extended objects: the margin keeps primitives whose
 * center is off-screen but whose extent reaches into the view.
 * func_80019D90 is the twin with a fixed near threshold.
 *
 * @par Callers
 * func_80013F08, func_80042128, func_8004A048, func_800592D0.
 */
s32 func_80019E70(s16 *point, s32 margin)
{
    s32 depth;
    s32 limit;
    s32 extentX;
    s32 extentY;

    depth = point[2] << 4;
    if (depth + margin < 0x406) {
        return 1;
    }
    if (D_8009FA0C == 1) {
        limit = depth + ((point[2] << 6) + (margin << 4) + ((margin + margin * 2) >> 2));
        extentX = point[0] << 8;
    } else {
        limit = depth + ((point[2] << 6) + (margin << 5) + ((margin + margin * 2) >> 3));
        extentX = point[0] << 9;
    }
    if (extentX < 0) {
        extentX = -extentX;
    }
    if (limit < extentX) {
        return 0;
    }
    extentY = point[1] << 10;
    if (extentY < 0) {
        extentY = -extentY;
    }
    depth = (depth << 4) - depth + (margin << 6) +
            (((margin * 8 - margin) * 16 - margin) >> 6);
    return (depth >= extentY) << 1;
}
