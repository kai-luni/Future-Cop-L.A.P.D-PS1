// Categories: Graphics > Culling | Graphics > Renderer
#include "game/types.h"

extern s32 gPlayerCount __asm__("D_8009FA0C");

/**
 * @brief Frustum visibility test for a point plus a safety margin, with a
 *        fixed near threshold.
 *
 * @param point Three s16 coordinates (x, y, z); z is the depth.
 * @param margin Extra extent added to the screen bounds (not to the near
 *               test, unlike func_80019E70).
 * @return 1 when depth <= 0x1000, 0 when outside the horizontal or vertical
 *         bound, 2 when it passes the vertical bound.
 *
 * @par Confirmed
 * depth = z << 4. Two scale sets selected by D_8009FA0C (screen-width mode,
 * see func_800102A0): mode 1 uses margin << 4 and |x| << 8, otherwise
 * margin << 5 and |x| << 9. Vertical bound:
 * (depth << 4) - depth + (margin << 6) + (((margin*8 - margin)*16 - margin) >> 6)
 * against |y| << 10, result shifted left by one. Otherwise identical to
 * func_80019E70.
 *
 * @par Callers
 * func_8004A048, func_800597C0, func_80070158.
 *
 * @par Environment path
 * Frustum visibility test for one point plus a safety margin, with a fixed
 * near threshold: depth = z << 4 returns 1 as soon as depth <= 0x1000,
 * otherwise the horizontal bound is built from the screen-width mode in
 * D_8009FA0C (one player: margin << 4 and |x| << 8; otherwise margin << 5 and
 * |x| << 9) and the vertical bound is a fixed function of depth and margin,
 * the result of that last test shifted left by one so callers read 0 or 2. It
 * is the fixed-near twin of func_80019E70. It sits in the culling stage of
 * the environment pass: func_8004A048 - the map-cell node drawer reached from
 * gDrawCellWindowObjects (func_8004A924:91) - and func_800597C0 both call it,
 * as does the effect unit func_80070158:129.
 */
s32 func_80019D90(s16 *point, s32 margin)
{
    s32 depth;
    s32 limit;
    s32 extentX;
    s32 extentY;

    depth = point[2] << 4;
    if (depth <= 0x1000) {
        return 1;
    }
    if (gPlayerCount == 1) {
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
