// Categories: Graphics > GTE > Transform | Platform > GTE
#include "game/types.h"
void gGteProjectPoint(u32 *src, u32 *dst) __asm__("func_80010704");


/**
 * @brief RTPS: perspective-transforms one vertex and returns the projected
 *        position together with its depth.
 *
 * @param src Pointer to the packed vertex (VXY0 at +0, VZ0 at +4).
 * @param dst Output block: SXY2 (projected screen XY) at +0, SZ3 (the
 *            interpolated screen depth) at +4.
 * @return In $v0: the GTE FLAG register.
 *
 * @par Confirmed mechanics
 * lwc2 $0 / lwc2 $1 load VXY0 and VZ0, two load-delay nops, `cop2 0x0180001`
 * is RTPS with sf = 1 (real opcode 01h), then swc2 $14 stores SXY2 and
 * swc2 $19 stores SZ3 before cfc2 $2,$31 reads FLAG.
 *
 * @par Compare
 * func_8001072C is the same transform but returns only SXY2 - the sibling that
 * the two renderer paths use when the depth is not needed.
 *
 * @par Callers
 * func_8006F310 and func_80070158.
 *
 * @par Environment path
 * Perspective-transforms one vertex with GTE RTPS using the currently loaded
 * rotation/translation and stores the screen XY (SXY2) and depth (SZ3) to the
 * output, returning the GTE FLAG register. Callers are gDrawTracerBand
 * (func_8006F310), which loads the camera matrix first, and gDrawGlowNodeList
 * (func_80070158). It is the projection primitive that turns camera-space
 * points into screen coordinates.
 */
void gGteProjectPoint(u32 *src, u32 *dst)
{
    __asm__ volatile(
        "lwc2 $0, 0($4)\n\t"
        "lwc2 $1, 4($4)\n\t"
        "nop\n\t"
        "nop\n\t"
        "cop2 0x0180001\n\t"
        "swc2 $14, 0($5)\n\t"
        "swc2 $19, 4($5)\n\t"
        "cfc2 $2, $31"
        :
        :
        : "$2", "memory");
}
