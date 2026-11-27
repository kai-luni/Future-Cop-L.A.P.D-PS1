// Categories: Graphics > GTE > Transform | Platform > GTE

void gGteProjectVertex(void *src, void *dst) __asm__("func_8001072C");

/**
 * @brief Projects one packed vertex and outputs the resulting screen
 * coordinates.
 *
 * @param src Packed vertex with VXY at +0 and VZ at +4.
 * @param dst Destination for the packed SXY2 word.
 * @return The retail routine leaves GTE FLAG in $v0. The reconstructed C entry
 *     has a void signature.
 *
 * @par Confirmed mechanics
 * Loads VXY0 and VZ0 via LWC2, issues RTPS, stores SXY2 through SWC2, and
 * copies FLAG to $v0 through CFC2.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * The retail listing has no direct call or tail jump to this entry.
 *
 * @par Role and path
 * This is a single-vertex perspective transform entry for the GTE renderer.
 *
 * @par Unknown
 * No caller has been established for this entry.
 */
void gGteProjectVertex(void *src, void *dst)
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
