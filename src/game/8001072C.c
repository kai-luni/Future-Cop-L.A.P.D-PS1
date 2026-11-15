// Categories: Graphics > GTE > Transform | Platform > GTE
/**
 * @brief RTPS: perspective-transforms one vertex and returns the projected
 *        screen position plus the GTE flag register.
 *
 * @param src Pointer to the packed vertex (VXY0 at +0, VZ0 at +4).
 * @param dst Pointer receiving the projected screen XY (SXY2, 4 bytes).
 * @return In $v0: the GTE FLAG register (cfc2 $2,$31), so the caller can test
 *         for overflow/saturation without a second command.
 *
 * @par Confirmed mechanics
 * lwc2 $0 / lwc2 $1 load VXY0 and VZ0, two nops, `cop2 0x0180001` is RTPS with
 * sf = 1 and identity matrix/vector (0x0), then swc2 $14 stores SXY2 and
 * cfc2 $2,$31 returns FLAG.
 *
 * @par Callers
 * func_8006F310 and func_80070158 - the per-vertex paths of the model
 * renderer.
 *
 * @par Sibling
 * func_80015F14 is the NCCS (colour shading) variant of the same pattern.
 */
void func_8001072C(void *src, void *dst)
{
    __asm__ volatile(
        "lwc2 $0, 0($4)\n\t"
        "lwc2 $1, 4($4)\n\t"
        "nop\n\t"
        "nop\n\t"
        "cop2 0x0180001\n\t"
        "swc2 $14, 0($5)\n\t"
        "cfc2 $2, $31"
        :
        :
        : "$2", "memory"
    );
}
