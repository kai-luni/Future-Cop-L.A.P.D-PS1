// Categories: Graphics > Clipping | Graphics > 2D
#include "game/types.h"

extern s32 gPlayerCount __asm__("D_8009FA0C");

/**
 * @brief Tests whether a projected point still lies inside the visible area.
 *
 * The test runs in 16.16 fixed point. The z coordinate gives the depth
 * (point[2] << 4); anything closer than 0x601 after adding the margin is
 * rejected outright. The horizontal limit then grows with depth and margin,
 * and the x extent (point[0] << 8 or << 9 depending on D_8009FA0C) must not
 * exceed it. The same comparison is repeated for y with a wider extent, so
 * the accepted region is a depth-dependent diamond rather than a rectangle.
 *
 * @param point Signed 16.16 screen point; point[2] is the depth.
 * @param margin Extra radius in the same fixed-point scale.
 * @return 1 when the point is inside the visible region, 0 otherwise.
 *
 * @par Globals
 * D_8009FA0C - screen mode: 1 selects the narrow-pixel branch.
 *
 * @par Control flow
 * Straight-line comparisons, no loops; the early exits keep the fast path first.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8006411C, func_8006429C.
 */
s32 func_80063FF8(s16 *point, s32 margin)
{
    s32 depth;
    s32 limit;
    s32 extentX;
    s32 extentY;

    depth = point[2] << 4;
    if (depth + margin < 0x601) {
        return 0;
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
    return depth >= extentY;
}
